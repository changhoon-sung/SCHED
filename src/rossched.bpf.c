#include <scx/common.bpf.h>

#include "dispatch_infer.h"
#include "enqueue_infer.h"
#include "intf.h"
#include "rl.h"
#include "select_cpu_infer.h"

char _license[] SEC("license") = "GPL";
const volatile bool debug = true;

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, u32);
    __type(value, u32);
    __uint(max_entries, 1000);
} e2e_status_map SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, u32);
    __type(value, int64_t);
    __uint(max_entries, 4);
} delay_map SEC(".maps");

struct state_action_record {
    u32 phase;
    u32 stage;
    u64 state;
    u32 action;
    u64 timestamp;
    u32 task_id;
};

struct cpu_state_t {
    u8 state;
};

struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __uint(max_entries, CPU_COUNT);
    __type(key, u32);
    __type(value, struct cpu_state_t);
} cpu_state_map SEC(".maps");

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);  // 256KB buffer
} state_action_records SEC(".maps");

/* Simple function for string comparison */
static __inline bool starts_with(const char *comm, const char *prefix) {
    /* Check if prefix matches the beginning of comm */
    int i = 0;
#pragma clang loop unroll(disable)
    for (i = 0; i < TASK_COMM_LEN; i++) {
        if (prefix[i] == '\0')
            return true; /* Success if prefix ends */
        if (comm[i] != prefix[i])
            return false; /* Mismatch if any character differs */
    }
    return false; /* Rarely exceeds TASK_COMM_LEN, but safe */
}

SEC("kprobe/do_idle")
int kprobe_do_idle(struct pt_regs *ctx) {
    u32 cpu = bpf_get_smp_processor_id();
    struct cpu_state_t *state;

    // Get the current state of the CPU
    state = bpf_map_lookup_elem(&cpu_state_map, &cpu);
    if (state) {
        state->state = 0;  // Idle
    }

    return 0;
}

SEC("tp/sched/sched_switch")
int handle_sched_switch(struct trace_event_raw_sched_switch *ctx) {
    u32 cpu = bpf_get_smp_processor_id();
    struct cpu_state_t *state;

    state = bpf_map_lookup_elem(&cpu_state_map, &cpu);
    if (state) {
        state->state = 1;  // Long Task
    }
    return 0;
}

static __inline bool
do_action_prob(u32 prob) {
    u32 rand = bpf_get_prandom_u32() & 0x63;  // 0x63 == 99
    return rand < prob;
}

// Min-Max normalization and integer scaling for a given array
// Float operations are not possible in the kernel, so handled together
static void normalize_and_int_scale_values(u32 *arr, u32 len, u32 min_vals[], u32 max_vals[], int normalized[]) {
    int i;
    // #pragma unroll
    for (i = 0; i < len; i++) {
        if (max_vals[i] == min_vals[i]) {
            normalized[i] = -INT_SCALE;
        } else {
            normalized[i] = (int)(2 * INT_SCALE * (arr[i] - min_vals[i])) / (max_vals[i] - min_vals[i]) - INT_SCALE;
        }

        // Clip the value
        if (normalized[i] < -INT_SCALE) {
            normalized[i] = -INT_SCALE;
        } else if (normalized[i] > INT_SCALE) {
            normalized[i] = INT_SCALE;
        }
    }
}

static void get_cpu_state_array(int res[RL_CPU_STATE_LEN]) {
    int i;
    struct cpu_state_t *cpu_state;
    u32 cpu = bpf_get_smp_processor_id();

    u32 cpu_array[CPU_COUNT];

#pragma unroll
    for (i = 0; i < CPU_COUNT; i++) {
        cpu_array[i] = i;
    }

#pragma unroll
    for (i = 0; i < CPU_COUNT; i++) {
        cpu_state = bpf_map_lookup_elem(&cpu_state_map, &cpu_array[i]);
        if (!cpu_state) {
            continue;
        }
        res[i] = (int)(cpu_state->state);
    }
    res[CPU_COUNT] = cpu;
}

static u64 get_encoded_cpu_state(void) {
    u64 state = 0;
    int i;
    struct cpu_state_t *cpu_state;
    u32 cpu = bpf_get_smp_processor_id();

    u32 cpu_array[CPU_COUNT];

#pragma unroll
    for (i = 0; i < CPU_COUNT; i++) {
        cpu_array[i] = i;
    }

#pragma unroll
    for (i = 0; i < CPU_COUNT; i++) {
        cpu_state = bpf_map_lookup_elem(&cpu_state_map, &cpu_array[i]);
        if (!cpu_state) {
            continue;
        }
        state = (state << RL_CPU_STATE_WIDTH) | (cpu_state->state & RL_CPU_STATE_MASK);
    }
    state = (state << RL_CPU_STATE_WIDTH) | (cpu & RL_CPU_STATE_MASK);

    return state;
}

// Helper function to decode CPU state
static void decode_cpu_state(u32 state) {
    int i;
    u32 cpu_state;

    for (i = 0; i < CPU_COUNT; i++) {
        // Extract the state of the corresponding CPU
        cpu_state = state & RL_CPU_STATE_MASK;

        // Debugging output
        bpf_printk("CPU %d decoded state: %u\n", i, cpu_state);

        // Move to the next CPU state
        state = state >> RL_CPU_STATE_WIDTH;
    }
}

static int select_cpu_for_task(u32 combined_state, struct task_struct *p) {
    int selected_cpu = -1;
    int i;
    u32 cpu_state;

    for (i = 0; i < CPU_COUNT; i++) {
        cpu_state = (combined_state >> (i * RL_CPU_STATE_WIDTH)) & RL_CPU_STATE_MASK;

        // Prefer idle CPUs
        if (cpu_state == 0) {
            selected_cpu = i;
            break;
        }

        // Next preference is CPUs handling short tasks
        if (cpu_state == 1 && selected_cpu == -1) {
            selected_cpu = i;
        }
    }

    return selected_cpu;
}

/*
 * Simple branching for DSQ ID based on task name (comm)
 *  - "camera_node", "dummy_camera_node", etc. -> DSQ_CAMERA
 *  - "radar_node" -> DSQ_RADAR
 *  - "imu_node", "canbus_polling_node", etc. -> DSQ_IMU
 *  - "planner_node" -> DSQ_PLANNER
 *  - "ultrasound_node" -> DSQ_ULTRA
 *  - Others -> DSQ_MISC
 */
static __always_inline u32 get_dsq_by_name(struct task_struct *p) {
    /* Buffer to store task name */
    char comm[TASK_COMM_LEN];
    long ret;

    /* Safely read task_struct->comm using CO-RE.
     * bpf_core_read_str() returns the length of the read string (including null) or an error (<0).
     */
    ret = bpf_core_read_str(comm, sizeof(comm), &p->comm);
    if (ret < 0) {
        /* Default to DSQ_MISC if reading fails */
        return DSQ_MISC;
    }

    /* Now that comm is populated, determine DSQ based on prefix */
    if (starts_with(comm, "imu")) {
        bpf_printk("Enqueue: imu");
        return DSQ_IMU;
    }
    if (starts_with(comm, "radar")) {
        bpf_printk("Enqueue: radar");
        return DSQ_RADAR;
    }
    // if (starts_with(comm, "camera_") || starts_with(comm, "dummy_camera_node"))
    if (starts_with(comm, "camera")) {
        bpf_printk("Enqueue: camera");
        return DSQ_CAMERA;
    }
    if (starts_with(comm, "planner")) {
        bpf_printk("Enqueue: planner");
        return DSQ_PLANNER;
    }
    if (starts_with(comm, "ultrasound")) {
        bpf_printk("Enqueue: ultrasound");
        return DSQ_ULTRA;
    }
    if (starts_with(comm, "dummy_camera")) {
        bpf_printk("Enqueue: dummy_camera");
        return DSQ_DUMMY_CAMERA;
    }
    if (starts_with(comm, "od")) {
        bpf_printk("Enqueue: od");
        return DSQ_OBJ_DETECTION;
    }
    if (starts_with(comm, "loc")) {
        bpf_printk("Enqueue: loc");
        return DSQ_LOC;
    }
    if (starts_with(comm, "canbus")) {
        bpf_printk("Enqueue: canbus");
        return DSQ_CANBUS;
    }
    if (starts_with(comm, "ekf")) {
        bpf_printk("Enqueue: ekf");
        return DSQ_EKF;
    }
    if (starts_with(comm, "ros")) {
        bpf_printk("Enqueue: ros");
        return DSQ_ROS_NODE;
    }
    /* Default to DSQ_MISC if no match */
    bpf_printk("Enqueue: misc %s", comm);
    return DSQ_MISC;
}

