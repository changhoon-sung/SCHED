# SCHED (VTC'25)

This repository supports the paper *SCHED* by demonstrating how to integrate a Decision-Tree-Based Scheduler into the Linux kernel using eBPF.

The example scheduler is implemented within a ROS-based autonomous vehicle scenario. Note that the RL state-action space and model specifications will vary depending on the target application and system configuration (e.g., number of CPUs).

We referenced the [scx](https://github.com/sched-ext/scx) and its source code when implementing the eBPF-based scheduler.
For build and loading instructions, please refer to the documentation provided in the [scx repository](https://github.com/sched-ext/scx).

## Overview

> 🚧 This repository open-sources stages 4–6 of the SCHED framework. Stages 1–3 will be released in future updates.

1. Scheduling data is collected from the Linux default scheduler.
2. An RL model is trained on this dataset to learn a scheduling policy.
3. The trained RL model is distilled and quantized into a lightweight Decision Tree.
---
4. The distilled Decision Tree is converted to C code via [m2cgen](https://github.com/BayesWitnesses/m2cgen).
5. The C code is quantized and scaled using fixed-point arithmetic (`c_to_bpf.py`).
6. The resulting code is integrated into an eBPF-based Linux scheduler (see `src/*_infer.h`).


## Directory Structure

```text
├── example                  # Example DT models (.pkl) and generated code
├── include                  # eBPF headers
├── Makefile
├── requirements.txt         # Python dependencies for model conversion
├── src
│   ├── dispatch_infer.h     # DT integration for dispatch decision
│   ├── enqueue_infer.h      # DT integration for enqueue decision
│   ├── intf.h
│   ├── rl.h
│   ├── rossched.bpf.c       # eBPF kernel scheduler logic
│   ├── rossched_user.c      # User-space loader for eBPF program
│   └── select_cpu_infer.h   # DT integration for CPU selection
└── tool
    ├── common.py
    ├── convert.sh           # Converts DT (.pkl) to C code
    └── c_to_bpf.py          # Converts C code into BPF-compatible code
```

## Quick Start
See [HOWTO.md](HOWTO.md) for detailed instructions on building and running the example.

## Model Integration Points

The distilled Decision Tree model is embedded into three decision-making functions exposed by the Linux [sched_ext](https://www.kernel.org/doc/html/next/scheduler/sched-ext.html) interface:
-	`select_cpu`: Selects the CPU to which a waking task will be assigned.
-	`enqueue`: Enqueues a ready task into either the global run queue or a per-CPU local queue.
-	`dispatch`: Selects which global task to dispatch when the local queue is empty.

Integration examples are provided in `src/rossched.bpf.c`.

## Kernel Integration Considerations
- **Arithmetic Precision**:
Most ML models rely on floating-point operations, but the Linux kernel does not support floating-point computation by default. Enabling FPU operations is costly and generally unsuitable for latency-sensitive scheduling logic. Instead, we convert and scale model operations into fixed-point (integer) arithmetic. Quantization and scaling strategies are application-specific.
- **Computation Limits**:
eBPF programs are subject to a strict 1 million instruction hard limit. Additionally, the eBPF verifier performs exhaustive control-flow analysis to ensure program safety. This can easily lead to rejection if the model or logic is too complex. Model size and the structure of the state-action space must be carefully controlled to remain within verifier constraints.
- **Model Deployment and Update**:
Since the model is compiled into the eBPF program, runtime learning or dynamic updates are not supported. Model updates must be manually deployed by recompiling and reloading the kernel module, which introduces operational overhead.

## Citation
If you use this framework, please cite our SCHED paper (VTC’25):

```bibtex
@inproceedings{sched.vtc25,
  author = {Dongjoo Seo and Changhoon Sung and Ping-Xiang Chen and Bryan Donyanavard and Nikil Dutt},
  title = "{SCHED: Safe CPU Scheduling Framework with Reinforcement Learning and Decision Trees for Autonomous Vehicles}",
  booktitle = {2025 IEEE 101st Vehicular Technology Conference (VTC2025-Spring)},
  month = {June},
  year = {2025}
}
```