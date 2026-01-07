#include "header.h"

void initLED();
void flashR(int t);
void flashB(int t);
void flashBuzzer();
void RBBBRpattern();

void buzzTask(void *p);

void initLED(){
	SIM->SCGC5|= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5|= SIM_SCGC5_PORTD_MASK;

	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);

	PORTB->PCR[FORWARD_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[FORWARD_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[BACK_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[BACK_LED] |= PORT_PCR_MUX(1);
	PORTB->PCR[MOTION_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[MOTION_LED] |= PORT_PCR_MUX(1);

	PORTB->PCR[BUZZER] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[BUZZER] |= PORT_PCR_MUX(1);

	PTB->PDDR |= MASK(RED_LED);
	PTD->PDDR |= MASK(BLUE_LED);
	PTB->PDDR |= MASK(FORWARD_LED);
	PTB->PDDR |= MASK(BACK_LED);
	PTB->PDDR |= MASK(MOTION_LED);
	PTB->PDDR |= MASK(BUZZER);
}

void flashR(int t){
	PTB->PCOR=MASK(RED_LED); 	//on
	vTaskDelay(t);
	PTB->PSOR=MASK(RED_LED);	//off
	vTaskDelay(t);
}

void flashB(int t){
	PTD->PCOR=MASK(BLUE_LED); 	//on
	vTaskDelay(t);
	PTD->PSOR=MASK(BLUE_LED);	//off
	vTaskDelay(t);
}

void flashBuzzer(){
	PTB->PTOR=MASK(BUZZER); 	//on
}

void RBBBRpattern(){
	flashR(200);
	vTaskDelay(400);
	flashB(200);
	flashB(200);
	flashB(200);
	vTaskDelay(400);
	flashR(200);
	vTaskDelay(400);
}


void buzzTask(void *p) {
	while(1){
		flashBuzzer();
		vTaskDelay(500);
	}
}
