/*
Author: Thomas Stewart
*/

#ifndef MACROS_H
#define MACROS_H

//Simplicity Definitions
#define HIGH 1
#define LOW 0

#define LShift <<
#define RShift >>
#define AND &
#define OR |
#define COMP ~
#define XOR ^

//===============Main========================
//System State
#define STATE_WAITING 0
#define STATE_COST 1
#define STATE_PIN 2
#define STATE_FUNCTION 3
//===========================================

//=================LCD=======================
//Data Bus			//Port B0-B7
#define LCD_DATA_PORT	PORTB
#define LCD_DATA_DDR	DDRB
#define LCD_DATA_PIN	PINB

//Control Signals	//PORT D4-D6	//8 bit interface setup -- may need 4 bit interface mode
#define LCD_CNTRL_PORT	PORTD
#define LCD_CNTRL_DDR	DDRD
#define LCD_CNTRL_PIN	PIND
#define LCD_RS			PORTD4	//Data Signal = HIGH, Instruction Signal = LOW
#define LCD_RW			PORTD6	//Read = HIGH , Write = LOW
#define LCD_EN			PORTD7	//Enable
//===========================================

//=================UART======================

//===========================================

#endif