/* Report additional debugging information */
const volatile bool debug;

/*
 * Maximum task weight.
 */
#define MAX_TASK_WEIGHT 10000

/*
 * Maximum frequency of task wakeup events / sec.
 */
#define MAX_WAKEUP_FREQ 1024

/*
 * DSQ used to dispatch regular tasks.
 */
#define SHARED_DSQ 200

/*
 * Default task time slice.
 */
const volatile u64 slice_max = 20ULL * NSEC_PER_MSEC;

/*
 * Time slice used when system is over commissioned.
 */
const volatile u64 slice_min = 1ULL * NSEC_PER_MSEC;

/*
 * Maximum time slice lag.
 *
 * Increasing this value can help to increase the responsiveness of interactive
 * tasks at the cost of making regular and newly created tasks less responsive
 * (0 = disabled).
 */
const volatile s64 slice_lag = 20ULL * NSEC_PER_MSEC;

/*
 * When enabled always dispatch all kthreads directly.
 *
 * This allows to prioritize critical kernel threads that may potentially slow
 * down the entire system if they are blocked for too long, but it may also
 * introduce interactivity issues or unfairness in scenarios with high kthread
 * activity, such as heavy I/O or network traffic.
 */
const volatile bool local_kthreads;

/*
 * Maximum threshold of voluntary context switches.
 */
const volatile u64 nvcsw_max_thresh = 10ULL;

/*
 * The CPU frequency performance level: a negative value will not affect the
 * performance level and will be ignored.
 */
volatile s64 cpufreq_perf_lvl;

/*
 * Scheduling statistics.
 */
volatile u64 nr_kthread_dispatches, nr_direct_dispatches, nr_shared_dispatches;

/*
 * Amount of currently running tasks.
 */
volatile u64 nr_running, nr_interactive;

/*
 * Amount of online CPUs.
 */
volatile u64 nr_online_cpus;

/*
 * Exit information.
 */
UEI_DEFINE(uei);

/*
 * Mask of CPUs that the scheduler can use until the system becomes saturated,
 * at which point tasks may overflow to other available CPUs.
 */
private(BPFLAND) struct bpf_cpumask __kptr *primary_cpumask;

/*
 * CPUs in the system have SMT is enabled.
 */
const volatile bool smt_enabled = true;

/*
 * Current global vruntime.
 */
static u64 vtime_now;

/*
 * Per-CPU context.
 */
struct cpu_ctx {
    u64 tot_runtime;
    u64 prev_runtime;
    u64 last_running;
    struct bpf_cpumask __kptr *l2_cpumask;
    struct bpf_cpumask __kptr *l3_cpumask;
};

struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __type(key, u32);
    __type(value, struct cpu_ctx);
    __uint(max_entries, 1);
} cpu_ctx_stor SEC(".maps");

/*
 * Return a CPU context.
 */
struct cpu_ctx *try_lookup_cpu_ctx(s32 cpu) {
    const u32 idx = 0;
    return bpf_map_lookup_percpu_elem(&cpu_ctx_stor, &idx, cpu);
}

/*
 * Per-task local storage.
 *
 * This contain all the per-task information used internally by the BPF code.
 */
struct task_ctx {
    /*
     * Temporary cpumask for calculating scheduling domains.
     */
    struct bpf_cpumask __kptr *cpumask;
    struct bpf_cpumask __kptr *l2_cpumask;
    struct bpf_cpumask __kptr *l3_cpumask;

    /*
     * Voluntary context switches metrics.
     */
    u64 nvcsw;
    u64 nvcsw_ts;
    u64 avg_nvcsw;

    /*
     * Frequency with which a task is blocked (consumer).
     */
    u64 blocked_freq;
    u64 last_blocked_at;

    /*
     * Frequency with which a task wakes other tasks (producer).
     */
    u64 waker_freq;
    u64 last_woke_at;

    /*
     * Task's average used time slice.
     */
    u64 avg_runtime;
    u64 sum_runtime;
    u64 last_run_at;

    /*
     * Task's deadline.
     */
    u64 deadline;

    /*
     * Set to true if the task is classified as interactive.
     */
    bool is_interactive;
};

/* Map that contains task-local storage. */
struct {
    __uint(type, BPF_MAP_TYPE_TASK_STORAGE);
    __uint(map_flags, BPF_F_NO_PREALLOC);
    __type(key, int);
    __type(value, struct task_ctx);
} task_ctx_stor SEC(".maps");

/*
 * Return a local task context from a generic task.
 */
struct task_ctx *try_lookup_task_ctx(const struct task_struct *p) {
    return bpf_task_storage_get(&task_ctx_stor,
                                (struct task_struct *)p, 0, 0);
}

/*
 * Compare two vruntime values, returns true if the first value is less than
 * the second one.
 *
 * Copied from scx_simple.
 */
static inline bool vtime_before(u64 a, u64 b) {
    return (s64)(a - b) < 0;
}

/*
 * Return true if the target task @p is a kernel thread.
 */
static inline bool is_kthread(const struct task_struct *p) {
    return p->flags & PF_KTHREAD;
}

/*
 * Allocate/re-allocate a new cpumask.
 */
static int calloc_cpumask(struct bpf_cpumask **p_cpumask) {
    struct bpf_cpumask *cpumask;

    cpumask = bpf_cpumask_create();
    if (!cpumask)
        return -ENOMEM;

    cpumask = bpf_kptr_xchg(p_cpumask, cpumask);
    if (cpumask)
        bpf_cpumask_release(cpumask);

    return 0;
}

/*
 * Exponential weighted moving average (EWMA).
 *
 * Copied from scx_lavd. Returns the new average as:
 *
 *	new_avg := (old_avg * .75) + (new_val * .25);
 */
static u64 calc_avg(u64 old_val, u64 new_val) {
    return (old_val - (old_val >> 2)) + (new_val >> 2);
}

/*
 * Evaluate the EWMA limited to the range [low ... high]
 */
static u64 calc_avg_clamp(u64 old_val, u64 new_val, u64 low, u64 high) {
    return CLAMP(calc_avg(old_val, new_val), low, high);
}

/*
 * Evaluate the average frequency of an event over time.
 */
static u64 update_freq(u64 freq, u64 delta) {
    u64 new_freq;

    new_freq = NSEC_PER_SEC / delta;
    return calc_avg(freq, new_freq);
}

/*
 * Return the total amount of tasks that are currently waiting to be scheduled.
 */
static u64 nr_tasks_waiting(void) {
    return scx_bpf_dsq_nr_queued(SHARED_DSQ) + 1;
}

/*
 * Return task's dynamic weight.
 */
static u64 task_weight(const struct task_struct *p, const struct task_ctx *tctx) {
    /*
     * Scale the static task weight by the average amount of voluntary
     * context switches to determine the dynamic weight.
     */
    u64 prio = p->scx.weight * CLAMP(tctx->avg_nvcsw, 1, nvcsw_max_thresh);

    return CLAMP(prio, 1, MAX_TASK_WEIGHT);
}

/*
 * Return a value proportionally scaled to the task's priority.
 */
static u64 scale_up_fair(const struct task_struct *p,
                         const struct task_ctx *tctx, u64 value) {
    return value * task_weight(p, tctx) / 100;
}

/*
 * Return a value inversely proportional to the task's priority.
 */
static u64 scale_inverse_fair(const struct task_struct *p,
                              const struct task_ctx *tctx, u64 value) {
    return value * 100 / task_weight(p, tctx);
}

/*
 * Return the task's allowed lag: used to determine how early its vruntime can
 * be.
 */
