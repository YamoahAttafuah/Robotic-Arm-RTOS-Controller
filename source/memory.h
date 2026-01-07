#include "header.h"

void initializeMemory();
void detectButtons();

volatile int g_save = 0;
volatile int g_run = 0;
volatile int g_reset = 0;

void initializeMemory(){
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;

	PORTA->PCR[SAVE_BUTTON] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[SAVE_BUTTON] |= PORT_PCR_MUX(1);
	PORTA->PCR[SAVE_BUTTON] |=  PORT_PCR_PE_MASK;

	PORTA->PCR[RUN_BUTTON] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RUN_BUTTON] |= PORT_PCR_MUX(1);
	PORTA->PCR[RUN_BUTTON] |=  PORT_PCR_PE_MASK;

	PORTA->PCR[RESET_BUTTON] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RESET_BUTTON] |= PORT_PCR_MUX(1);
	PORTA->PCR[RESET_BUTTON] |=  PORT_PCR_PE_MASK;

	PTA->PDDR &= ~MASK(SAVE_BUTTON);
	PTA->PDDR &= ~MASK(RUN_BUTTON);
	PTA->PDDR &= ~MASK(RESET_BUTTON);
}


void detectButtons() {
	if ((PTA->PDIR & MASK(SAVE_BUTTON)) == 0) {
		PTB->PSOR=MASK(FORWARD_LED);
		vTaskDelay(10);
		g_save = 1;
	}
	if ((PTA->PDIR & MASK(RUN_BUTTON)) == 0) {
		PTB->PSOR=MASK(BACK_LED);
		vTaskDelay(10);
		g_run = 1;
	}
	if ((PTA->PDIR & MASK(RESET_BUTTON)) == 0) {
		PTB->PSOR=MASK(FORWARD_LED);
		vTaskDelay(10);
		g_reset = 1;
	}
}


int getSave(){
	return g_save;
}


int getRun(){
	return g_run;
}


int getReset(){
	return g_reset;
}
