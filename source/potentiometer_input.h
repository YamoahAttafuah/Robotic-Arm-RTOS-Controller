#include "header.h"

int getBasePos();
int interp(int xp);
void initializeADC();
void printPotReadings();
double interpolate(double pot_reading);

volatile int g_base_pos = 0;
volatile int g_arm2_pos = 0;
volatile int g_grip_rotation = 0;
volatile int g_grip_pos = 0;

int x0, x1, y0, y1;
volatile int xp = 0;	//the value reading ADC
volatile int yp = 0;


void initializeADC() {
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Select analog for pin
	PORTE->PCR[BASE_POT] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[BASE_POT] |= PORT_PCR_MUX(0);
	PORTE->PCR[ARM2_POT] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[ARM2_POT] |= PORT_PCR_MUX(0);
	PORTE->PCR[GRIP_ROTATION_POT] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GRIP_ROTATION_POT] |= PORT_PCR_MUX(0);
	PORTE->PCR[GRIP_POT] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[GRIP_POT] |= PORT_PCR_MUX(0);

	//16 bit single-ended conversion, when DIFF=0
	ADC0->CFG1 |= ADC_CFG1_MODE(3);
	ADC0->SC1[0] |= ADC_SC1_AIEN_MASK ;

	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_SetPriority(ADC0_IRQn, 3);
	NVIC_EnableIRQ(ADC0_IRQn);

	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0);
}


void ADC0_IRQHandler(void) {
	if (ADC0->SC1[0] & ADC_SC1_COCO_MASK){
		enum channels{one, two, three, four};
		static enum channels next_channel = one;
		switch (next_channel){
		case one:
			g_base_pos = ADC0->R[0];
			ADC0->SC1[0] = ADC_SC1_ADCH(31);
			ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(3) | ADC_SC1_COCO_MASK;
			next_channel = two;
			break;
		case two :
			g_arm2_pos = ADC0->R[0];
			ADC0->SC1[0] = ADC_SC1_ADCH(31);
			ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(4) | ADC_SC1_COCO_MASK;
			next_channel = three;
			break;
		case three:
			g_grip_rotation = ADC0->R[0];
			ADC0->SC1[0] = ADC_SC1_ADCH(31);
			ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(7) | ADC_SC1_COCO_MASK;
			next_channel = four;
			break;
		case four:
			g_grip_pos = ADC0->R[0];
			ADC0->SC1[0] = ADC_SC1_ADCH(31);
			ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0) | ADC_SC1_COCO_MASK;
			next_channel = one;
			break;
		default:
			ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(0) | ADC_SC1_COCO_MASK;
			next_channel = one;
			break;
		}
	}
}


int interp(int xp){
	//xp is the raw potentiometer value
	x1 = 65536/100;
	y1 = 0xCCCC;
	xp = xp/100;

	float y1_float = (float)y1;
	float x1_float = (float)x1;
	float result_float = (y1_float / x1_float) * xp;

	int yp = (int)result_float;
	//	printf("Full potentiometer range: %d\n", x1);
	//	printf("Full mod range: %d\n", y1);
	//	printf("Potentiometer value: %d\n", xp);
	//	printf("Interpolated value: %d\n", yp);
	return yp;
}

int getBasePos(){
	return g_base_pos;
}

int getArm2Pos(){
	return g_arm2_pos;
}

int getGripRotation(){
	return g_grip_rotation;
}

int getGripPos(){
	return g_grip_pos;
}

void printPotReadings(){
	printf("Base Pos: %d\n", g_base_pos);
	printf("Arm 2 Pos: %d\n", g_arm2_pos);
	printf("Grip Rotation Pos: %d\n", g_grip_rotation);
	printf("Grip Pos: %d\n\n", g_grip_pos);
}