static u64 task_lag(const struct task_struct *p, const struct task_ctx *tctx) {
    return scale_up_fair(p, tctx, slice_lag);
}

/*
 * ** Taken directly from fair.c in the Linux kernel **
 *
 * The "10% effect" is relative and cumulative: from _any_ nice level,
 * if you go up 1 level, it's -10% CPU usage, if you go down 1 level
 * it's +10% CPU usage. (to achieve that we use a multiplier of 1.25.
 * If a task goes up by ~10% and another task goes down by ~10% then
 * the relative distance between them is ~25%.)
 */
const int sched_prio_to_weight[40] = {
    /* -20 */ 88761,
    71755,
    56483,
    46273,
    36291,
    /* -15 */ 29154,
    23254,
    18705,
    14949,
    11916,
    /* -10 */ 9548,
    7620,
    6100,
    4904,
    3906,
    /*  -5 */ 3121,
    2501,
    1991,
    1586,
    1277,
    /*   0 */ 1024,
    820,
    655,
    526,
    423,
    /*   5 */ 335,
    272,
    215,
    172,
    137,
    /*  10 */ 110,
    87,
    70,
    56,
    45,
    /*  15 */ 36,
    29,
    23,
    18,
    15,
};

static u64 max_sched_prio(void) {
    return ARRAY_SIZE(sched_prio_to_weight);
}

/*
 * Convert task priority to weight (following fair.c logic).
 */
static u64 sched_prio_to_latency_weight(u64 prio) {
    u64 max_prio = max_sched_prio();

    if (prio >= max_prio) {
        scx_bpf_error("invalid priority");
        return 0;
    }

    return sched_prio_to_weight[max_prio - prio - 1];
}

/*
 * Evaluate task's deadline.
 *
 * Reuse a logic similar to scx_rusty or scx_lavd and evaluate the deadline as
 * a function of the waiting and wake-up events and the average task's runtime.
 */
static u64 task_deadline(struct task_struct *p, struct task_ctx *tctx) {
    u64 waker_freq, blocked_freq;
    u64 lat_prio, lat_weight;
    u64 avg_run_scaled, avg_run;
    u64 freq_factor;

    /*
     * Limit the wait and wake-up frequencies to prevent spikes.
     */
    waker_freq = CLAMP(tctx->waker_freq, 1, MAX_WAKEUP_FREQ);
    blocked_freq = CLAMP(tctx->blocked_freq, 1, MAX_WAKEUP_FREQ);

    /*
     * We want to prioritize producers (waker tasks) more than consumers
     * (blocked tasks), using the following formula:
     *
     *   freq_factor = blocked_freq * waker_freq^2
     *
     * This seems to improve the overall responsiveness of
     * producer/consumer pipelines.
     */
    freq_factor = blocked_freq * waker_freq * waker_freq;

    /*
     * Evaluate the "latency priority" as a function of the wake-up, block
     * frequencies and average runtime, using the following formula:
     *
     *   lat_prio = log(freq_factor / avg_run_scaled)
     *
     * Frequencies can grow very quickly, almost exponential, so use
     * log2_u64() to get a more linear metric that can be used as a
     * priority.
     *
     * The avg_run_scaled component is used to scale the latency priority
     * proportionally to the task's weight and inversely proportional to
     * its runtime, so that a task with a higher weight / shorter runtime
     * gets a higher latency priority than a task with a lower weight /
     * higher runtime.
     */
    avg_run_scaled = scale_inverse_fair(p, tctx, tctx->avg_runtime);
    avg_run = log2_u64(avg_run_scaled + 1);

    lat_prio = log2_u64(freq_factor);
    lat_prio = MIN(lat_prio, max_sched_prio());

    if (lat_prio >= avg_run)
        lat_prio -= avg_run;
    else
        lat_prio = 0;

    /*
     * Lastly, translate the latency priority into a weight and apply it to
     * the task's average runtime to determine the task's deadline.
     */
    lat_weight = sched_prio_to_latency_weight(lat_prio);

    return tctx->avg_runtime * 100 / lat_weight;
}

/*
 * Return task's evaluated deadline applied to its vruntime.
 */
static u64 task_vtime(struct task_struct *p, struct task_ctx *tctx) {
    u64 min_vruntime = vtime_now - task_lag(p, tctx);

    /*
     * Limit the vruntime to to avoid excessively penalizing tasks.
     */
    if (vtime_before(p->scx.dsq_vtime, min_vruntime)) {
        p->scx.dsq_vtime = min_vruntime;
        tctx->deadline = p->scx.dsq_vtime + task_deadline(p, tctx);
    }

    return tctx->deadline;
}

/*
 * Evaluate task's time slice in function of the total amount of tasks that are
 * waiting to be dispatched and the task's weight.
 */
static inline void task_refill_slice(struct task_struct *p) {
    /*
     * Scale the time slice of an inversely proportional factor of the
     * total amount of tasks that are waiting.
     */
    p->scx.slice = CLAMP(slice_max / nr_tasks_waiting(), slice_min, slice_max);
}

static void task_set_domain(struct task_struct *p, s32 cpu,
                            const struct cpumask *cpumask) {
    struct bpf_cpumask *primary, *l2_domain, *l3_domain;
    struct bpf_cpumask *p_mask, *l2_mask, *l3_mask;
    struct task_ctx *tctx;
    struct cpu_ctx *cctx;

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;

    cctx = try_lookup_cpu_ctx(cpu);
    if (!cctx)
        return;

    primary = primary_cpumask;
    if (!primary)
        return;

    l2_domain = cctx->l2_cpumask;
    if (!l2_domain)
        l2_domain = primary;

    l3_domain = cctx->l3_cpumask;
    if (!l3_domain)
        l3_domain = primary;

    p_mask = tctx->cpumask;
    if (!p_mask) {
        scx_bpf_error("cpumask not initialized");
        return;
    }

    l2_mask = tctx->l2_cpumask;
    if (!l2_mask) {
        scx_bpf_error("l2 cpumask not initialized");
        return;
    }

    l3_mask = tctx->l3_cpumask;
    if (!l3_mask) {
        scx_bpf_error("l3 cpumask not initialized");
        return;
    }

    /*
     * Determine the task's scheduling domain.
     * idle CPU, re-try again with the primary scheduling domain.
     */
    bpf_cpumask_and(p_mask, cpumask, cast_mask(primary));

    /*
     * Determine the L2 cache domain as the intersection of the task's
     * primary cpumask and the L2 cache domain mask of the previously used
     * CPU.
     */
    bpf_cpumask_and(l2_mask, cast_mask(p_mask), cast_mask(l2_domain));

    /*
     * Determine the L3 cache domain as the intersection of the task's
     * primary cpumask and the L3 cache domain mask of the previously used
     * CPU.
     */
    bpf_cpumask_and(l3_mask, cast_mask(p_mask), cast_mask(l3_domain));
}

/*
 * Find an idle CPU in the system.
 *
 * NOTE: the idle CPU selection doesn't need to be formally perfect, it is
 * totally fine to accept racy conditions and potentially make mistakes, by
 * picking CPUs that are not idle or even offline, the logic has been designed
 * to handle these mistakes in favor of a more efficient response and a reduced
 * scheduling overhead.
 */
