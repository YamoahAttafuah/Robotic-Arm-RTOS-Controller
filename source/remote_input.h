#include "header.h"

#define RX	1
#define TX	2
#define OSR 15
#define BAUD_RATE  	9600
#define CLOCK	20971520u

void initUART(void);
void initializeRemote();
void initRemotePins(void);


void initializeRemote(void) {
	initRemotePins();
	initUART();
}


void initRemotePins(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1);
	PORTA->PCR[RX] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[RX] |=  PORT_PCR_MUX(2);
	PORTA->PCR[TX] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[TX] |=  PORT_PCR_MUX(2);
}


void initUART(void){
	SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
	SIM->SCGC4 |= SIM_SCGC4_UART0_MASK;
	uint8_t sbr = (uint16_t)((CLOCK)/((OSR+1) *BAUD_RATE ));
	UART0->BDH = 0;
	UART0->BDL = sbr;
	UART0->C2  |= UART_C2_RIE_MASK | UART_C2_RE_MASK ;

	NVIC_SetPriority(UART0_IRQn, 3);
	NVIC_ClearPendingIRQ(UART0_IRQn);
	NVIC_EnableIRQ(UART0_IRQn);
}


void UART0_IRQHandler(void) {
	uint8_t ch;
	if (UART0->S1 & (UART_S1_OR_MASK |UART_S1_NF_MASK | UART_S1_FE_MASK | UART_S1_PF_MASK)) {
		UART0->S1 |= UART0_S1_OR_MASK | UART0_S1_NF_MASK | UART0_S1_FE_MASK | UART0_S1_PF_MASK;
		ch = UART0->D;
	}
	if (UART0->S1 & UART0_S1_RDRF_MASK) {
		ch = UART0->D;
		rxChar = ch;
		BaseType_t checkIfYieldRequired;
		checkIfYieldRequired = xTaskResumeFromISR(remoteControlHandle);
		portYIELD_FROM_ISR(checkIfYieldRequired);
	}
}
