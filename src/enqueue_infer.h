/*
 * enqueue_infer.h example
 *
 * This is a simplified example of the RL state-action space for the
 * enqueue functions in the context of a ROS-based system. It is dependent on 
 * the implementation of the application and the number of tasks and CPUs in the system.
 * 
 * 	State: 
 * 		- task_type (0-11)
 * 		- e2e_status (0-3)
 * 		- delay_level (4)
 * 		- Total: 6 states
 * 
 * 	Action: 
 * 		- 0: kernel task direct dispatch
 * 		- 1: insert to custom dsq
 * 		- Total: 2 actions
*/

#pragma once

#include <scx/common.bpf.h>

#include "intf.h"
#include "rl.h"

// Part of the original enqueue_fixed.c content
static __inline void enqueue_tree_score(int input[RL_ENQUEUE_STATE_LEN], int output[RL_ENQUEUE_ACTION_LEN]) {
    int var0[2];
    if (input[4] <= -5000) {
        if (input[0] <= 9090) {
            if (input[1] <= -6666) {
                if (input[0] <= 5454) {
                    if (input[2] <= 476) {
                        if (input[0] <= -1818) {
                            if (input[2] <= 416) {
                                if (input[2] <= 414) {
                                    var0[0] = 116;
                                    var0[1] = 9884;
                                    ;
                                } else {
                                    var0[0] = 1667;
                                    var0[1] = 8333;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        } else {
                            if (input[2] <= 450) {
                                if (input[2] <= 360) {
                                    var0[0] = 433;
                                    var0[1] = 9567;
                                    ;
                                } else {
                                    var0[0] = 236;
                                    var0[1] = 9764;
                                    ;
                                }
                            } else {
                                if (input[2] <= 454) {
                                    var0[0] = 3333;
                                    var0[1] = 6667;
                                    ;
                                } else {
                                    var0[0] = 353;
                                    var0[1] = 9647;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[0] <= 1818) {
                            if (input[0] <= 0) {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            } else {
                                if (input[2] <= 479) {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    ;
                                } else {
                                    var0[0] = 1429;
                                    var0[1] = 8571;
                                    ;
                                }
                            }
                        } else {
                            if (input[2] <= 479) {
                                if (input[0] <= 3636) {
                                    var0[0] = 1818;
                                    var0[1] = 8182;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 5000;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        }
                    }
                } else {
                    if (input[2] <= 395) {
                        if (input[2] <= 375) {
                            if (input[2] <= 371) {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            } else {
                                var0[0] = 10000;
                                var0[1] = 0;
                                ;
                            }
                        } else {
                            var0[0] = 0;
                            var0[1] = 10000;
                            ;
                        }
                    } else {
                        if (input[2] <= 445) {
                            if (input[2] <= 401) {
                                var0[0] = 5000;
                                var0[1] = 5000;
                                ;
                            } else {
                                if (input[2] <= 406) {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    ;
                                } else {
                                    var0[0] = 2941;
                                    var0[1] = 7059;
                                    ;
                                }
                            }
                        } else {
                            var0[0] = 0;
                            var0[1] = 10000;
                            ;
                        }
                    }
                }
            } else {
                if (input[0] <= 3636) {
                    if (input[1] <= 0) {
                        if (input[2] <= 375) {
                            var0[0] = 0;
                            var0[1] = 10000;
                            ;
                        } else {
                            if (input[2] <= 383) {
                                if (input[2] <= 378) {
                                    var0[0] = 104;
                                    var0[1] = 9896;
                                    ;
                                } else {
                                    var0[0] = 367;
                                    var0[1] = 9633;
                                    ;
                                }
                            } else {
                                if (input[2] <= 412) {
                                    var0[0] = 27;
                                    var0[1] = 9973;
                                    ;
                                } else {
                                    var0[0] = 82;
                                    var0[1] = 9918;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[0] <= 0) {
                            if (input[1] <= 6666) {
                                if (input[0] <= -1818) {
                                    var0[0] = 8;
                                    var0[1] = 9992;
                                    ;
                                } else {
                                    var0[0] = 42;
                                    var0[1] = 9958;
                                    ;
                                }
                            } else {
                                if (input[0] <= -1818) {
                                    var0[0] = 34;
                                    var0[1] = 9966;
                                    ;
                                } else {
                                    var0[0] = 8;
                                    var0[1] = 9992;
                                    ;
                                }
                            }
                        } else {
                            if (input[0] <= 1818) {
                                if (input[2] <= 2837) {
                                    var0[0] = 32;
                                    var0[1] = 9968;
                                    ;
                                } else {
                                    var0[0] = 27;
                                    var0[1] = 9973;
                                    ;
                                }
                            } else {
                                if (input[1] <= 6666) {
                                    var0[0] = 27;
                                    var0[1] = 9973;
                                    ;
                                } else {
                                    var0[0] = 33;
                                    var0[1] = 9967;
                                    ;
                                }
                            }
                        }
                    }
                } else {
                    if (input[2] <= 438) {
                        if (input[2] <= 436) {
                            if (input[2] <= 406) {
                                if (input[2] <= 404) {
                                    var0[0] = 74;
                                    var0[1] = 9926;
                                    ;
                                } else {
                                    var0[0] = 1667;
                                    var0[1] = 8333;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        } else {
                            var0[0] = 1818;
                            var0[1] = 8182;
                            ;
                        }
                    } else {
                        if (input[0] <= 5454) {
                            if (input[1] <= 3333) {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            } else {
                                var0[0] = 33;
                                var0[1] = 9967;
                                ;
                            }
                        } else {
                            if (input[1] <= 3333) {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            } else {
                                var0[0] = 46;
                                var0[1] = 9954;
                                ;
                            }
                        }
                    }
                }
            }
        } else {
            if (input[1] <= -6666) {
                if (input[2] <= 298) {
                    var0[0] = 0;
                    var0[1] = 10000;
                    ;
                } else {
                    if (input[2] <= 404) {
                        if (input[2] <= 378) {
                            if (input[2] <= 320) {
                                if (input[2] <= 306) {
                                    var0[0] = 3571;
                                    var0[1] = 6429;
                                    ;
                                } else {
                                    var0[0] = 7692;
                                    var0[1] = 2308;
                                    ;
                                }
                            } else {
                                if (input[2] <= 375) {
                                    var0[0] = 3222;
                                    var0[1] = 6778;
                                    ;
                                } else {
                                    var0[0] = 2258;
                                    var0[1] = 7742;
                                    ;
                                }
                            }
                        } else {
                            if (input[2] <= 383) {
                                if (input[2] <= 380) {
                                    var0[0] = 4545;
                                    var0[1] = 5455;
                                    ;
                                } else {
                                    var0[0] = 8889;
                                    var0[1] = 1111;
                                    ;
                                }
                            } else {
                                if (input[2] <= 389) {
                                    var0[0] = 3095;
                                    var0[1] = 6905;
                                    ;
                                } else {
                                    var0[0] = 4709;
                                    var0[1] = 5291;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[2] <= 471) {
                            if (input[2] <= 463) {
                                if (input[2] <= 440) {
                                    var0[0] = 3090;
                                    var0[1] = 6910;
                                    ;
                                } else {
                                    var0[0] = 3803;
                                    var0[1] = 6197;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        } else {
                            if (input[2] <= 474) {
                                var0[0] = 6667;
                                var0[1] = 3333;
                                ;
                            } else {
                                if (input[2] <= 476) {
                                    var0[0] = 4000;
                                    var0[1] = 6000;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 5000;
                                    ;
                                }
                            }
                        }
                    }
                }
            } else {
                if (input[1] <= 0) {
                    if (input[2] <= 458) {
                        if (input[2] <= 442) {
                            if (input[2] <= 433) {
                                if (input[2] <= 338) {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    ;
                                } else {
                                    var0[0] = 1000;
                                    var0[1] = 9000;
                                    ;
                                }
                            } else {
                                if (input[2] <= 436) {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    ;
                                } else {
                                    var0[0] = 625;
                                    var0[1] = 9375;
                                    ;
                                }
                            }
                        } else {
                            if (input[2] <= 447) {
                                if (input[2] <= 444) {
                                    var0[0] = 2500;
                                    var0[1] = 7500;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 5000;
                                    ;
                                }
                            } else {
                                if (input[2] <= 453) {
                                    var0[0] = 2000;
                                    var0[1] = 8000;
                                    ;
                                } else {
                                    var0[0] = 1667;
                                    var0[1] = 8333;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[2] <= 492) {
                            if (input[2] <= 473) {
                                if (input[2] <= 467) {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    ;
                                } else {
                                    var0[0] = 3333;
                                    var0[1] = 6667;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        } else {
                            if (input[2] <= 502) {
                                var0[0] = 2222;
                                var0[1] = 7778;
                                ;
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                ;
                            }
                        }
                    }
                } else {
                    if (input[2] <= 2837) {
                        var0[0] = 173;
                        var0[1] = 9827;
                        ;
                    } else {
                        var0[0] = 77;
                        var0[1] = 9923;
                        ;
                    }
                }
            }
        }
    } else {
        var0[0] = 5043;
        var0[1] = 4957;
        ;
    }
    output[0] = var0[0];
    output[1] = var0[1];
    ;
}

// Helper to find argmax in a 2D integer vector
static __inline int enqueue_argmax(int arr[RL_ENQUEUE_ACTION_LEN]) {
    if (arr[RL_ENQUEUE_ACTION_DIRECT_DISPATCH] > arr[RL_ENQUEUE_ACTION_INSERT_DSQ]) {
        return RL_ENQUEUE_ACTION_DIRECT_DISPATCH;
    } else {
        return RL_ENQUEUE_ACTION_INSERT_DSQ;
    }
}

// Final inference function
static __inline int run_enqueue_inference(int input[RL_ENQUEUE_STATE_LEN]) {
    int output[RL_ENQUEUE_ACTION_LEN];
    __builtin_memset(output, 0, sizeof(output));
    enqueue_tree_score(input, output);
    int best_idx = enqueue_argmax(output);
    bpf_printk("enqueue: inference: output[0], output[1]: %d, %d\n", output[0], output[1]);
    return best_idx;
}