static s32 pick_idle_cpu(struct task_struct *p, s32 prev_cpu, u64 wake_flags, bool *is_idle) {
    const struct cpumask *idle_smtmask, *idle_cpumask;
    const struct cpumask *primary, *p_mask, *l2_mask, *l3_mask;
    struct task_ctx *tctx;
    bool is_prev_llc_affine = false;
    s32 cpu;

    *is_idle = false;

    /*
     * For tasks that can run only on a single CPU, we can simply verify if
     * their only allowed CPU is still idle.
     */
    if (p->nr_cpus_allowed == 1 || p->migration_disabled) {
        if (scx_bpf_test_and_clear_cpu_idle(prev_cpu))
            *is_idle = true;
        return prev_cpu;
    }

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return -ENOENT;

    primary = cast_mask(primary_cpumask);
    if (!primary)
        return -EINVAL;

    /*
     * Acquire the CPU masks to determine the idle CPUs in the system.
     */
    idle_smtmask = scx_bpf_get_idle_smtmask();
    idle_cpumask = scx_bpf_get_idle_cpumask();

    /*
     * Task's scheduling domains.
     */
    p_mask = cast_mask(tctx->cpumask);
    if (!p_mask) {
        scx_bpf_error("cpumask not initialized");
        cpu = -EINVAL;
        goto out_put_cpumask;
    }

    l2_mask = cast_mask(tctx->l2_cpumask);
    if (!l2_mask) {
        scx_bpf_error("l2 cpumask not initialized");
        cpu = -EINVAL;
        goto out_put_cpumask;
    }

    l3_mask = cast_mask(tctx->l3_cpumask);
    if (!l3_mask) {
        scx_bpf_error("l3 cpumask not initialized");
        cpu = -EINVAL;
        goto out_put_cpumask;
    }

    /*
     * Check if the previously used CPU is still in the L3 task domain. If
     * not, we may want to move the task back to its original L3 domain.
     */
    is_prev_llc_affine = bpf_cpumask_test_cpu(prev_cpu, l3_mask);

    /*
     * If the current task is waking up another task and releasing the CPU
     * (WAKE_SYNC), attempt to migrate the wakee on the same CPU as the
     * waker.
     */
    if (wake_flags & SCX_WAKE_SYNC) {
        struct task_struct *current = (void *)bpf_get_current_task_btf();
        const struct cpumask *curr_l3_domain;
        struct cpu_ctx *cctx;
        bool share_llc, has_idle;

        /*
         * Determine waker CPU scheduling domain.
         */
        cpu = bpf_get_smp_processor_id();
        cctx = try_lookup_cpu_ctx(cpu);
        if (!cctx) {
            cpu = -EINVAL;
            goto out_put_cpumask;
        }

        curr_l3_domain = cast_mask(cctx->l3_cpumask);
        if (!curr_l3_domain)
            curr_l3_domain = primary;

        /*
         * If both the waker and wakee share the same L3 cache keep
         * using the same CPU if possible.
         */
        share_llc = bpf_cpumask_test_cpu(prev_cpu, curr_l3_domain);
        if (share_llc &&
            scx_bpf_test_and_clear_cpu_idle(prev_cpu)) {
            cpu = prev_cpu;
            *is_idle = true;
            goto out_put_cpumask;
        }

        /*
         * If the waker's L3 domain is not saturated attempt to migrate
         * the wakee on the same CPU as the waker (since it's going to
         * block and release the current CPU).
         */
        has_idle = bpf_cpumask_intersects(curr_l3_domain, idle_cpumask);
        if (has_idle &&
            bpf_cpumask_test_cpu(cpu, p_mask) &&
            !(current->flags & PF_EXITING) &&
            scx_bpf_dsq_nr_queued(SCX_DSQ_LOCAL_ON | cpu) == 0) {
            *is_idle = true;
            goto out_put_cpumask;
        }
    }

    /*
     * Find the best idle CPU, prioritizing full idle cores in SMT systems.
     */
    if (smt_enabled) {
        /*
         * If the task can still run on the previously used CPU and
         * it's a full-idle core, keep using it.
         */
        if (is_prev_llc_affine &&
            bpf_cpumask_test_cpu(prev_cpu, idle_smtmask) &&
            scx_bpf_test_and_clear_cpu_idle(prev_cpu)) {
            cpu = prev_cpu;
            *is_idle = true;
            goto out_put_cpumask;
        }

        /*
         * Search for any full-idle CPU in the primary domain that
         * shares the same L2 cache.
         */
        cpu = scx_bpf_pick_idle_cpu(l2_mask, SCX_PICK_IDLE_CORE);
        if (cpu >= 0) {
            *is_idle = true;
            goto out_put_cpumask;
        }

        /*
         * Search for any full-idle CPU in the primary domain that
         * shares the same L3 cache.
         */
        cpu = scx_bpf_pick_idle_cpu(l3_mask, SCX_PICK_IDLE_CORE);
        if (cpu >= 0) {
            *is_idle = true;
            goto out_put_cpumask;
        }

        /*
         * Search for any other full-idle core in the primary domain.
         */
        cpu = scx_bpf_pick_idle_cpu(p_mask, SCX_PICK_IDLE_CORE);
        if (cpu >= 0) {
            *is_idle = true;
            goto out_put_cpumask;
        }
    }

    /*
     * If a full-idle core can't be found (or if this is not an SMT system)
     * try to re-use the same CPU, even if it's not in a full-idle core.
     */
    if (is_prev_llc_affine &&
        scx_bpf_test_and_clear_cpu_idle(prev_cpu)) {
        cpu = prev_cpu;
        *is_idle = true;
        goto out_put_cpumask;
    }

    /*
     * Search for any idle CPU in the primary domain that shares the same
     * L2 cache.
     */
    cpu = scx_bpf_pick_idle_cpu(l2_mask, 0);
    if (cpu >= 0) {
        *is_idle = true;
        goto out_put_cpumask;
    }

    /*
     * Search for any idle CPU in the primary domain that shares the same
     * L3 cache.
     */
    cpu = scx_bpf_pick_idle_cpu(l3_mask, 0);
    if (cpu >= 0) {
        *is_idle = true;
        goto out_put_cpumask;
    }

    /*
     * Search for any idle CPU in the scheduling domain.
     */
    cpu = scx_bpf_pick_idle_cpu(p_mask, 0);
    if (cpu >= 0) {
        *is_idle = true;
        goto out_put_cpumask;
    }

    /*
     * We couldn't find any idle CPU, return the previous CPU if it is in
     * the task's L3 domain, otherwise pick any other CPU in the L3 domain.
     */
    if (is_prev_llc_affine)
        cpu = prev_cpu;
    else
        cpu = scx_bpf_pick_any_cpu(l3_mask, 0);

out_put_cpumask:
    scx_bpf_put_cpumask(idle_cpumask);
    scx_bpf_put_cpumask(idle_smtmask);

    /*
     * If we couldn't find any CPU, or in case of error, return the
     * previously used CPU.
     */
    if (cpu < 0)
        cpu = prev_cpu;

    return cpu;
}

/*
 * Pick a target CPU for a task which is being woken up.
 *
 * If a task is dispatched here, ops.enqueue() will be skipped: task will be
 * dispatched directly to the CPU returned by this callback.
 */
s32 BPF_STRUCT_OPS(rossched_select_cpu, struct task_struct *p,
                   s32 prev_cpu, u64 wake_flags) {
    s32 selected_cpu = prev_cpu;
    u32 e2e_stage = -1;

    if (!p)
        return -1;

    // Read the current e2e stage from e2e_status_map
    u32 key = E2E_LATENCY_KEY;
    u32 *e2e_status = bpf_map_lookup_elem(&e2e_status_map, &key);
    if (!e2e_status) {
        bpf_printk("enqueue: failed to get e2e status (not initialized?)\n");
    } else {
        e2e_stage = *e2e_status;
    }

    if (do_action_prob(ACTION_PROBABILITY)) {
        int cpu_states[RL_CPU_STATE_LEN];
        get_cpu_state_array(cpu_states);
        // for(int i = 0; i < RL_CPU_STATE_LEN; i++) {
        // 	bpf_printk("CPU %d raw state: %d\n", i, cpu_states[i]);
        // }

        // Set min-max range for normalization
        int mn[RL_CPU_STATE_LEN] = {
            0,
        };
        int mx[RL_CPU_STATE_LEN];
        for (int i = 0; i < CPU_COUNT; i++) {
            mx[i] = 1;  // 0 or 1
        }
        mx[CPU_COUNT] = CPU_COUNT - 1;  // Max value of the last array is the last CPU ID (0 ~ CPU_COUNT-1)

        int normalized_cpu_states[RL_CPU_STATE_LEN];
        normalize_and_int_scale_values(cpu_states, RL_CPU_STATE_LEN, mn, mx, normalized_cpu_states);
        // for(int i = 0; i < RL_CPU_STATE_LEN; i++) {
        // 	bpf_printk("select_cpu: CPU %d normalized state: %d\n", i, normalized_cpu_states[i]);
        // }

        selected_cpu = run_select_cpu_inference(normalized_cpu_states);
        // selected_cpu = select_cpu_for_task(rec->state, p);
    }

    if (SUBMIT_TO_BUFFER) {
        struct state_action_record *rec = bpf_ringbuf_reserve(
            &state_action_records, sizeof(*rec), 0);

        if (rec) {
            rec->phase = 0;  // 0 == select_cpu phase
            rec->stage = e2e_stage;
            rec->timestamp = bpf_ktime_get_ns();
            rec->state = get_encoded_cpu_state();
            rec->action = selected_cpu;
            rec->task_id = p->pid;
            bpf_ringbuf_submit(rec, 0);
        }
    }

    // TODO: MISC and Dummy to CPU0
    // u32 dsq_id = get_dsq_by_name(p);
    // if(dsq_id == DSQ_MISC || dsq_id == DSQ_DUMMY_CAMERA) {
    // // if(dsq_id == DSQ_MISC) {
    // 	selected_cpu = 0;	// misc dedicated
    // } else if (prev_cpu == 0) {
    // 	selected_cpu = (bpf_get_prandom_u32() % (CPU_COUNT - 1)) + 1;	// migrate to 1-11
    // }
    bpf_printk("select_cpu: prev CPU: %d, selected CPU: %d\n", prev_cpu, selected_cpu);
    return selected_cpu;
}

