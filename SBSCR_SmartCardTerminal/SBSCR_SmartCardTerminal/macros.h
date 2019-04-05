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
#define STATE_MAINMENU 0
#define STATE_REQUEST 1
#define STATE_INSERTCARD 2
#define STATE_GETPIN 3
#define STATE_WAIT 4
#define STATE_REMOVECARD 5

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

//===============Keypad======================				
//Ports for system
#define KEYPAD_PORT PORTA
#define KEYPAD_DDR	DDRA
#define KEYPAD_PIN	PINA

//Key Count
#define KEYPAD_KEYCOUNT 16

//Timer value for debouncing
#define KEYPAD_TIMER 50

//Keys
#define KEY_0	'0'
#define KEY_1	'1'
#define KEY_2	'2'
#define KEY_3	'3'
#define KEY_4	'4'
#define KEY_5	'5'
#define KEY_6	'6'
#define KEY_7	'7'
#define KEY_8	'8'
#define KEY_9	'9'
#define KEY_A	'A'
#define KEY_B	'B'
#define KEY_C	'C'
#define KEY_D	'D'
#define KEY_E	'E'
#define KEY_F	'F'

//FSM States
#define KPState_NotPushed	1
#define	KPState_Detect		2
#define KPState_Pushed		3
#define KPState_Released	4
//===========================================

//Smart Card Switch
#define SC_Switch_I PORTC7
#define SC_Switch PINC
#define SC_Reset_I PORTC6

//PWM
#define PWM_Port_P PORTD5
#define PWM_Port PORTD

#endif