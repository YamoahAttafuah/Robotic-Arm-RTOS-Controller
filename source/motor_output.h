#include "header.h"

int getQuantizedValue(int val);

void initPWM();
void initMotorPins();
void TPM0_IRQHandler();
void initializeMotors();
void changeBasePos(int pos, int t);
void changeArm2Pos(int pos, int t);
void changeGripRotation(int pos, int t);
void changeGripPos(int pos, int t);
void setArm2MotionLight(int new_pos);
void setBaseMotionBuzzer(int new_pos);



void initializeMotors() {
	initMotorPins();
	initPWM();
}

void initMotorPins(){
	SIM->SCGC5 |=SIM_SCGC5_PORTD_MASK;
	PORTD->PCR[BASE_MOTOR] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BASE_MOTOR] |= PORT_PCR_MUX(4);
	PORTD->PCR[ARM2_MOTOR] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[ARM2_MOTOR] |= PORT_PCR_MUX(4);
	PORTD->PCR[GRIP_ROTATION_MOTOR] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GRIP_ROTATION_MOTOR] |= PORT_PCR_MUX(4);
	PORTD->PCR[GRIP_MOTOR] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[GRIP_MOTOR] |= PORT_PCR_MUX(4);
}

void initPWM(){
	SIM->SCGC6 |=SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	TPM0->MOD = 0xCCCC;  //20ms

	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;
	TPM0->CONTROLS[3].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[3].CnSC |= TPM_CnSC_CHF_MASK;
	TPM0->CONTROLS[4].CnSC |= TPM_CnSC_MSB(1) | TPM_CnSC_ELSB(1) ;
	TPM0->CONTROLS[4].CnSC |= TPM_CnSC_CHF_MASK;

	TPM0->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(3) | TPM_SC_TOIE_MASK  ;
	TPM0->SC |= TPM_SC_CMOD(1); //enable internal clock to run

	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_SetPriority(TPM0_IRQn, 3);
	NVIC_EnableIRQ(TPM0_IRQn);
}


void TPM0_IRQHandler(){
	if (TPM0->SC & TPM_SC_TOF_MASK){	//count overflow
			if (TPM0->CONTROLS[0].CnSC &  TPM_CnSC_CHF_MASK) {
				TPM0->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
			}
			if (TPM0->CONTROLS[2].CnSC &  TPM_CnSC_CHF_MASK) {
				TPM0->CONTROLS[2].CnSC |= TPM_CnSC_CHF_MASK;
			}
			if (TPM0->CONTROLS[3].CnSC &  TPM_CnSC_CHF_MASK) {
				TPM0->CONTROLS[3].CnSC |= TPM_CnSC_CHF_MASK;
			}
			if (TPM0->CONTROLS[4].CnSC &  TPM_CnSC_CHF_MASK) {
				TPM0->CONTROLS[4].CnSC |= TPM_CnSC_CHF_MASK;
			}
		}
	TPM0->SC |= TPM_SC_TOF_MASK ; //clear interrupt
}


void changeBasePos(int pos, int t) {
	int val = getQuantizedValue(pos);
	vTaskDelay(t);
	setBaseMotionBuzzer(val);
	vTaskDelay(t);
	TPM0->CONTROLS[0].CnV = val;
	vTaskDelay(t);
}

void changeArm2Pos(int pos, int t) {
	int val = getQuantizedValue(pos);
	vTaskDelay(t);
	setArm2MotionLight(val);
	vTaskDelay(t);
	TPM0->CONTROLS[2].CnV = val;
	vTaskDelay(t);
}

void changeGripRotation(int pos, int t) {
	int val = getQuantizedValue(pos);
	vTaskDelay(t);
	TPM0->CONTROLS[3].CnV = val;
	vTaskDelay(t);
}

void changeGripPos(int pos, int t) {
	int val = getQuantizedValue(pos);
	vTaskDelay(t);
	TPM0->CONTROLS[4].CnV = val;
	vTaskDelay(t);
}

void setArm2MotionLight(int new_pos){
	int prev_pos = TPM0->CONTROLS[2].CnV;
	if (new_pos == 0) {
		arm2_motion = -1;
	}
	if (new_pos > prev_pos) {
		arm2_motion = 0; // forward
	}
	else if (new_pos <= prev_pos) {
		arm2_motion = 1; // backward
	}
}

void setBaseMotionBuzzer(int new_pos){
	int prev_pos = TPM0->CONTROLS[0].CnV;
	if (new_pos == 0) {
		base_motion = -1;
	}
	if (new_pos > prev_pos) {
		base_motion = 0; // cw
	}
	else if (new_pos <= prev_pos) {
		base_motion = 1; // ccw
	}
}

int getQuantizedValue(int val) {
	if (val >= 0 && val < 10485) { //104
		return 0xA3D; //0 deg
	}
	else if (val >= 10485 && val < 20970) { //209
		return 0xCCC; //45 deg
	}
	else if (val >= 20970 && val < 31455) { //314
		return 0xF5C; //90 deg
	}
	else if (val >= 31455 && val < 41940) { //419
		return 0x117B; //135 deg
	}
	else {
		return 0x147B; //180 deg
	}
}