/*
 * Wake up an idle CPU for task @p.
 */
static void kick_task_cpu(struct task_struct *p) {
    s32 cpu = scx_bpf_task_cpu(p);
    bool is_idle = false;

    cpu = pick_idle_cpu(p, cpu, 0, &is_idle);
    if (is_idle)
        scx_bpf_kick_cpu(cpu, 0);
}

// Helper function to check if task name contains IRQ-related strings
static bool is_irq_task(const struct task_struct *p) {
    char comm[16];
    bpf_probe_read_kernel_str(&comm, sizeof(comm), p->comm);

    // Direct string comparison for each character
    // Check for "irq/"
    if (comm[0] == 'i' && comm[1] == 'r' && comm[2] == 'q' && comm[3] == '/')
        return true;

    // Check for "softirq"
    if (comm[0] == 's' && comm[1] == 'o' && comm[2] == 'f' && comm[3] == 't' &&
        comm[4] == 'i' && comm[5] == 'r' && comm[6] == 'q')
        return true;

    // Check for "sirq-"
    if (comm[0] == 's' && comm[1] == 'i' && comm[2] == 'r' && comm[3] == 'q' &&
        comm[4] == '-')
        return true;

    // Check for "watchdog"
    if (comm[0] == 'w' && comm[1] == 'a' && comm[2] == 't' && comm[3] == 'c' &&
        comm[4] == 'h' && comm[5] == 'd' && comm[6] == 'o' && comm[7] == 'g')
        return true;

    // Check for "ksoftirqd"
    if (comm[0] == 'k' && comm[1] == 's' && comm[2] == 'o' && comm[3] == 'f' &&
        comm[4] == 't' && comm[5] == 'i' && comm[6] == 'r' && comm[7] == 'q' &&
        comm[8] == 'd')
        return true;

    return false;
}

static bool match_comm(const char *comm, const char *target, int len) {
    for (int i = 0; i < len; i++) {
        if (comm[i] != target[i])
            return false;
        if (comm[i] == '\0')
            break;
    }
    return true;
}

static u32 get_task_type(const struct task_struct *p) {
    char comm[16];
    bpf_probe_read_kernel_str(&comm, sizeof(comm), p->comm);

    // Check for IRQ tasks first
    if (comm[0] == 'i' && comm[1] == 'r' && comm[2] == 'q' && comm[3] == '/')
        return 4;
    if (comm[0] == 'k' && comm[1] == 's' && comm[2] == 'o' && comm[3] == 'f' &&
        comm[4] == 't' && comm[5] == 'i' && comm[6] == 'r' && comm[7] == 'q')
        return 4;

// Check for our pipeline tasks
#pragma unroll
    for (int i = 0; i < 8; i++) {
        if (i == 0 && comm[i] == 'l') {
            if (match_comm(&comm[i], "loc_node", 8))
                return 0;
        } else if (i == 0 && comm[i] == 'c') {
            if (match_comm(&comm[i], "canbus_polling_node", 18))
                return 1;
        } else if (i == 0 && comm[i] == 'e') {
            if (match_comm(&comm[i], "ekf_node", 8))
                return 2;
        }
    }

    return 3;  // other tasks
}

static u32 normalize_delay(int64_t delay_us) {
    // Convert to unsigned before division
    u64 abs_delay;
    bool is_negative = delay_us < 0;

    // Get absolute value
    if (is_negative) {
        abs_delay = (u64)(-delay_us);
    } else {
        abs_delay = (u64)delay_us;
    }

    // Unsigned division for microseconds to milliseconds
    u32 delay_ms = (u32)(abs_delay / 1000);

    // Apply sign back and clamp
    if (is_negative) {
        delay_ms = (delay_ms > 5000) ? 5000 : delay_ms;
        return 5000 - delay_ms;  // shift negative values to 0~5000 range
    } else {
        delay_ms = (delay_ms > 5000) ? 5000 : delay_ms;
        return 5000 + delay_ms;  // shift positive values to 5000~10000 range
    }
}

static u32 get_delay_level(u32 e2e_stage) {
    int64_t *delays = bpf_map_lookup_elem(&delay_map, &e2e_stage);
    if (!delays) {
        bpf_printk("Failed to get delay for stage %u\n", e2e_stage);
        return 5000;
    }

    u32 normalized_delay = normalize_delay(*delays);
    bpf_printk("Raw delay: %lld, Normalized delay: %u\n", *delays, normalized_delay);

    return normalized_delay;
}

// Helper function to calculate state index for Q-learning
static u64 get_state_index(const struct task_struct *p, u32 dsq_id) {
    u32 task_type = dsq_id;

    // Read the current e2e stage from e2e_status_map
    u32 key = E2E_LATENCY_KEY;
    u32 *e2e_status = bpf_map_lookup_elem(&e2e_status_map, &key);
    if (!e2e_status) {
        bpf_printk("Failed to get e2e status\n");
        return 0;
    }
    u32 e2e_stage = *e2e_status;

    // Get delay_level corresponding to the current e2e stage
    u32 delay_level = get_delay_level(e2e_stage);

    /*
     * Combine each value into bit fields:
     * - Lowest DELAY_BITS bits for delay_level
     * - Next E2E_STAGE_BITS bits for e2e_stage
     * - Remaining higher bits for task_type
     */
    u64 state = ((u64)task_type << (DELAY_BITS + E2E_STAGE_BITS)) |
                ((u64)e2e_stage << DELAY_BITS) |
                (delay_level);

    bpf_printk("State calculation: task_type=%u, e2e_stage=%u, delay_level=%u -> state=%llu",
               task_type, e2e_stage, delay_level, state);

    return state;
}
/*
 * Dispatch all the other tasks that were not dispatched directly in
 * select_cpu().
 */
