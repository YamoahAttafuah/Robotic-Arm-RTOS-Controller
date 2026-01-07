#include "header.h"

void initLCDPins(void);
void write_D0_D7(unsigned char instruction);
void lcd_write_instruc (unsigned char instruction);
void lcd_write_char (unsigned char c);
void lcd_init (void);
void lcd_clear(void);
void lcd_goto(unsigned char column, unsigned char row);
void lcd_write_string(char *s);
void lcd_scroll_text(char name1[],int row, int column);


void initializeLCD() {
	initLCDPins();
	lcd_init();
	lcd_clear();
	lcd_goto(0,0);
	//lcd_write_string("YT!");
}


void displayOnLCD(char name3[], unsigned char row) {
	lcd_clear();
	lcd_goto(0,row);
	lcd_write_string(name3);
}


void initLCDPins(void){
	SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

	PORTC->PCR[D0] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D0] |= PORT_PCR_MUX(1);
	PORTC->PCR[D1] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D1] |= PORT_PCR_MUX(1);
	PORTC->PCR[D2] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D2] |= PORT_PCR_MUX(1);
	PORTC->PCR[D3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D3] |= PORT_PCR_MUX(1);
	PORTC->PCR[D4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D4] |= PORT_PCR_MUX(1);
	PORTC->PCR[D5] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D5] |= PORT_PCR_MUX(1);
	PORTC->PCR[D6] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D6] |= PORT_PCR_MUX(1);
	PORTC->PCR[D7] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[D7] |= PORT_PCR_MUX(1);
	PORTC->PCR[RS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[RS] |= PORT_PCR_MUX(1);
	PORTC->PCR[E] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[E] |= PORT_PCR_MUX(1);

	PTC->PDDR |= MASK(D0) | MASK(D1) | MASK(D2) | MASK(D3) | MASK(D4) | MASK(D5) | MASK(D6) | MASK(D7);
	PTC->PDDR |= MASK(RS) | MASK(E);
}


void delay_ms(int t_ms){
	for (int i=0; i<t_ms; i++)
		for (int j=0; j<48000;j++);
}


 void write_D0_D7(unsigned char instruction){
	(instruction & MASK(0)) ? (PTC->PSOR = MASK(D0)) : (PTC->PCOR = MASK(D0));
	(instruction & MASK(1)) ? (PTC->PSOR = MASK(D1)) : (PTC->PCOR = MASK(D1));
	(instruction & MASK(2)) ? (PTC->PSOR = MASK(D2)) : (PTC->PCOR = MASK(D2));
	(instruction & MASK(3)) ? (PTC->PSOR = MASK(D3)) : (PTC->PCOR = MASK(D3));
	(instruction & MASK(4)) ? (PTC->PSOR = MASK(D4)) : (PTC->PCOR = MASK(D4));
	(instruction & MASK(5)) ? (PTC->PSOR = MASK(D5)) : (PTC->PCOR = MASK(D5));
	(instruction & MASK(6)) ? (PTC->PSOR = MASK(D6)) : (PTC->PCOR = MASK(D6));
	(instruction & MASK(7)) ? (PTC->PSOR = MASK(D7)) : (PTC->PCOR = MASK(D7));
}

void lcd_write_instruc (unsigned char instruction){
	delay_ms(2);
	PTC->PCOR = MASK(RS);
	PTC->PCOR= MASK(E);
	write_D0_D7(instruction);
	PTC->PSOR= MASK(E);
	delay_ms(2);
	PTC->PCOR= MASK(E);
}

void lcd_write_char (unsigned char c){
	delay_ms(2);
	PTC->PSOR = MASK(RS);
	PTC->PCOR= MASK(E);
	write_D0_D7(c);
	PTC->PSOR= MASK(E);
	delay_ms(2);
	PTC->PCOR= MASK(E);
}


void lcd_init (void) {
	delay_ms(2);
	lcd_write_instruc(0x06);
	lcd_write_instruc(0x0C);
	lcd_write_instruc(0x38);
}


void lcd_clear(void) {
	delay_ms(2);
	lcd_write_instruc(0x01);
	lcd_write_instruc(0x02);
}


void lcd_goto(unsigned char column, unsigned char row) {
	delay_ms(2);
	if(row==0) lcd_write_instruc(0x80 + column);
	if(row==1) lcd_write_instruc(0xC0+ column);
}


void lcd_write_string(char *s) {
	delay_ms(2);
	while(*s != 0) {
		lcd_write_char(*s);
		s++;
	}
}
