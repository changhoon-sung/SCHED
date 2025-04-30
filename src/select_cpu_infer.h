/*
 * select_cpu.h example
 *
 * This is a simplified example of the RL state-action space for the
 * select_cpu functions in the context of a ROS-based system. It is dependent
 * on the implementation of the application and the number of tasks and CPUs in the
 * system.
 * 
 * 	State: 
 * 		- 12 CPU states (0,1) * 12
 * 		- Current CPU (0-11) * 1
 * 		- Total: 13 states
 * 
 * 	Action: 
 * 		- CPU number (0-11)
 * 		- Total: 12 actions
*/

#pragma once

#include "rl.h"

// Content from the original select_cpu.c, specifically the `score()` function
static __inline void select_cpu_tree_score(int input[RL_CPU_STATE_LEN], int output[RL_CPU_ACTION_LEN]) {
    int var0[12];
    if (input[12] <= -4090) {
        if (input[4] <= 0) {
            if (input[6] <= 0) {
                if (input[2] <= 0) {
                    if (input[11] <= 0) {
                        if (input[12] <= -9090) {
                            var0[0] = 1504;
                            var0[1] = 0;
                            var0[2] = 8;
                            var0[3] = 0;
                            var0[4] = 49;
                            var0[5] = 114;
                            var0[6] = 41;
                            var0[7] = 8152;
                            var0[8] = 0;
                            var0[9] = 74;
                            var0[10] = 57;
                            var0[11] = 0;
                            ;
                        } else {
                            var0[0] = 1345;
                            var0[1] = 3;
                            var0[2] = 6;
                            var0[3] = 3;
                            var0[4] = 36;
                            var0[5] = 87;
                            var0[6] = 68;
                            var0[7] = 8339;
                            var0[8] = 0;
                            var0[9] = 61;
                            var0[10] = 48;
                            var0[11] = 3;
                            ;
                        }
                    } else {
                        if (input[8] <= 0) {
                            if (input[3] <= 0) {
                                if (input[0] <= 0) {
                                    var0[0] = 1008;
                                    var0[1] = 3;
                                    var0[2] = 5;
                                    var0[3] = 14;
                                    var0[4] = 57;
                                    var0[5] = 110;
                                    var0[6] = 48;
                                    var0[7] = 8572;
                                    var0[8] = 0;
                                    var0[9] = 95;
                                    var0[10] = 85;
                                    var0[11] = 4;
                                    ;
                                } else {
                                    var0[0] = 1162;
                                    var0[1] = 2;
                                    var0[2] = 8;
                                    var0[3] = 17;
                                    var0[4] = 64;
                                    var0[5] = 110;
                                    var0[6] = 71;
                                    var0[7] = 8358;
                                    var0[8] = 0;
                                    var0[9] = 83;
                                    var0[10] = 118;
                                    var0[11] = 6;
                                    ;
                                }
                            } else {
                                var0[0] = 1060;
                                var0[1] = 7;
                                var0[2] = 9;
                                var0[3] = 20;
                                var0[4] = 96;
                                var0[5] = 131;
                                var0[6] = 98;
                                var0[7] = 8266;
                                var0[8] = 2;
                                var0[9] = 140;
                                var0[10] = 166;
                                var0[11] = 4;
                                ;
                            }
                        } else {
                            var0[0] = 994;
                            var0[1] = 4;
                            var0[2] = 11;
                            var0[3] = 23;
                            var0[4] = 68;
                            var0[5] = 123;
                            var0[6] = 100;
                            var0[7] = 8263;
                            var0[8] = 0;
                            var0[9] = 183;
                            var0[10] = 230;
                            var0[11] = 0;
                            ;
                        }
                    }
                } else {
                    var0[0] = 1170;
                    var0[1] = 22;
                    var0[2] = 14;
                    var0[3] = 40;
                    var0[4] = 110;
                    var0[5] = 150;
                    var0[6] = 93;
                    var0[7] = 7990;
                    var0[8] = 4;
                    var0[9] = 216;
                    var0[10] = 175;
                    var0[11] = 14;
                    ;
                }
            } else {
                var0[0] = 1852;
                var0[1] = 6;
                var0[2] = 4;
                var0[3] = 8;
                var0[4] = 60;
                var0[5] = 75;
                var0[6] = 54;
                var0[7] = 7823;
                var0[8] = 0;
                var0[9] = 48;
                var0[10] = 58;
                var0[11] = 10;
                ;
            }
        } else {
            var0[0] = 961;
            var0[1] = 8;
            var0[2] = 33;
            var0[3] = 44;
            var0[4] = 326;
            var0[5] = 439;
            var0[6] = 236;
            var0[7] = 6597;
            var0[8] = 0;
            var0[9] = 722;
            var0[10] = 609;
            var0[11] = 23;
            ;
        }
    } else {
        var0[0] = 458;
        var0[1] = 1340;
        var0[2] = 1012;
        var0[3] = 820;
        var0[4] = 632;
        var0[5] = 565;
        var0[6] = 678;
        var0[7] = 344;
        var0[8] = 1851;
        var0[9] = 551;
        var0[10] = 625;
        var0[11] = 1124;
        ;
    }
    output[0] = var0[0];
    output[1] = var0[1];
    output[2] = var0[2];
    output[3] = var0[3];
    output[4] = var0[4];
    output[5] = var0[5];
    output[6] = var0[6];
    output[7] = var0[7];
    output[8] = var0[8];
    output[9] = var0[9];
    output[10] = var0[10];
    output[11] = var0[11];
    ;
}

// Helper function to find the argmax index in the output vector
static __inline int select_cpu_argmax(int arr[RL_CPU_ACTION_LEN]) {
    int idx = 0;
    int best = arr[0];
#pragma clang loop unroll(disable)
    for (int i = 1; i < RL_CPU_ACTION_LEN; i++) {
        if (arr[i] > best) {
            best = arr[i];
            idx = i;
        }
    }
    return idx;
}

// Final inference function
static __inline int run_select_cpu_inference(int input[RL_CPU_STATE_LEN]) {
    int output[RL_CPU_ACTION_LEN];
    __builtin_memset(output, 0, sizeof(output));
    select_cpu_tree_score(input, output);
    int best_idx = select_cpu_argmax(output);
    return best_idx;
}