void BPF_STRUCT_OPS(rossched_enqueue, struct task_struct *p, u64 enq_flags) {
    u32 dsq_id;
    u32 reason = 0, action = -1;
    u32 e2e_stage = -1;
    struct task_ctx *tctx;
    struct state_action_record *rec;

    tctx = try_lookup_task_ctx(p);

    if (!tctx) {
        goto enqueue_end;
    }

    if (!p) {
        goto enqueue_end;
    }

    if (do_action_prob(ACTION_PROBABILITY)) {
        int enqueue_status[RL_ENQUEUE_STATE_LEN] = {
            0,
        };

        // Read the current e2e stage from e2e_status_map
        u32 key = E2E_LATENCY_KEY;
        u32 *e2e_status = bpf_map_lookup_elem(&e2e_status_map, &key);
        if (!e2e_status) {
            bpf_printk("enqueue: failed to get e2e status (not initialized?)\n");
            goto default_action;
        }
        e2e_stage = *e2e_status;

        // Get delay_level corresponding to the current e2e stage
        u32 delay_level = get_delay_level(e2e_stage);

        enqueue_status[0] = get_dsq_by_name(p);
        enqueue_status[1] = e2e_stage;
        enqueue_status[2] = delay_level;

        int mn[RL_ENQUEUE_STATE_LEN] = {
            0,
        };
        int mx[RL_ENQUEUE_STATE_LEN] = {
            DSQ_MAX_ID,
            E2E_STAGE_PLANNER,
            MAX_DELAY,
            RL_ENQUEUE_PADDING,
            RL_ENQUEUE_PADDING,
            RL_ENQUEUE_PADDING,
        };

        int normalized_enqueue_status[RL_ENQUEUE_STATE_LEN];
        normalize_and_int_scale_values(enqueue_status, RL_ENQUEUE_STATE_LEN, mn, mx, normalized_enqueue_status);
        action = run_enqueue_inference(normalized_enqueue_status);

        switch (action) {
            case RL_ENQUEUE_ACTION_DIRECT_DISPATCH:
                scx_bpf_dispatch(p, SCX_DSQ_LOCAL, SCX_SLICE_DFL,
                                 enq_flags | SCX_ENQ_PREEMPT);
                reason = RL_ENQUEUE_ACTION_DIRECT_DISPATCH;
                break;
            case RL_ENQUEUE_ACTION_INSERT_DSQ:
                dsq_id = get_dsq_by_name(p);
                scx_bpf_dispatch_vtime(p, dsq_id, SCX_SLICE_DFL, task_vtime(p, tctx), enq_flags);
                reason = RL_ENQUEUE_ACTION_INSERT_DSQ;
                break;
            default:
                // should not reach here
                bpf_printk("Error: Invalid enqueue action %u\n", action);
                break;
        }
        goto enqueue_submit;

    } else {
    default_action:
        dsq_id = get_dsq_by_name(p);
        if ((is_kthread(p) && (local_kthreads || p->nr_cpus_allowed == 1)) || dsq_id == DSQ_OBJ_DETECTION) {
            scx_bpf_dispatch(p, SCX_DSQ_LOCAL, SCX_SLICE_DFL,
                             enq_flags | SCX_ENQ_PREEMPT);
            reason = RL_ENQUEUE_ACTION_DIRECT_DISPATCH;
            goto enqueue_submit;
        }

        dsq_id = get_dsq_by_name(p);
        scx_bpf_dispatch_vtime(p, dsq_id, SCX_SLICE_DFL, task_vtime(p, tctx), enq_flags);
        reason = RL_ENQUEUE_ACTION_INSERT_DSQ;
    }

enqueue_submit:
    if (SUBMIT_TO_BUFFER) {
        rec = bpf_ringbuf_reserve(&state_action_records, sizeof(*rec), 0);
        if (rec) {
            rec->phase = 1;  // 1 => Enqueue agent
            rec->stage = e2e_stage;
            rec->timestamp = bpf_ktime_get_ns();
            rec->state = get_state_index(p, dsq_id);
            rec->action = reason;
            rec->task_id = p->pid;
            // ring buffer submission
            bpf_ringbuf_submit(rec, 0);
        }
    }

    // Additional logging for submission outcome
    bpf_printk("Enqueue submit completed. Reason: %u\n", reason);

    if (p) {
        reason = is_kthread(p) && (local_kthreads || p->nr_cpus_allowed == 1) ? 0 : 1;
        bpf_printk("enqueue: default reason: %u, inferred reason: %u\n", reason, action);
    }

enqueue_end:
    // Wake up idle CPU if needed
    kick_task_cpu(p);
    return;
}

static int dsq_id_to_idx(int dsq_id) {
    switch (dsq_id) {
        case DSQ_IMU:
            return 0;
        case DSQ_RADAR:
            return 1;
        case DSQ_CAMERA:
            return 2;
        case DSQ_PLANNER:
            return 3;
        case DSQ_ULTRA:
            return 4;
        case DSQ_LOC:
            return 5;
        case DSQ_OBJ_DETECTION:
            return 6;
        case DSQ_CANBUS:
            return 7;
        case DSQ_EKF:
            return 8;
        case DSQ_ROS_NODE:
            return 9;
        case DSQ_MISC:
            return 10;
        case DSQ_DUMMY_CAMERA:
            return 11;
        default:
            return -1;
    }
}

int BPF_STRUCT_OPS(rossched_dispatch, s32 cpu, struct task_struct *prev) {
    u32 reason = 0;
    u32 e2e_stage = -1;
    struct state_action_record *rec;

    // Read the current e2e stage from e2e_status_map
    u32 key = E2E_LATENCY_KEY;
    u32 *e2e_status = bpf_map_lookup_elem(&e2e_status_map, &key);
    if (!e2e_status) {
        bpf_printk("enqueue: failed to get e2e status (not initialized?)\n");
        goto default_action;
    } else {
        e2e_stage = *e2e_status;
    }

    if (do_action_prob(ACTION_PROBABILITY)) {
        u32 dsq_ids[DSQ_ORDER_LEN] = DSQ_ORDER;
        int dsq_lens[DSQ_ORDER_LEN];

        scx_bpf_consume(DSQ_OBJ_DETECTION);

#pragma unroll
        for (int i = 0; i < DSQ_ORDER_LEN; i++) {
            dsq_lens[i] = scx_bpf_dsq_nr_queued(dsq_ids[i]);
        }

        // Set min-max range for normalization
        int mn[RL_DISPATCH_STATE_LEN] = {
            0,
        };
        int mx[RL_DISPATCH_STATE_LEN];
#pragma unroll
        for (int i = 0; i < RL_DISPATCH_STATE_LEN; i++) {
            mx[i] = DSQ_MAX_BUSY;
        }

        int normalized_dsq_lens[RL_DISPATCH_STATE_LEN];
        int inference_output[RL_DISPATCH_ACTION_LEN];
        normalize_and_int_scale_values(dsq_lens, RL_DISPATCH_STATE_LEN, mn, mx, normalized_dsq_lens);

        run_dispatch_inference(normalized_dsq_lens, inference_output);

        // // Full sorting version
        // int dsq_prio[DSQ_ORDER_LEN] = DSQ_CONSUME_ORDER;
        // #pragma unroll
        // for(int i = 0; i < DSQ_ORDER_LEN; i++) {
        // 	if (scx_bpf_consume(dsq_prio[i])) {
        // 		reason = dsq_prio[i];
        // 		goto dispatch_end;
        // 	}
        // }

        // Select non-zero values
        int len = 0;
        int selected_dsq_idx[DSQ_ORDER_LEN];
        int selected_output[DSQ_ORDER_LEN];
        for (int i = 0; i < DSQ_ORDER_LEN; i++) {
            if (inference_output[i] > 0) {
                selected_dsq_idx[len] = i;
                selected_output[len] = inference_output[i];
                len++;
            }
        }
        selection_sort_with_idx(selected_output, selected_dsq_idx, len);

        // Among selected DSQs, prioritize higher probabilities
        for (int i = 0; i < len; i++) {
            if (0 <= selected_dsq_idx[i] && selected_dsq_idx[i] < DSQ_ORDER_LEN && scx_bpf_consume(dsq_ids[selected_dsq_idx[i]])) {
                reason = selected_dsq_idx[i];
                goto dispatch_submit;
            }
        }
        // If all selected DSQs are empty, prioritize DSQs with probability 0
        int dsq_prio[DSQ_ORDER_LEN] = DSQ_CONSUME_ORDER;
#pragma unroll
        for (int i = 0; i < DSQ_ORDER_LEN; i++) {
            if (inference_output[dsq_id_to_idx(dsq_prio[i])] == 0 && scx_bpf_consume(dsq_prio[i])) {
                reason = dsq_prio[i];
                goto dispatch_submit;
            }
        }

        // If all DSQs are empty
        bpf_printk("dispatch: All DSQ are emtpy!\n");
        if (prev && (prev->scx.flags & SCX_TASK_QUEUED)) {
            reason = 12;
            int x = inference_output[DSQ_ORDER_LEN];
            if (x > 0) {
                task_refill_slice(prev);
                bpf_printk("dispatch: prev refilled\n");
            }
        }
        goto dispatch_submit;
    } else {
    default_action:
        if (scx_bpf_consume(DSQ_OBJ_DETECTION)) {
            reason = 6;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_MISC)) {
            reason = 10;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_PLANNER)) {
            reason = 3;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_ULTRA)) {
            reason = 4;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_LOC)) {
            reason = 5;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_CANBUS)) {
            reason = 7;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_EKF)) {
            reason = 8;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_ROS_NODE)) {
            reason = 9;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_CAMERA)) {
            reason = 2;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_IMU)) {
            reason = 0;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_RADAR)) {
            reason = 1;
            goto dispatch_submit;
        }
        if (scx_bpf_consume(DSQ_DUMMY_CAMERA)) {
            reason = 11;
            goto dispatch_submit;
        }
        /*
         * If the current task expired its time slice and no other task wants
         * to run, simply replenish its time slice and let it run for another
         * round on the same CPU.
         */
        if (prev && (prev->scx.flags & SCX_TASK_QUEUED)) {
            reason = 12;
            goto dispatch_submit;
        }
    }

