#include "header.h"

void init_Timer();
void init_pin();
void initializeUltrasonic();


void initializeUltrasonic(){
	init_pin();
	init_Timer();
}


void init_Timer(){
	SIM->SCGC6 |=SIM_SCGC6_TPM1_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) ;
	TPM1->MOD= 0x1FFF;

	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_MSB(1) |TPM_CnSC_ELSB(1);
	TPM1->CONTROLS[0].CnV = 3;

	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_ELSA(1) |TPM_CnSC_ELSB(1) ;
	TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK | TPM_CnSC_CHIE_MASK;
	TPM1->SC |=  TPM_SC_TOF_MASK | TPM_SC_PS(7) | TPM_SC_TOIE_MASK ;
	TPM1->SC |= TPM_SC_CMOD(1);

	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_SetPriority(TPM1_IRQn, 3);
	NVIC_EnableIRQ(TPM1_IRQn);
}


void TPM1_IRQHandler() {
	static int ctr = 0;
	static unsigned int previous = 0;
	unsigned int current = 0;
	static unsigned int interval = 0;
	if (TPM1->STATUS & TPM_STATUS_CH1F_MASK) {
		current=TPM1->CONTROLS[1].CnV;
		PTB->PTOR|= MASK(RED_LED);
		current |= (ctr << 16);
		interval = current - previous;
		previous = current;
		TPM1->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;
	}
	if (TPM1->SC & TPM_SC_TOF_MASK) {
		ctr++;
		TPM1->SC |= TPM_SC_TOF_MASK;
		if (!( ctr%10)){
			int Distance = 0;
			Distance = interval;
			if (interval>4000);
			else {
				if (Distance <= 100) {
					PTB->PSOR = MASK(EXTRA_LED);
				}
				else {
					PTB->PCOR = MASK(EXTRA_LED);
				}
			}
		}
	}
}

void init_pin(){
	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[PTA12] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[PTA12] |=PORT_PCR_MUX(3);

	SIM->SCGC5 |=SIM_SCGC5_PORTA_MASK;
	PORTA->PCR[PTA13] &=~PORT_PCR_MUX_MASK;
	PORTA->PCR[PTA13] |=PORT_PCR_MUX(3);
}
