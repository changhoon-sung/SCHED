#!/bin/bash

cat ../example/dt/dt_select_cpu.pkl | m2cgen --language c > select_cpu.c
python3 c_to_bpf.py select_cpu.c > select_cpu_fixed.c

cat ../example/dt/dt_enqueue.pkl | m2cgen --language c > enqueue.c
python3 c_to_bpf.py enqueue.c > enqueue_fixed.c

cat ../example/dt/dt_dispatch.pkl | m2cgen --language c > dispatch.c
python3 c_to_bpf.py dispatch.c > dispatch_fixed.c
