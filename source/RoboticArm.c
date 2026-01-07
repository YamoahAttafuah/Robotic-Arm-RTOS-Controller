#include "header.h"
#include "motor_output.h"
#include "potentiometer_input.h"
#include "led_buzzer_output.h"

volatile int base_pos = 0;
volatile int arm2_pos = 0;
volatile int grip_rotation = 0;
volatile int grip_pos = 0;

void initialize();
void RBBBRtask(void *p);
void moveMotorsTask(void *p);
void motionLightsTask(void *p);
void getPosValuesTask(void *p);

int getInterpBasePos();
int getInterpArm2Pos();
int getInterpGripRotation();
int getInterpGripPos();

TaskHandle_t moveMotorsTaskHandle = NULL;
TaskHandle_t getPosValuesTaskHandle = NULL;
TaskHandle_t motionLightsTaskHandle = NULL;
TaskHandle_t RBBBRtaskHandle = NULL;


int main(){
	initialize();
	xTaskCreate(moveMotorsTask, "moveMotorsTask", 200, NULL, tskIDLE_PRIORITY, &moveMotorsTaskHandle);
	xTaskCreate(getPosValuesTask, "getPosValuesTask", 200, NULL, tskIDLE_PRIORITY, &getPosValuesTaskHandle);
	xTaskCreate(RBBBRtask, "RBBBRtask", 200, NULL, tskIDLE_PRIORITY, &RBBBRtaskHandle);
	xTaskCreate(motionLightsTask, "motionLightsTask", 200, NULL, tskIDLE_PRIORITY, &motionLightsTaskHandle);
	vTaskStartScheduler();
	while(1){}
}

void initialize() {
	initializeMotors();
	initializeADC();
	initLED();
}

void getPosValuesTask(void *p){
	while(1) {
		base_pos = getInterpBasePos();
		vTaskDelay(50);
		arm2_pos = getInterpArm2Pos();
		vTaskDelay(50);
		grip_rotation = getInterpGripRotation();
		vTaskDelay(50);
		grip_pos = getInterpGripPos();
		vTaskDelay(50);
	}
}

void moveMotorsTask(void *p) {
	while(1) {
		changeBasePos(base_pos, 20);
		vTaskDelay(50);
		changeArm2Pos(arm2_pos, 20);
		vTaskDelay(50);
		changeGripRotation(grip_rotation, 20);
		vTaskDelay(50);
		changeGripPos(grip_pos, 20);
	}
}

void motionLightsTask(void *p){
	while(1) {
		if (arm2_motion == 0) { //forward
			PTB->PSOR=MASK(FORWARD_LED);
			PTB->PCOR=MASK(BACK_LED);
			vTaskDelay(150);
		}
		else if (arm2_motion == 1) { //backward
			PTB->PCOR=MASK(FORWARD_LED);
			PTB->PSOR=MASK(BACK_LED);
			vTaskDelay(150);
		}
		else if (arm2_motion == -1){
			PTB->PCOR=MASK(FORWARD_LED);
			PTB->PCOR=MASK(BACK_LED);
			vTaskDelay(150);
		}
		else {
			PTB->PCOR=MASK(FORWARD_LED);
			PTB->PCOR=MASK(BACK_LED);
			vTaskDelay(150);
		}
	}
}


void RBBBRtask(void *p){
	while(1){
		RBBBRpattern();
		vTaskDelay(300);
	}
}

int getInterpBasePos(){	return interp(getBasePos()); }
int getInterpArm2Pos(){	return interp(getArm2Pos()); }
int getInterpGripRotation(){ return interp(getGripRotation()); }
int getInterpGripPos(){	return interp(getGripPos()); }

