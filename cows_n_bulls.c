#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd.h"

#include <stdlib.h>
#include <stdio.h>

#define _RESULTADO 1
#define _TENTATIVA 0

// #define _DEBUG

char tentativa[] = "0000" ;
char numero[5] ;
unsigned char tent_pos = 0 ;

char estado = _TENTATIVA ;

ISR(PCINT1_vect) {
	SREG &= ~(1 << 7) ;
	unsigned char input = PINC & 0x07 ;

	if (estado == _TENTATIVA) {
		switch (input) {
			case 1: dec_tentativa() ;
					break ;
			case 2: inc_tentativa() ;
					break ;
			case 4: next_pos() ;
					break ;


		}
	} else  {
		estado  = _TENTATIVA ;
		tent_pos = 0 ;
		gotoXY(tent_pos, 1) ;
	}


	_delay_ms(200);

	SREG |= (1 << 7) ;

}


void inc_tentativa() {
	tentativa[tent_pos]++ ;
	if (tentativa[tent_pos] > '9') {
		tentativa[tent_pos] = '0' ;
	}

	writeStringXY(tentativa, 0, 1) ;
	gotoXY(tent_pos,1) ;

}

void dec_tentativa() {
	tentativa[tent_pos]-- ;
	if (tentativa[tent_pos] < '0') {
		tentativa[tent_pos] = '9' ;
	}

	writeStringXY(tentativa, 0, 1) ;
	gotoXY(tent_pos,1) ;

}


void next_pos() {
	tent_pos++;
	gotoXY(tent_pos, 1) ;

	if (tent_pos >= 4) {
		tent_pos = 0 ;
		resultado() ;
	}
}

void resultado() {
	estado = _RESULTADO ;
	int touros = conta_touros() ;
	int vacas = conta_vacas_touros() - touros ;

	if (touros == 4) {
		sendInst(_LCD_CLR) ;
		writeStringXY("Parabens!!", 0, 0) ;
		writeStringXY("Voce acertou!", 0, 1) ;
		gen_num() ;
	} else {

		char txt_touros[10] ;
		sprintf(txt_touros, "%d Touros", touros) ;

		char txt_vacas[10] ;
		sprintf(txt_vacas, "%d Vacas", vacas) ;

		sendInst(_LCD_CLR) ;
		writeStringXY(txt_touros, 0, 0) ;
		writeStringXY(txt_vacas, 0, 1) ;
	}

	_delay_ms(200);

	while (!(PINC & 0x07)) ;

	_delay_ms(200) ;

	estado =_TENTATIVA ;
	tela_tentativa() ;

}

int conta_touros() {
	int i, cont = 0 ;
	for (i = 0; i < 4; i++) {
		if (tentativa[i] == numero[i]) {
			cont++ ;
		}
	}

	return cont ;
}

int conta_vacas_touros() {
	int i, j, cont = 0 ;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			if (tentativa[i] == numero[j]) {
				cont++ ;
				break ;
			}
		}
	}

	return cont ;
}


void tela_tentativa() {
	sendInst(_LCD_CLR) ;
	writeStringXY(tentativa, 0, 1) ;
	gotoXY(tent_pos, 1);


	writeStringXY("Vacas e Touros!", 0, 0) ;
	gotoXY(tent_pos, 1);

	writeStringXY("Acerte o numero!", 0, 0) ;
	gotoXY(tent_pos, 1);

}

void gen_num() {
	int n = rand() % 1000 ;
	sprintf(numero, "%0.4d", n) ;
	numero[4] = '\0' ;

}

int main() {

	SREG |= (1 << 7) ;
	PCICR = (1 << PCIE1) ;
	PCMSK1 = (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) ;

	lcd_init() ;

	DDRC &= ~((1 << DDC0) |(1 << DDC1) |(1 << DDC2)) ;


	sendInst(_LCD_ON | _LCD_CURSOR_ON | _LCD_CURSOR_BLINK) ;
	sendInst(_LCD_CLR) ;
	sendInst(_LCD_HOME) ;

	srand(0) ;

	gen_num() ;


	while (1) {
		if (estado == _TENTATIVA) {
			sendInst(_LCD_CLR) ;
			writeStringXY(tentativa, 0, 1) ;
			gotoXY(tent_pos, 1);

			#ifdef _DEBUG
				writeStringXY(numero, 0, 0) ;
			#else
				writeStringXY("Vacas e Touros!", 0, 0) ;
			#endif
			gotoXY(tent_pos, 1);
			_delay_ms(1000) ;

			#ifdef _DEBUG
				writeStringXY(numero, 0, 0) ;
			#else
				writeStringXY("Acerte o numero!", 0, 0) ;
			#endif
			gotoXY(tent_pos, 1);
			_delay_ms(10000) ;
		}

	}
}
