#ifndef HEADER_H_
#define HEADER_H_

#include <mkl25z4.h>
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"

#define MASK(X)  (1<<X)
#define BASE_MOTOR			(0) 	//PTD0
#define ARM2_MOTOR			(2)		//PTD2
#define GRIP_ROTATION_MOTOR	(3)		//PTD3
#define GRIP_MOTOR			(4)		//PTD4

#define BASE_POT			(20)	//PTE20   (ALT4)
#define ARM2_POT			(22) 	//PTE22
#define GRIP_ROTATION_POT	(21) 	//PTE21
#define GRIP_POT			(23) 	//PTE23

#define RED_LED			(18)  		//PTB18
#define BLUE_LED  		(1) 		//PTD1
#define FORWARD_LED		(0)  		//PTB0
#define BACK_LED		(1)			//PTB1
#define MOTION_LED		(2)			//PTB2
#define BUZZER			(3)			//PTB3

int getInterpBasePos();
int getInterpArm2Pos();
int getInterpGripRotation();
int getInterpGripPos();


// Global variables accessible by all files
int pos;
int arm2_motion = -1; // 0 for forward, 1 for backwards, -1 for static so off
int base_motion = -1; // 0 for cw, 1 for ccw, -1 for static so off
volatile int control_source = 0; // 0 for local potentiometer control, 1 for remote controller, -1 for motion disabled

#endif
