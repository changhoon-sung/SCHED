# HOWTO

## Tested Environment
- ARM64 Architecture
- Ubuntu 24.04
- Linux 6.12.0
- scx v1.0.8

## 1. Environment Setup
> For detailed instructions on configuring the eBPF scheduler build environment, refer to the [scx](https://github.com/sched-ext/scx/tree/v1.0.8).  
Build and install Linux version 6.12 on the host machine with the following `CONFIG` options enabled:

- `CONFIG_BPF=y`
- `CONFIG_BPF_EVENTS=y`
- `CONFIG_BPF_JIT=y`
- `CONFIG_BPF_SYSCALL=y`
- `CONFIG_DEBUG_INFO_BTF=y`
- `CONFIG_FTRACE=y`
- `CONFIG_SCHED_CLASS_EXT=y`

## 2. Creating a Container

> The eBPF-based scheduler is under active development and is highly sensitive to dependency versions.  
> Therefore, it is recommended to build in an isolated container environment rather than directly on the host system.

```bash
docker run \
	-it \
	--init \
	--privileged \
	--security-opt seccomp=unconfined \
	-v /sys/kernel/tracing:/sys/kernel/tracing\
	--name SCHED \
	ubuntu:24.04 \
	/bin/bash
```

## 3. Clone the Repository and Build
```bash
apt update && apt install -y git
git clone https://github.com/changhoon-sung/SCHED.git
cd SCHED
./build.sh
```

## 4. Load the eBPF Scheduler
```bash
# The 'p' parameter is an example argument that sets the probability (in percent) of using the new scheduler.
# For example, p=50 means the new scheduler is used 50% of the time.
./build/rossched -p 0
```