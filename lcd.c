#include <avr/io.h>
#include <util/delay.h>

#define _LCD_4_BIT_MODE

#define _LCD_RS_DDR DDRD
#define _LCD_RS_PORT PORTD
#define _LCD_RS PORTD5

#define _LCD_RW_DDR DDRD
#define _LCD_RW_PORT PORTD
#define _LCD_RW PORTD6

#define _LCD_EN_DDR DDRD
#define _LCD_EN_PORT PORTD
#define _LCD_EN PORTD7


#define _LCD_D0 PORTB0
#define _LCD_D0_PORT PORTB
#define _LCD_D0_DDR *(&_LCD_D0_PORT - 1)
#define _LCD_D0_PIN *(&_LCD_D0_PORT - 2)

#define _LCD_D1 PORTB1
#define _LCD_D1_PORT PORTB
#define _LCD_D1_DDR *(&_LCD_D1_PORT - 1)
#define _LCD_D1_PIN *(&_LCD_D1_PORT - 2)

#define _LCD_D2 PORTB2
#define _LCD_D2_PORT PORTB
#define _LCD_D2_DDR *(&_LCD_D2_PORT - 1)
#define _LCD_D2_PIN *(&_LCD_D2_PORT - 2)

#define _LCD_D3 PORTB3
#define _LCD_D3_PORT PORTB
#define _LCD_D3_DDR *(&_LCD_D3_PORT - 1)
#define _LCD_D3_PIN *(&_LCD_D3_PORT - 2)

#define _LCD_D4 PORTB4
#define _LCD_D4_PORT PORTB
#define _LCD_D4_DDR *(&_LCD_D4_PORT - 1)
#define _LCD_D4_PIN *(&_LCD_D4_PORT - 2)

#define _LCD_D5 PORTB5
#define _LCD_D5_PORT PORTB
#define _LCD_D5_DDR *(&_LCD_D5_PORT - 1)
#define _LCD_D5_PIN *(&_LCD_D5_PORT - 2)

#define _LCD_D6 PORTB6
#define _LCD_D6_PORT PORTB
#define _LCD_D6_DDR *(&_LCD_D6_PORT - 1)
#define _LCD_D6_PIN *(&_LCD_D6_PORT - 2)

#define _LCD_D7 PORTB7
#define _LCD_D7_PORT PORTB
#define _LCD_D7_DDR *(&_LCD_D7_PORT - 1)
#define _LCD_D7_PIN *(&_LCD_D7_PORT - 2)



#define _LCD_CLR 0x01
#define _LCD_HOME 0x02
#define _LCD_CURSOR_RIGHT 0x05
#define _LCD_CURSOR_LEFT 0x04
#define _LCD_SHIFT_LEFT 0x06 
#define _LCD_SHIFT_RIGHT 0x07
#define _LCD_OFF 0x08
#define _LCD_ON 0x0C
#define _LCD_CURSOR_ON 0x0A
#define _LCD_CURSOR_BLINK 0x09
#define _LCD_CURSOR_OFF 0x0C



void waitBusy() {

	DDRB = 0 ;
	PORTD &= ~(1 << PORTD7) ;

	PORTD &= ~(1 << PORTD5) ;
	PORTD |= (1 << PORTD6) ;
	PORTD |= (1 << PORTD7) ;

	while (PINB & (1 << PINB7)) ;

	PORTD &= ~(1 << PORTD7) ;
	PORTD &= ~(1 << PORTD6) ;
	DDRB = 0xff ;
}

void sendChar( unsigned char i) {
	PORTD &= ~(1 << PORTD7) ;
	PORTD |= (1 << PORTD5) ;

	PORTD |= (1 << PORTD7) ;

	PORTB = i ;

	PORTD &= ~(1 << PORTD7) ;
	waitBusy() ;
}

void sendInst8( unsigned char i) {
	PORTD &= ~(1 << PORTD7) ;


	PORTD |= (1 << PORTD7) ;

	PORTB = i ;

	PORTD &= ~(1 << PORTD7) ;
	waitBusy();
}

void sendInst4( unsigned char i) {
	PORTD &= ~(1 << PORTD7 ) ;

	PORTD |= (1 << PORTD7) ;

	PORTB = (i & 0xf0) | (PORTB & 0x0f) ;

	PORTD &= ~(1 <<PORTD7) ;
	
	PORTD &= ~(1 << PORTD7 ) ;

	PORTD |= (1 << PORTD7) ;

	PORTB = ((i & 0x0f) << 4) | (PORTB & 0x0f) ;

	PORTD &= ~(1 <<PORTD7) ;
	waitBusy() ;

}

void sendInst(unsigned char byte) {
	#ifdef 	_LCD_4_BIT_MODE
		sendInst4(byte) ;
	#else
		sendInst8(byte) ;
	#endif
}

void sendChar4(unsigned char i) {
	//E = 0
	PORTD &= ~(1 << PORTD7 ) ;
	//RS = 1
	PORTD |= (1 << PORTD5) ;

	//E = 1
	PORTD |= (1 << PORTD7) ;

	//4 bits mais significativos em PB4-PB7
	PORTB = (i & 0xf0) | (PORTB & 0x0f) ;

	//E = 0, RS = 0
	PORTD &= ~((1 <<PORTD7) | (1 << PORTD5));

	//RS = 1
	PORTD |= (1 << PORTD5) ;

	//E = 1
	PORTD |= (1 << PORTD7) ;

	//4 bits menos significativos em PB4-PB7
	PORTB = ((i & 0x0f) << 4) | (PORTB & 0x0f) ;

	//E = 0, RS = 0
	PORTD &= ~((1 <<PORTD7) | (1 << PORTD5))  ;
	waitBusy() ;

}



void gotoXY(unsigned char x, unsigned char y) {

	sendInst(0x80 | (y * 0x40) + x) ;

}

void writeStringXY(char *txt, unsigned char x, unsigned char y) {
	gotoXY(x,y) ;
	writeString(txt) ;
}

void writeString(char *txt) {
	#ifdef _LCD_4_BIT_MODE
		writeString4(txt) ;
	#else
		wirteString8(txt) ;
	#endif
}

void writeString8(char *txt) {
	while (*txt) {
		sendChar(*txt) ;
		txt++ ;
	}
	
}

void writeString4(char *txt) {
	while (*txt) {
		sendChar4(*txt) ;
		txt++ ;
	}
}

void lcd_init() {
	DDRD = 0xff ;
	DDRB = 0xff ;
	#ifdef _LCD_4_BIT_MODE
		sendInst8(0x28) ;
		sendInst4(0x28) ;
	#else
		sendInst8(0x38) ;
	#endif
}