void setLocalControlSource(){ 
	control_source = 0; 
	vTaskDelay(50); 
	displayOnLCD("Local Mode", 0); 
} 
 
 
void setRemoteControlSource(){ 
	control_source = 1; 
	vTaskDelay(50); 
	displayOnLCD("Remote Mode", 0); 
} 
 
 
void selfTestMode() { 
	displayOnLCD("Self-test mode", 1); 
	vTaskDelay(1000); 
	
	changeBasePos(0xF5C, 20, 1); 
	vTaskDelay(1000); 
	
	changeBasePos(0xA3D, 20, 1); 
	vTaskDelay(1000); 
	
	changeArm2Pos(0xF5C, 20, 1); 
	vTaskDelay(1000); 
	
	changeArm2Pos(0xA3D, 20, 1); 
	vTaskDelay(1000); 
	
	changeGripRotation(0xF5C, 20, 1); 
	vTaskDelay(1000); 
	
	changeGripRotation(0xA3D, 20, 1); 
	vTaskDelay(1000); 
	
	changeGripPos(0xF5C, 20, 1); 
	vTaskDelay(1000); 
	
	changeGripPos(0xA3D, 20, 1); 
	vTaskDelay(1000); 
} 
 
 
void remotelyMoveMotor(void){ 
	switch (rxChar){ 
		case 'q': 
			changeBasePos(0xA3D, 20, 1); 
			break; 
		case 'w': 
			changeBasePos(0xCCC, 20, 1); 
			break; 
		case 'e': 
			changeBasePos(0xF5C, 20, 1); 
			break; 
		case 'r': 
			changeBasePos(0x117B, 20, 1); 
			break; 
		case 't': 
			changeBasePos(0x147B, 20, 1); 
			break; 
		case 'a': 
			changeArm2Pos(0xA3D, 20, 1); 
		break; 
		case 's': 
			changeArm2Pos(0xCCC, 20, 1); 
			break; 
		case 'd': 
			changeArm2Pos(0xF5C, 20, 1); 
			break; 
		case 'f': 
			changeArm2Pos(0x117B, 20, 1); 
			break; 
		case 'g': 
			changeArm2Pos(0x147B, 20, 1); 
			break; 
		case 'z': 
			changeGripRotation(0xA3D, 20, 1); 
			break; 
		case 'x': 
			changeGripRotation(0xCCC, 20, 1); 
			break; 
		case 'c': 
			changeGripRotation(0xF5C, 20, 1); 
			break; 
		case 'v': 
			changeGripRotation(0x117B, 20, 1); 
			break; 
		case 'b': 
			changeGripRotation(0x147B, 20, 1); 
			break; 
		case 'y': 
			changeGripPos(0xA3D, 20, 1); 
			break; 
		case 'u': 
			changeGripPos(0xCCC, 20, 1); 
			break; 
		case 'i': 
			changeGripPos(0xF5C, 20, 1); 
			break; 
		case 'o': 
			changeGripPos(0x117B, 20, 1); 
			break; 
		case 'p': 
			changeGripPos(0x147B, 20, 1); 
			break; 
		case 'l': 
			PTB->PSOR=MASK(BACK_LED); 
			setLocalControlSource(); 
			break; 
		case 'k': 
			PTB->PSOR=MASK(FORWARD_LED); 
			setRemoteControlSource(); 
			break; 
		case 'h': 
			selfTestMode(); 
			break; 
		default: 
			break; 
	} 
} 
 
 
void saveMotorPositions() { 
	saved_motor_pos[saved_moves_num] = getInterpBasePos(); 
	if (saved_moves_num < arr_length-1) saved_moves_num = saved_moves_num + 1; 
	
	vTaskDelay(10); 
	saved_motor_pos[saved_moves_num] = getInterpArm2Pos(); 
	if (saved_moves_num < arr_length-1) saved_moves_num = saved_moves_num + 1; 
	
	vTaskDelay(10); 
	saved_motor_pos[saved_moves_num] = getInterpGripRotation(); 
	if (saved_moves_num < arr_length-1) saved_moves_num = saved_moves_num + 1; 
	
	vTaskDelay(10); 
	saved_motor_pos[saved_moves_num] = getInterpGripPos(); 
	if (saved_moves_num < arr_length-1) saved_moves_num = saved_moves_num + 1; 
	vTaskDelay(10); 
} 
 
 
void runMotorPositions() { 
	run = 1;

	changeBasePos(saved_motor_pos[next_move], 20, 0); 
	vTaskDelay(10);
	if (next_move < saved_moves_num) next_move = next_move + 1; 
	vTaskDelay(10); 
		
	changeArm2Pos(saved_motor_pos[next_move], 20, 0); 
	vTaskDelay(10); 
	if (next_move < saved_moves_num) next_move = next_move + 1; 
	vTaskDelay(10); 
	
	changeGripRotation(saved_motor_pos[next_move], 20, 0); 
	vTaskDelay(10); 
	if (next_move < saved_moves_num) next_move = next_move + 1; 
	vTaskDelay(10); 
	
	changeGripPos(saved_motor_pos[next_move], 20, 0); 
	vTaskDelay(10); 
	if (next_move < saved_moves_num) next_move = next_move + 1; 
	vTaskDelay(400); 
}


void clearMotorPositions() { 
	saved_moves_num = 0; 
	next_move = 0; 
	run = 0; 
}