dispatch_submit:
    if (SUBMIT_TO_BUFFER) {
        rec = bpf_ringbuf_reserve(&state_action_records, sizeof(*rec), 0);
        if (rec) {
            rec->phase = 2;  // 2 => Dispatch agent
            rec->stage = e2e_stage;
            rec->timestamp = bpf_ktime_get_ns();
            rec->action = reason;
            rec->task_id = prev ? prev->pid : 0;
            u32 dsq_ids[DSQ_ORDER_LEN] = DSQ_ORDER;
            u64 total_dsq_len = 0;
#pragma unroll
            for (int i = 0; i < DSQ_ORDER_LEN; i++) {
                u32 qlen = scx_bpf_dsq_nr_queued(dsq_ids[i]);
                total_dsq_len = (total_dsq_len << DSQ_WIDTH) | (qlen & DSQ_MASK);
            }
            rec->state = total_dsq_len;
            bpf_ringbuf_submit(rec, 0);
        }
    }

    bpf_printk("dispatch: consumed DSQ %u\n", reason);
    return 0;
}

/*
 * Scale target CPU frequency based on the performance level selected
 * from user-space and the CPU utilization.
 */
static void update_cpuperf_target(struct task_struct *p, struct task_ctx *tctx) {
    u64 now = bpf_ktime_get_ns();
    s32 cpu = scx_bpf_task_cpu(p);
    u64 perf_lvl, delta_runtime, delta_t;
    struct cpu_ctx *cctx;

    if (cpufreq_perf_lvl >= 0) {
        /*
         * Apply fixed cpuperf scaling factor determined by user-space.
         */
        perf_lvl = MIN(cpufreq_perf_lvl, SCX_CPUPERF_ONE);
        scx_bpf_cpuperf_set(cpu, perf_lvl);
        return;
    }

    /*
     * Auto mode: always tset max performance for interactive tasks.
     */
    if (tctx->is_interactive) {
        scx_bpf_cpuperf_set(cpu, SCX_CPUPERF_ONE);
        return;
    }

    /*
     * For non-interactive tasks determine their cpufreq scaling factor as
     * a function of their CPU utilization.
     */
    cctx = try_lookup_cpu_ctx(cpu);
    if (!cctx)
        return;

    /*
     * Evaluate dynamic cpuperf scaling factor using the average CPU
     * utilization, normalized in the range [0 .. SCX_CPUPERF_ONE].
     */
    delta_t = now - cctx->last_running;
    delta_runtime = cctx->tot_runtime - cctx->prev_runtime;
    perf_lvl = MIN(delta_runtime * SCX_CPUPERF_ONE / delta_t, SCX_CPUPERF_ONE);

    /*
     * Apply the dynamic cpuperf scaling factor.
     */
    scx_bpf_cpuperf_set(cpu, perf_lvl);

    cctx->last_running = bpf_ktime_get_ns();
    cctx->prev_runtime = cctx->tot_runtime;
}

void BPF_STRUCT_OPS(rossched_running, struct task_struct *p) {
    struct task_ctx *tctx;

    __sync_fetch_and_add(&nr_running, 1);

    /*
     * Refresh task's time slice immediately before it starts to run on its
     * assigned CPU.
     */
    task_refill_slice(p);

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;
    tctx->last_run_at = bpf_ktime_get_ns();

    /*
     * Adjust target CPU frequency before the task starts to run.
     */
    update_cpuperf_target(p, tctx);

    /*
     * Update CPU interactive state.
     */
    if (tctx->is_interactive)
        __sync_fetch_and_add(&nr_interactive, 1);

    /*
     * Update global vruntime.
     */
    if (vtime_before(vtime_now, p->scx.dsq_vtime))
        vtime_now = p->scx.dsq_vtime;
}

/*
 * Update task statistics when the task is releasing the CPU (either
 * voluntarily or because it expires its assigned time slice).
 */
void BPF_STRUCT_OPS(rossched_stopping, struct task_struct *p, bool runnable) {
    u64 now = bpf_ktime_get_ns(), slice;
    s32 cpu = scx_bpf_task_cpu(p);
    s64 delta_t;
    struct cpu_ctx *cctx;
    struct task_ctx *tctx;

    cctx = try_lookup_cpu_ctx(cpu);
    if (cctx)
        cctx->tot_runtime += now - cctx->last_running;

    __sync_fetch_and_sub(&nr_running, 1);

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;

    if (tctx->is_interactive)
        __sync_fetch_and_sub(&nr_interactive, 1);

    /*
     * If the time slice is not fully depleted, it means that the task
     * voluntarily relased the CPU, therefore update the voluntary context
     * switch counter.
     *
     * NOTE: the sched_ext core implements sched_yield() by setting the
     * time slice to 0, so we won't boost the priority of tasks that are
     * explicitly calling sched_yield().
     *
     * This is actually a good thing, because we want to prioritize tasks
     * that are releasing the CPU, because they're doing I/O, waiting for
     * input or sending output to other tasks.
     *
     * Tasks that are using sched_yield() don't really need the priority
     * boost and when they get the chance to run again they will be
     * naturally prioritized by the vruntime-based scheduling policy.
     */
    if (p->scx.slice > 0)
        tctx->nvcsw++;

    /*
     * Update task's average runtime.
     */
    slice = now - tctx->last_run_at;
    tctx->sum_runtime += slice;
    tctx->avg_runtime = calc_avg(tctx->avg_runtime, tctx->sum_runtime);

    /*
     * Update task vruntime charging the weighted used time slice.
     */
    p->scx.dsq_vtime += scale_inverse_fair(p, tctx, slice);
    tctx->deadline = p->scx.dsq_vtime + task_deadline(p, tctx);

    /*
     * Refresh voluntary context switch metrics.
     *
     * Evaluate the average number of voluntary context switches per second
     * using an exponentially weighted moving average, see calc_avg().
     */
    delta_t = (s64)(now - tctx->nvcsw_ts);
    if (delta_t > NSEC_PER_SEC) {
        u64 avg_nvcsw = tctx->nvcsw * NSEC_PER_SEC / delta_t;
        u64 max_nvcsw = nvcsw_max_thresh * 100;

        tctx->nvcsw = 0;
        tctx->nvcsw_ts = now;

        /*
         * Evaluate the latency weight of the task as its average rate
         * of voluntary context switches (limited to to prevent
         * excessive spikes).
         */
        tctx->avg_nvcsw = calc_avg_clamp(tctx->avg_nvcsw, avg_nvcsw, 0, max_nvcsw);

        /*
         * Classify the task based on the average of voluntary context
         * switches.
         *
         * If the task has an average greater than the global average
         * it is classified as interactive, otherwise the task is
         * classified as regular.
         */
        tctx->is_interactive = tctx->avg_nvcsw >= nvcsw_max_thresh;
    }
}

