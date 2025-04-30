#pragma once

/**
 * Example RL State-Action Space and Constants
 * It depends on the implementation of the application and the number of tasks 
 * and CPUs in the system.
 *
 * select_cpu:
 * 	State: 
 * 		- 12 CPU states (0,1) * 12
 * 		- Current CPU (0-11) * 1
 * 		- Total: 13 states
 * 
 * 	Action: 
 * 		- CPU number (0-11)
 * 		- Total: 12 actions
 * 
 * enqueue:
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
 * 
 * dispatch:
 * 	State: 
 * 		- Queue length for each app (12)
 * 		- Total: 12 states
 * 
 * 	Action: 
 * 		- Consume each app queue or current task expired
 * 		- Total: 13 actions
 */


// common
#define CPU_COUNT 12
#define INT_SCALE 10000

#define E2E_LATENCY_KEY 128

u8 ACTION_PROBABILITY = 0;
u8 SUBMIT_TO_BUFFER = 1;

#define TASK_COMM_LEN   16

#define DSQ_WIDTH 5
#define DSQ_MASK ((1U << DSQ_WIDTH) - 1)

#define MIN_DELAY		0
#define MAX_DELAY		10000	// 10ms
#define DELAY_BITS 	    14   
#define E2E_STAGE_BITS       4

// E2E Status
#define E2E_STAGE_LOC        0
#define E2E_STAGE_CANBUS     1
#define E2E_STAGE_EKF        2
#define E2E_STAGE_PLANNER    3

// select_cpu
#define RL_CPU_STATE_LEN (CPU_COUNT + 1)
#define RL_CPU_ACTION_LEN CPU_COUNT
#define RL_CPU_STATE_WIDTH 4
#define RL_CPU_STATE_MASK ((1U << RL_CPU_STATE_WIDTH) - 1)

// enqueue
// By definition, ENQUEUE_STATE_LEN is 6, but actually only task_type, 
// e2e_status, and current_e2e_delay_level are sent, and the rest are padded 
// during the training phase.
#define RL_ENQUEUE_PADDING 	 	0
#define RL_ENQUEUE_STATE_LEN 				6
#define RL_ENQUEUE_ACTION_LEN 				2
#define RL_ENQUEUE_ACTION_DIRECT_DISPATCH 	0
#define RL_ENQUEUE_ACTION_INSERT_DSQ 		1

// dispatch
#define RL_DISPATCH_STATE_LEN 12
#define RL_DISPATCH_ACTION_LEN 13


// DSQ ID
#define MIN_DSQ_ID 		10
#define DSQ_IMU         10
#define DSQ_RADAR       11
#define DSQ_CAMERA      12
#define DSQ_CANBUS      13
#define DSQ_PLANNER     14
#define DSQ_ULTRA       15
#define DSQ_MISC        16
#define DSQ_DUMMY_CAMERA 17
#define DSQ_LOC 		18
#define DSQ_OBJ_DETECTION 19 
#define DSQ_ROS_NODE 	20
#define DSQ_EKF 		21
#define DSQ_MAX_ID 		21

// DSQ ORDER
#define DSQ_ORDER { DSQ_IMU, DSQ_RADAR, DSQ_CAMERA, DSQ_PLANNER, \
	DSQ_ULTRA, DSQ_LOC, DSQ_OBJ_DETECTION, DSQ_CANBUS, \
	DSQ_EKF, DSQ_ROS_NODE, DSQ_MISC, DSQ_DUMMY_CAMERA }
#define DSQ_CONSUME_ORDER { DSQ_MISC, DSQ_OBJ_DETECTION, DSQ_PLANNER, DSQ_ULTRA, \
	DSQ_LOC, DSQ_CANBUS, DSQ_EKF, DSQ_ROS_NODE, DSQ_CAMERA, DSQ_IMU, DSQ_RADAR, DSQ_DUMMY_CAMERA }
#define DSQ_ORDER_LEN RL_DISPATCH_STATE_LEN

// Could be larger, but for now, treat anything 64 or above as the maximum value
// Observed to increase up to 41 with rosbag
#define DSQ_MAX_BUSY 64	

enum enqueue_action {
    ENQ_LOCAL_HEAD = 0,
    ENQ_LOCAL_TAIL,
    ENQ_GLOBAL,
	ENQ_IMU,
    ENQ_PLANNER_DSQ,
	ENQ_RADAR_DSQ,
	ENQ_CAMERA_DSQ,
	ENQ_CANBUS_DSQ,
	ENQ_ULTRA_DSQ,
	ENQ_LOC_DSQ,
	ENQ_OBJ_DETECTION_DSQ,
	ENQ_ROS_NODE_DSQ,
	ENQ_DUMMY_CAMERA_DSQ,
	ENQ_EKF_DSQ,
	ENQ_MISC_DSQ
};


