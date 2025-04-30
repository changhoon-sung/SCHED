/*
 * dispatch_infer.h example
 *
 * This is a simplified example of the RL state-action space for the
 * dispatch functions in the context of a ROS-based system. It is dependent on 
 * the implementation of the application and the number of tasks and CPUs in the system.
 * 
 * 	State: 
 * 		- Queue length for each app (12)
 * 		- Total: 12 states
 * 
 * 	Action: 
 * 		- Consume each app queue or current task expired
 * 		- Total: 13 actions
*/

#pragma once

#include "rl.h"

static __inline void dispatch_tree_score(int input[RL_DISPATCH_STATE_LEN], int output[RL_DISPATCH_ACTION_LEN]) {
    int var0[13];
    if (input[2] <= -7968) {
        if (input[10] <= -9843) {
            if (input[11] <= -2343) {
                if (input[11] <= -2968) {
                    if (input[11] <= -3593) {
                        if (input[2] <= -9843) {
                            if (input[8] <= -9843) {
                                if (input[1] <= -9843) {
                                    var0[0] = 2950;
                                    var0[1] = 886;
                                    var0[2] = 551;
                                    var0[3] = 230;
                                    var0[4] = 169;
                                    var0[5] = 296;
                                    var0[6] = 385;
                                    var0[7] = 403;
                                    var0[8] = 741;
                                    var0[9] = 177;
                                    var0[10] = 1548;
                                    var0[11] = 1183;
                                    var0[12] = 479;
                                    ;
                                } else {
                                    var0[0] = 2749;
                                    var0[1] = 1002;
                                    var0[2] = 475;
                                    var0[3] = 287;
                                    var0[4] = 171;
                                    var0[5] = 338;
                                    var0[6] = 368;
                                    var0[7] = 347;
                                    var0[8] = 779;
                                    var0[9] = 184;
                                    var0[10] = 1589;
                                    var0[11] = 1148;
                                    var0[12] = 561;
                                    ;
                                }
                            } else {
                                if (input[5] <= -9531) {
                                    var0[0] = 2817;
                                    var0[1] = 998;
                                    var0[2] = 459;
                                    var0[3] = 224;
                                    var0[4] = 188;
                                    var0[5] = 199;
                                    var0[6] = 354;
                                    var0[7] = 434;
                                    var0[8] = 691;
                                    var0[9] = 213;
                                    var0[10] = 1646;
                                    var0[11] = 1251;
                                    var0[12] = 524;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 2000;
                                    var0[2] = 1000;
                                    var0[3] = 500;
                                    var0[4] = 500;
                                    var0[5] = 500;
                                    var0[6] = 0;
                                    var0[7] = 500;
                                    var0[8] = 500;
                                    var0[9] = 500;
                                    var0[10] = 1500;
                                    var0[11] = 1500;
                                    var0[12] = 1000;
                                    ;
                                }
                            }
                        } else {
                            if (input[7] <= -8906) {
                                if (input[4] <= -9531) {
                                    var0[0] = 3294;
                                    var0[1] = 862;
                                    var0[2] = 493;
                                    var0[3] = 215;
                                    var0[4] = 139;
                                    var0[5] = 334;
                                    var0[6] = 389;
                                    var0[7] = 361;
                                    var0[8] = 639;
                                    var0[9] = 174;
                                    var0[10] = 1543;
                                    var0[11] = 1154;
                                    var0[12] = 403;
                                    ;
                                } else {
                                    var0[0] = 4062;
                                    var0[1] = 625;
                                    var0[2] = 78;
                                    var0[3] = 156;
                                    var0[4] = 78;
                                    var0[5] = 469;
                                    var0[6] = 391;
                                    var0[7] = 234;
                                    var0[8] = 312;
                                    var0[9] = 156;
                                    var0[10] = 1797;
                                    var0[11] = 1562;
                                    var0[12] = 78;
                                    ;
                                }
                            } else {
                                if (input[5] <= -8906) {
                                    var0[0] = 2646;
                                    var0[1] = 1128;
                                    var0[2] = 778;
                                    var0[3] = 389;
                                    var0[4] = 156;
                                    var0[5] = 350;
                                    var0[6] = 350;
                                    var0[7] = 428;
                                    var0[8] = 1206;
                                    var0[9] = 195;
                                    var0[10] = 1012;
                                    var0[11] = 973;
                                    var0[12] = 389;
                                    ;
                                } else {
                                    var0[0] = 2517;
                                    var0[1] = 839;
                                    var0[2] = 699;
                                    var0[3] = 350;
                                    var0[4] = 0;
                                    var0[5] = 280;
                                    var0[6] = 280;
                                    var0[7] = 350;
                                    var0[8] = 699;
                                    var0[9] = 280;
                                    var0[10] = 2098;
                                    var0[11] = 1049;
                                    var0[12] = 559;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[0] <= -9843) {
                            if (input[1] <= -9531) {
                                if (input[11] <= -3281) {
                                    var0[0] = 3423;
                                    var0[1] = 541;
                                    var0[2] = 270;
                                    var0[3] = 270;
                                    var0[4] = 450;
                                    var0[5] = 360;
                                    var0[6] = 180;
                                    var0[7] = 360;
                                    var0[8] = 991;
                                    var0[9] = 450;
                                    var0[10] = 1081;
                                    var0[11] = 1081;
                                    var0[12] = 541;
                                    ;
                                } else {
                                    var0[0] = 4186;
                                    var0[1] = 698;
                                    var0[2] = 233;
                                    var0[3] = 116;
                                    var0[4] = 233;
                                    var0[5] = 233;
                                    var0[6] = 349;
                                    var0[7] = 349;
                                    var0[8] = 698;
                                    var0[9] = 116;
                                    var0[10] = 1628;
                                    var0[11] = 698;
                                    var0[12] = 465;
                                    ;
                                }
                            } else {
                                if (input[11] <= -3281) {
                                    var0[0] = 5000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 5000;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 10000;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            if (input[1] <= -9843) {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 10000;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 10000;
                                ;
                            }
                        }
                    }
                } else {
                    if (input[11] <= -2656) {
                        if (input[0] <= -9843) {
                            if (input[1] <= -9531) {
                                if (input[8] <= -9843) {
                                    var0[0] = 2361;
                                    var0[1] = 1111;
                                    var0[2] = 972;
                                    var0[3] = 417;
                                    var0[4] = 278;
                                    var0[5] = 139;
                                    var0[6] = 139;
                                    var0[7] = 278;
                                    var0[8] = 972;
                                    var0[9] = 0;
                                    var0[10] = 2639;
                                    var0[11] = 278;
                                    var0[12] = 417;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 5000;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[1] <= -9218) {
                                    var0[0] = 1667;
                                    var0[1] = 0;
                                    var0[2] = 5000;
                                    var0[3] = 0;
                                    var0[4] = 1667;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 1667;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 10000;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            var0[0] = 0;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 0;
                            var0[10] = 10000;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        }
                    } else {
                        if (input[7] <= -9531) {
                            if (input[6] <= -9531) {
                                if (input[8] <= -9218) {
                                    var0[0] = 2890;
                                    var0[1] = 913;
                                    var0[2] = 646;
                                    var0[3] = 152;
                                    var0[4] = 114;
                                    var0[5] = 266;
                                    var0[6] = 380;
                                    var0[7] = 494;
                                    var0[8] = 684;
                                    var0[9] = 76;
                                    var0[10] = 1749;
                                    var0[11] = 989;
                                    var0[12] = 646;
                                    ;
                                } else {
                                    var0[0] = 2766;
                                    var0[1] = 638;
                                    var0[2] = 1277;
                                    var0[3] = 426;
                                    var0[4] = 0;
                                    var0[5] = 319;
                                    var0[6] = 213;
                                    var0[7] = 319;
                                    var0[8] = 1383;
                                    var0[9] = 106;
                                    var0[10] = 1596;
                                    var0[11] = 532;
                                    var0[12] = 426;
                                    ;
                                }
                            } else {
                                if (input[6] <= -9218) {
                                    var0[0] = 4286;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 2857;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 2857;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            if (input[5] <= -9843) {
                                if (input[8] <= -8906) {
                                    var0[0] = 4167;
                                    var0[1] = 1667;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 833;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 3333;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 3889;
                                    var0[1] = 1111;
                                    var0[2] = 0;
                                    var0[3] = 556;
                                    var0[4] = 0;
                                    var0[5] = 556;
                                    var0[6] = 1111;
                                    var0[7] = 0;
                                    var0[8] = 556;
                                    var0[9] = 0;
                                    var0[10] = 1667;
                                    var0[11] = 556;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[3] <= -9843) {
                                    var0[0] = 1622;
                                    var0[1] = 811;
                                    var0[2] = 811;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 541;
                                    var0[6] = 270;
                                    var0[7] = 0;
                                    var0[8] = 541;
                                    var0[9] = 270;
                                    var0[10] = 2703;
                                    var0[11] = 2162;
                                    var0[12] = 270;
                                    ;
                                } else {
                                    var0[0] = 2826;
                                    var0[1] = 870;
                                    var0[2] = 435;
                                    var0[3] = 870;
                                    var0[4] = 0;
                                    var0[5] = 435;
                                    var0[6] = 870;
                                    var0[7] = 0;
                                    var0[8] = 435;
                                    var0[9] = 0;
                                    var0[10] = 1739;
                                    var0[11] = 870;
                                    var0[12] = 652;
                                    ;
                                }
                            }
                        }
                    }
                }
            } else {
                if (input[8] <= -9843) {
                    if (input[11] <= -1406) {
                        if (input[6] <= -9843) {
                            if (input[11] <= -1718) {
                                if (input[1] <= -9218) {
                                    var0[0] = 2604;
                                    var0[1] = 1667;
                                    var0[2] = 625;
                                    var0[3] = 0;
                                    var0[4] = 208;
                                    var0[5] = 521;
                                    var0[6] = 208;
                                    var0[7] = 521;
                                    var0[8] = 1146;
                                    var0[9] = 208;
                                    var0[10] = 833;
                                    var0[11] = 937;
                                    var0[12] = 521;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 5000;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[1] <= -9843) {
                                    var0[0] = 1724;
                                    var0[1] = 2759;
                                    var0[2] = 690;
                                    var0[3] = 345;
                                    var0[4] = 690;
                                    var0[5] = 0;
                                    var0[6] = 345;
                                    var0[7] = 1034;
                                    var0[8] = 345;
                                    var0[9] = 0;
                                    var0[10] = 1034;
                                    var0[11] = 1034;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 10000;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            if (input[1] <= -9843) {
                                if (input[11] <= -1718) {
                                    var0[0] = 2353;
                                    var0[1] = 588;
                                    var0[2] = 0;
                                    var0[3] = 1176;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 2353;
                                    var0[9] = 0;
                                    var0[10] = 588;
                                    var0[11] = 1765;
                                    var0[12] = 1176;
                                    ;
                                } else {
                                    var0[0] = 4286;
                                    var0[1] = 1429;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 1429;
                                    var0[10] = 2857;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[1] <= -9218) {
                                    var0[0] = 7143;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 1429;
                                    var0[4] = 0;
                                    var0[5] = 1429;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 10000;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[0] <= -9843) {
                            if (input[1] <= -9531) {
                                if (input[11] <= -781) {
                                    var0[0] = 4186;
                                    var0[1] = 930;
                                    var0[2] = 698;
                                    var0[3] = 465;
                                    var0[4] = 233;
                                    var0[5] = 0;
                                    var0[6] = 233;
                                    var0[7] = 465;
                                    var0[8] = 698;
                                    var0[9] = 0;
                                    var0[10] = 1163;
                                    var0[11] = 465;
                                    var0[12] = 465;
                                    ;
                                } else {
                                    var0[0] = 2963;
                                    var0[1] = 370;
                                    var0[2] = 1111;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 1111;
                                    var0[7] = 370;
                                    var0[8] = 1111;
                                    var0[9] = 0;
                                    var0[10] = 1852;
                                    var0[11] = 0;
                                    var0[12] = 1111;
                                    ;
                                }
                            } else {
                                if (input[1] <= -9218) {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 2500;
                                    var0[7] = 0;
                                    var0[8] = 5000;
                                    var0[9] = 0;
                                    var0[10] = 2500;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 5000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 5000;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            if (input[1] <= -9687) {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 5000;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 5000;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 10000;
                                ;
                            }
                        }
                    }
                } else {
                    if (input[2] <= -9843) {
                        if (input[0] <= -9687) {
                            var0[0] = 0;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 10000;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 0;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        } else {
                            if (input[11] <= -1250) {
                                var0[0] = 10000;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            } else {
                                if (input[3] <= -9843) {
                                    var0[0] = 5000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 5000;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 10000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[11] <= -468) {
                            var0[0] = 0;
                            var0[1] = 10000;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 0;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        } else {
                            var0[0] = 0;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 10000;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 0;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        }
                    }
                }
            }
        } else {
            if (input[11] <= -8906) {
                if (input[11] <= -9531) {
                    if (input[3] <= -9531) {
                        if (input[5] <= -9843) {
                            if (input[10] <= -9218) {
                                if (input[0] <= -9843) {
                                    var0[0] = 3082;
                                    var0[1] = 937;
                                    var0[2] = 539;
                                    var0[3] = 216;
                                    var0[4] = 141;
                                    var0[5] = 268;
                                    var0[6] = 367;
                                    var0[7] = 391;
                                    var0[8] = 779;
                                    var0[9] = 182;
                                    var0[10] = 1472;
                                    var0[11] = 1167;
                                    var0[12] = 460;
                                    ;
                                } else {
                                    var0[0] = 4118;
                                    var0[1] = 1471;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 588;
                                    var0[5] = 0;
                                    var0[6] = 294;
                                    var0[7] = 588;
                                    var0[8] = 882;
                                    var0[9] = 0;
                                    var0[10] = 294;
                                    var0[11] = 1176;
                                    var0[12] = 588;
                                    ;
                                }
                            } else {
                                if (input[11] <= -9843) {
                                    var0[0] = 3611;
                                    var0[1] = 1389;
                                    var0[2] = 278;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 278;
                                    var0[7] = 833;
                                    var0[8] = 833;
                                    var0[9] = 0;
                                    var0[10] = 1111;
                                    var0[11] = 833;
                                    var0[12] = 833;
                                    ;
                                } else {
                                    var0[0] = 6667;
                                    var0[1] = 833;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 833;
                                    var0[9] = 833;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 833;
                                    ;
                                }
                            }
                        } else {
                            if (input[1] <= -9843) {
                                if (input[7] <= -9843) {
                                    var0[0] = 4615;
                                    var0[1] = 923;
                                    var0[2] = 154;
                                    var0[3] = 154;
                                    var0[4] = 308;
                                    var0[5] = 154;
                                    var0[6] = 0;
                                    var0[7] = 462;
                                    var0[8] = 923;
                                    var0[9] = 0;
                                    var0[10] = 923;
                                    var0[11] = 1077;
                                    var0[12] = 308;
                                    ;
                                } else {
                                    var0[0] = 3373;
                                    var0[1] = 964;
                                    var0[2] = 241;
                                    var0[3] = 120;
                                    var0[4] = 120;
                                    var0[5] = 241;
                                    var0[6] = 602;
                                    var0[7] = 120;
                                    var0[8] = 602;
                                    var0[9] = 241;
                                    var0[10] = 1084;
                                    var0[11] = 1807;
                                    var0[12] = 482;
                                    ;
                                }
                            } else {
                                if (input[7] <= -9843) {
                                    var0[0] = 6000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 2000;
                                    var0[12] = 2000;
                                    ;
                                } else {
                                    var0[0] = 3077;
                                    var0[1] = 0;
                                    var0[2] = 3846;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 769;
                                    var0[7] = 0;
                                    var0[8] = 769;
                                    var0[9] = 0;
                                    var0[10] = 769;
                                    var0[11] = 769;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[10] <= -9531) {
                            var0[0] = 0;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 10000;
                            var0[9] = 0;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        } else {
                            var0[0] = 0;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 10000;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        }
                    }
                } else {
                    if (input[10] <= -9218) {
                        if (input[0] <= -9843) {
                            if (input[8] <= -9843) {
                                if (input[9] <= -9843) {
                                    var0[0] = 4419;
                                    var0[1] = 775;
                                    var0[2] = 233;
                                    var0[3] = 78;
                                    var0[4] = 0;
                                    var0[5] = 310;
                                    var0[6] = 233;
                                    var0[7] = 233;
                                    var0[8] = 698;
                                    var0[9] = 78;
                                    var0[10] = 2016;
                                    var0[11] = 543;
                                    var0[12] = 388;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 5000;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 5000;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[7] <= -9531) {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 5000;
                                    var0[12] = 5000;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 10000;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        } else {
                            if (input[1] <= -9843) {
                                if (input[7] <= -9843) {
                                    var0[0] = 0;
                                    var0[1] = 2000;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 6000;
                                    var0[11] = 2000;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 10000;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[11] <= -9218) {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 10000;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 5000;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 5000;
                                    ;
                                }
                            }
                        }
                    } else {
                        var0[0] = 0;
                        var0[1] = 0;
                        var0[2] = 0;
                        var0[3] = 0;
                        var0[4] = 0;
                        var0[5] = 0;
                        var0[6] = 0;
                        var0[7] = 0;
                        var0[8] = 10000;
                        var0[9] = 0;
                        var0[10] = 0;
                        var0[11] = 0;
                        var0[12] = 0;
                        ;
                    }
                }
            } else {
                if (input[11] <= -4843) {
                    if (input[3] <= -9531) {
                        if (input[7] <= -9218) {
                            if (input[1] <= -8593) {
                                if (input[6] <= -9843) {
                                    var0[0] = 2265;
                                    var0[1] = 1150;
                                    var0[2] = 627;
                                    var0[3] = 244;
                                    var0[4] = 314;
                                    var0[5] = 279;
                                    var0[6] = 697;
                                    var0[7] = 174;
                                    var0[8] = 801;
                                    var0[9] = 105;
                                    var0[10] = 1777;
                                    var0[11] = 1289;
                                    var0[12] = 279;
                                    ;
                                } else {
                                    var0[0] = 3429;
                                    var0[1] = 571;
                                    var0[2] = 1143;
                                    var0[3] = 0;
                                    var0[4] = 286;
                                    var0[5] = 571;
                                    var0[6] = 0;
                                    var0[7] = 571;
                                    var0[8] = 286;
                                    var0[9] = 286;
                                    var0[10] = 1429;
                                    var0[11] = 1429;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 10000;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        } else {
                            if (input[8] <= -9218) {
                                if (input[5] <= -8906) {
                                    var0[0] = 7143;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 1429;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 1429;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 10000;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                if (input[2] <= -9374) {
                                    var0[0] = 10000;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 0;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 3030;
                                    var0[1] = 2121;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 303;
                                    var0[6] = 606;
                                    var0[7] = 606;
                                    var0[8] = 1515;
                                    var0[9] = 0;
                                    var0[10] = 909;
                                    var0[11] = 909;
                                    var0[12] = 0;
                                    ;
                                }
                            }
                        }
                    } else {
                        if (input[0] <= -9374) {
                            if (input[10] <= -9531) {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 10000;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 10000;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        } else {
                            if (input[8] <= -8593) {
                                if (input[7] <= -9218) {
                                    var0[0] = 2222;
                                    var0[1] = 2222;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 1111;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 2222;
                                    var0[11] = 0;
                                    var0[12] = 2222;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 3750;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 2500;
                                    var0[11] = 3750;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 10000;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        }
                    }
                } else {
                    if (input[6] <= -9531) {
                        if (input[7] <= -9531) {
                            if (input[5] <= -9374) {
                                if (input[10] <= -9531) {
                                    var0[0] = 3478;
                                    var0[1] = 1217;
                                    var0[2] = 435;
                                    var0[3] = 261;
                                    var0[4] = 174;
                                    var0[5] = 348;
                                    var0[6] = 87;
                                    var0[7] = 87;
                                    var0[8] = 696;
                                    var0[9] = 87;
                                    var0[10] = 870;
                                    var0[11] = 1913;
                                    var0[12] = 348;
                                    ;
                                } else {
                                    var0[0] = 6667;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 1111;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 1111;
                                    var0[11] = 1111;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 5000;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 5000;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        } else {
                            if (input[7] <= -8906) {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 10000;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            } else {
                                var0[0] = 0;
                                var0[1] = 0;
                                var0[2] = 10000;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        }
                    } else {
                        if (input[0] <= -9531) {
                            if (input[1] <= -9843) {
                                if (input[11] <= -1718) {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 2000;
                                    var0[4] = 0;
                                    var0[5] = 2000;
                                    var0[6] = 2000;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 4000;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                } else {
                                    var0[0] = 0;
                                    var0[1] = 0;
                                    var0[2] = 0;
                                    var0[3] = 0;
                                    var0[4] = 0;
                                    var0[5] = 0;
                                    var0[6] = 0;
                                    var0[7] = 0;
                                    var0[8] = 0;
                                    var0[9] = 0;
                                    var0[10] = 10000;
                                    var0[11] = 0;
                                    var0[12] = 0;
                                    ;
                                }
                            } else {
                                var0[0] = 0;
                                var0[1] = 10000;
                                var0[2] = 0;
                                var0[3] = 0;
                                var0[4] = 0;
                                var0[5] = 0;
                                var0[6] = 0;
                                var0[7] = 0;
                                var0[8] = 0;
                                var0[9] = 0;
                                var0[10] = 0;
                                var0[11] = 0;
                                var0[12] = 0;
                                ;
                            }
                        } else {
                            var0[0] = 10000;
                            var0[1] = 0;
                            var0[2] = 0;
                            var0[3] = 0;
                            var0[4] = 0;
                            var0[5] = 0;
                            var0[6] = 0;
                            var0[7] = 0;
                            var0[8] = 0;
                            var0[9] = 0;
                            var0[10] = 0;
                            var0[11] = 0;
                            var0[12] = 0;
                            ;
                        }
                    }
                }
            }
        }
    } else {
        if (input[2] <= -6406) {
            if (input[2] <= -7031) {
                if (input[2] <= -7499) {
                    var0[0] = 0;
                    var0[1] = 0;
                    var0[2] = 0;
                    var0[3] = 0;
                    var0[4] = 0;
                    var0[5] = 0;
                    var0[6] = 5000;
                    var0[7] = 0;
                    var0[8] = 0;
                    var0[9] = 0;
                    var0[10] = 0;
                    var0[11] = 5000;
                    var0[12] = 0;
                    ;
                } else {
                    var0[0] = 0;
                    var0[1] = 10000;
                    var0[2] = 0;
                    var0[3] = 0;
                    var0[4] = 0;
                    var0[5] = 0;
                    var0[6] = 0;
                    var0[7] = 0;
                    var0[8] = 0;
                    var0[9] = 0;
                    var0[10] = 0;
                    var0[11] = 0;
                    var0[12] = 0;
                    ;
                }
            } else {
                if (input[2] <= -6718) {
                    var0[0] = 0;
                    var0[1] = 0;
                    var0[2] = 0;
                    var0[3] = 0;
                    var0[4] = 0;
                    var0[5] = 0;
                    var0[6] = 0;
                    var0[7] = 0;
                    var0[8] = 0;
                    var0[9] = 0;
                    var0[10] = 10000;
                    var0[11] = 0;
                    var0[12] = 0;
                    ;
                } else {
                    if (input[7] <= -9687) {
                        var0[0] = 0;
                        var0[1] = 0;
                        var0[2] = 0;
                        var0[3] = 0;
                        var0[4] = 5000;
                        var0[5] = 5000;
                        var0[6] = 0;
                        var0[7] = 0;
                        var0[8] = 0;
                        var0[9] = 0;
                        var0[10] = 0;
                        var0[11] = 0;
                        var0[12] = 0;
                        ;
                    } else {
                        var0[0] = 0;
                        var0[1] = 0;
                        var0[2] = 0;
                        var0[3] = 0;
                        var0[4] = 0;
                        var0[5] = 0;
                        var0[6] = 0;
                        var0[7] = 0;
                        var0[8] = 0;
                        var0[9] = 0;
                        var0[10] = 10000;
                        var0[11] = 0;
                        var0[12] = 0;
                        ;
                    }
                }
            }
        } else {
            if (input[10] <= -5000) {
                var0[0] = 0;
                var0[1] = 0;
                var0[2] = 0;
                var0[3] = 0;
                var0[4] = 0;
                var0[5] = 0;
                var0[6] = 0;
                var0[7] = 0;
                var0[8] = 0;
                var0[9] = 0;
                var0[10] = 0;
                var0[11] = 0;
                var0[12] = 10000;
                ;
            } else {
                var0[0] = 832;
                var0[1] = 781;
                var0[2] = 801;
                var0[3] = 733;
                var0[4] = 717;
                var0[5] = 719;
                var0[6] = 730;
                var0[7] = 762;
                var0[8] = 799;
                var0[9] = 707;
                var0[10] = 852;
                var0[11] = 795;
                var0[12] = 771;
                ;
            }
        }
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
    output[12] = var0[12];
    ;
}

static void selection_sort_with_idx(int arr[], int idx[], int len) {
    for (int i = 0; i < len - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < len; j++) {
            if (arr[j] > arr[min_idx]) {
                min_idx = j;
            }
        }
        // Swap the elements in arr
        int temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
        // Swap the corresponding indices in idx
        int temp_idx = idx[min_idx];
        idx[min_idx] = idx[i];
        idx[i] = temp_idx;
    }
}

static int run_dispatch_inference(int input[RL_DISPATCH_STATE_LEN], int output[RL_DISPATCH_ACTION_LEN]) {
    dispatch_tree_score(input, output);
    return 0;
}