void BPF_STRUCT_OPS(rossched_runnable, struct task_struct *p, u64 enq_flags) {
    u64 now = bpf_ktime_get_ns(), delta;
    struct task_struct *waker;
    struct task_ctx *tctx;

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;
    tctx->sum_runtime = 0;

    waker = bpf_get_current_task_btf();
    tctx = try_lookup_task_ctx(waker);
    if (!tctx)
        return;

    delta = MAX(now - tctx->last_woke_at, 1);
    tctx->waker_freq = update_freq(tctx->waker_freq, delta);
    tctx->last_woke_at = now;
}

void BPF_STRUCT_OPS(rossched_quiescent, struct task_struct *p, u64 deq_flags) {
    u64 now = bpf_ktime_get_ns(), delta;
    struct task_ctx *tctx;

    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;

    delta = MAX(now - tctx->last_blocked_at, 1);
    tctx->blocked_freq = update_freq(tctx->blocked_freq, delta);
    tctx->last_blocked_at = now;
}

void BPF_STRUCT_OPS(rossched_set_cpumask, struct task_struct *p,
                    const struct cpumask *cpumask) {
    s32 cpu = bpf_get_smp_processor_id();

    task_set_domain(p, cpu, cpumask);
}

void BPF_STRUCT_OPS(rossched_enable, struct task_struct *p) {
    u64 now = bpf_ktime_get_ns();
    struct task_ctx *tctx;

    /* Initialize task's vruntime */
    p->scx.dsq_vtime = vtime_now;

    /* Initialize voluntary context switch timestamp */
    tctx = try_lookup_task_ctx(p);
    if (!tctx)
        return;
    tctx->nvcsw_ts = now;
    tctx->last_woke_at = now;
    tctx->last_blocked_at = now;

    tctx->deadline = p->scx.dsq_vtime + task_deadline(p, tctx);
}

s32 BPF_STRUCT_OPS(rossched_init_task, struct task_struct *p,
                   struct scx_init_task_args *args) {
    s32 cpu = bpf_get_smp_processor_id();
    struct task_ctx *tctx;
    struct bpf_cpumask *cpumask;

    tctx = bpf_task_storage_get(&task_ctx_stor, p, 0,
                                BPF_LOCAL_STORAGE_GET_F_CREATE);
    if (!tctx)
        return -ENOMEM;
    /*
     * Create task's primary cpumask.
     */
    cpumask = bpf_cpumask_create();
    if (!cpumask)
        return -ENOMEM;
    cpumask = bpf_kptr_xchg(&tctx->cpumask, cpumask);
    if (cpumask)
        bpf_cpumask_release(cpumask);
    /*
     * Create task's L2 cache cpumask.
     */
    cpumask = bpf_cpumask_create();
    if (!cpumask)
        return -ENOMEM;
    cpumask = bpf_kptr_xchg(&tctx->l2_cpumask, cpumask);
    if (cpumask)
        bpf_cpumask_release(cpumask);
    /*
     * Create task's L3 cache cpumask.
     */
    cpumask = bpf_cpumask_create();
    if (!cpumask)
        return -ENOMEM;
    cpumask = bpf_kptr_xchg(&tctx->l3_cpumask, cpumask);
    if (cpumask)
        bpf_cpumask_release(cpumask);

    task_set_domain(p, cpu, p->cpus_ptr);

    return 0;
}

/*
 * Evaluate the amount of online CPUs.
 */
s32 get_nr_online_cpus(void) {
    const struct cpumask *online_cpumask;
    int cpus;

    online_cpumask = scx_bpf_get_online_cpumask();
    cpus = bpf_cpumask_weight(online_cpumask);
    scx_bpf_put_cpumask(online_cpumask);

    return cpus;
}

static int init_cpumask(struct bpf_cpumask **cpumask) {
    struct bpf_cpumask *mask;
    int err = 0;

    /*
     * Do nothing if the mask is already initialized.
     */
    mask = *cpumask;
    if (mask)
        return 0;
    /*
     * Create the CPU mask.
     */
    err = calloc_cpumask(cpumask);
    if (!err)
        mask = *cpumask;
    if (!mask)
        err = -ENOMEM;

    return err;
}

SEC("syscall")
int enable_sibling_cpu(struct domain_arg *input) {
    struct cpu_ctx *cctx;
    struct bpf_cpumask *mask, **pmask;
    int err = 0;

    cctx = try_lookup_cpu_ctx(input->cpu_id);
    if (!cctx)
        return -ENOENT;

    /* Make sure the target CPU mask is initialized */
    switch (input->lvl_id) {
        case 2:
            pmask = &cctx->l2_cpumask;
            break;
        case 3:
            pmask = &cctx->l3_cpumask;
            break;
        default:
            return -EINVAL;
    }
    err = init_cpumask(pmask);
    if (err)
        return err;

    bpf_rcu_read_lock();
    mask = *pmask;
    if (mask)
        bpf_cpumask_set_cpu(input->sibling_cpu_id, mask);
    bpf_rcu_read_unlock();

    return err;
}

SEC("syscall")
int enable_primary_cpu(struct cpu_arg *input) {
    struct bpf_cpumask *mask;
    int err = 0;

    /* Make sure the primary CPU mask is initialized */
    err = init_cpumask(&primary_cpumask);
    if (err)
        return err;
    /*
     * Enable the target CPU in the primary scheduling domain. If the
     * target CPU is a negative value, clear the whole mask (this can be
     * used to reset the primary domain).
     */
    bpf_rcu_read_lock();
    mask = primary_cpumask;
    if (mask) {
        s32 cpu = input->cpu_id;

        if (cpu < 0)
            bpf_cpumask_clear(mask);
        else
            bpf_cpumask_set_cpu(cpu, mask);
    }
    bpf_rcu_read_unlock();

    return err;
}

s32 BPF_STRUCT_OPS_SLEEPABLE(rossched_init) {
    int err;

    bpf_printk("rossched_dsq_init: creating DSQs...\n");
    /* Initialize amount of online CPUs */
    nr_online_cpus = get_nr_online_cpus();

    err = scx_bpf_create_dsq(DSQ_IMU, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_IMU: err=%d", err);
        return err;
    }
    err = scx_bpf_create_dsq(DSQ_RADAR, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_RADAR: err=%d", err);
        return err;
    }
    err = scx_bpf_create_dsq(DSQ_CAMERA, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_CAMERA: err=%d", err);
        return err;
    }
    err = scx_bpf_create_dsq(DSQ_PLANNER, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_PLANNER: err=%d", err);
        return err;
    }
    err = scx_bpf_create_dsq(DSQ_ULTRA, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_ULTRA: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_OBJ_DETECTION, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_OBJ_DETECTION: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_DUMMY_CAMERA, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_DUMMY_CAMERA: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_LOC, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_LOC: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_MISC, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_MISC: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_CANBUS, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_CANBUS: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_EKF, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_EKF: err=%d", err);
        return err;
    }

    err = scx_bpf_create_dsq(DSQ_ROS_NODE, -1);
    if (err) {
        scx_bpf_error("failed to create DSQ_ROS_NODE: err=%d", err);
        return err;
    }

    /*
     * Create the global shared DSQ.
     */
    err = scx_bpf_create_dsq(SHARED_DSQ, -1);
    if (err) {
        scx_bpf_error("failed to create shared DSQ: %d", err);
        return err;
    }

    /* Initialize the primary scheduling domain */
    err = init_cpumask(&primary_cpumask);
    if (err)
        return err;

    return 0;
}

void BPF_STRUCT_OPS(rossched_exit, struct scx_exit_info *ei) {
    UEI_RECORD(uei, ei);
}

SCX_OPS_DEFINE(rossched_ops,
               .select_cpu = (void *)rossched_select_cpu,
               .enqueue = (void *)rossched_enqueue,
               .dispatch = (void *)rossched_dispatch,
               .running = (void *)rossched_running,
               .stopping = (void *)rossched_stopping,
               .runnable = (void *)rossched_runnable,
               .quiescent = (void *)rossched_quiescent,
               .set_cpumask = (void *)rossched_set_cpumask,
               .enable = (void *)rossched_enable,
               .init_task = (void *)rossched_init_task,
               .init = (void *)rossched_init,
               .exit = (void *)rossched_exit,
               .flags = SCX_OPS_ENQ_EXITING,
               .timeout_ms = 10000,
               .name = "rossched");
