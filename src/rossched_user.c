/* SPDX-License-Identifier: GPL-2.0 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <libgen.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include <scx/common.h>
#include "rossched.skel.h"

static bool verbose;
static volatile int exit_req;
static int nr_cpus = 0;  /* Actual number of online CPUs. Dynamically obtained if needed */

/* Signal handler */
static void sigint_handler(int sig)
{
    exit_req = 1;
}

static void print_percpu_info(int map_fd_qsizes, int map_fd_dispatch)
{
    /* Debug message: Function entry */
    printf("[DEBUG] Entering print_percpu_info()\n");
    fflush(stdout);

    printf("something\n");
    fflush(stdout);
}

int main(int argc, char **argv)
{
    struct rossched_bpf *skel;
    struct bpf_link *link;
    int map_fd_qsizes, map_fd_dispatch;
    int opt;
    char action_prob = -1;

    libbpf_set_print(NULL); /* Or set debug callback if needed */
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigint_handler);

restart:
    /* Open BPF skeleton */
    printf("[DEBUG] Opening BPF skeleton...\n");
    fflush(stdout);

    skel = SCX_OPS_OPEN(rossched_ops, rossched_bpf);
    if (!skel) {
        fprintf(stderr, "Failed to open BPF skeleton\n");
        return 1;
    }

    while ((opt = getopt(argc, argv, "vp:")) != -1) {
        switch (opt) {
        case 'v':
            verbose = true;
            break;
        case 'p':
            action_prob = atoi(optarg);

            break;
            default:
            fprintf(stderr, "Usage: %s -p <action_prob> [-v]\n", basename(argv[0]));
            return 1;
        }
    }
    
    // prob must be a value between 0 and 100
    if (action_prob < 0 || action_prob > 100) {
        fprintf(stderr, "Invalid action probability (expected 0-100, got %d)\n", action_prob);
        return 1;
    }
    skel->bss->ACTION_PROBABILITY = (u8)action_prob;
    printf("[DEBUG] action_prob=%d\n", action_prob);


    /* Load BPF program */
    printf("[DEBUG] Loading BPF skeleton...\n");
    fflush(stdout);

    SCX_OPS_LOAD(skel, rossched_ops, rossched_bpf, uei);
    if (!skel) {
        fprintf(stderr, "Failed to load BPF skeleton\n");
        rossched_bpf__destroy(skel);
        return 1;
    }

    /* Attach scheduler */
    printf("[DEBUG] Attaching BPF skeleton...\n");
    fflush(stdout);

    link = SCX_OPS_ATTACH(skel, rossched_ops, rossched_bpf);
    if (!link) {
        fprintf(stderr, "Failed to attach BPF skeleton\n");
        rossched_bpf__destroy(skel);
        return 1;
    }

    /* (Additional) Get the actual number of online CPUs */
    nr_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (nr_cpus < 0)
        nr_cpus = 1; /* fallback */

    printf("[DEBUG] nr_cpus=%d\n", nr_cpus);
    fflush(stdout);

    printf("Successfully started! Monitoring CPU queues and dispatch counts...\n");
    fflush(stdout);

    /* Print information every 1 second */
    while (!exit_req && !UEI_EXITED(skel, uei)) {
        /* Debug message: Inside while loop */
        printf("[DEBUG] Main loop: exit_req=%d, UEI_EXITED=%d\n",
               exit_req, (int)UEI_EXITED(skel, uei));
        fflush(stdout);

        print_percpu_info(map_fd_qsizes, map_fd_dispatch);

        sleep(1);
    }

    printf("[DEBUG] Cleaning up and destroying BPF link...\n");
    fflush(stdout);

    bpf_link__destroy(link);
    {
        __u64 ecode = UEI_REPORT(skel, uei);
        rossched_bpf__destroy(skel);

        /* Debug message: Print ecode value */
        printf("[DEBUG] ecode=%llu\n", (unsigned long long)ecode);
        fflush(stdout);

        if (UEI_ECODE_RESTART(ecode)) {
            printf("[DEBUG] ecode indicates restart, jumping to restart label...\n");
            fflush(stdout);
            goto restart;
        }
    }
    return 0;
}