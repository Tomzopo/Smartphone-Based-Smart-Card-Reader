#ifndef KEYPAD_H
#define KEYPAD_H

#include <util/delay.h>

void Init_Keypad(void);
uint8_t Keypad_GetButton(void);
char Keypad_NumToChar(uint8_t);
bool Keypad_IsNum(char);

//Keypad Setup
//======================================================================================
//				[X1]		[X2]		[X3]		[X4]
//		[Y1]	|1|			|2|			|3|			|F|
//		[Y2]	|4|			|5|			|6|			|E|
//		[Y3]	|7|			|8|			|9|			|D|
//		[Y4]	|A|			|0|			|B|			|C|
//
// 				[X1]		[X2]		[X3]		[X4]
// 		[Y1]	{1110 1110}	{1110 1101}	{1110 1011}	{1110 0111}
// 		[Y2]	{1101 1110}	{1101 1101} {1101 1011} {1101 0111}
// 		[Y3]	{1011 1110}	{1011 1101} {1011 1011} {1011 0111}
// 		[Y4]	{0111 1110}	{0111 1101} {0111 1011} {0111 0111}
//
// 				[X1] 		[X2] 		[X3] 		[X4]
// 		[Y1]	{e e} 		{e d} 		{e b} 		{e 7}
// 		[Y2]	{d e} 		{d d} 		{d b} 		{d 7}
// 		[Y3]	{b e} 		{b d} 		{b b} 		{b 7}
// 		[Y4]	{7 e} 		{7 d} 		{7 b} 		{7 7}
//
//
// 	   |1|	|2|  |3|  |4|
// 		E -> D -> B -> 7
// 		1110 1101 1011 0111
//======================================================================================

char keypad_States[16]=  {	0xEE, 0xED, 0xEB, 0xE7,
									0xDE, 0xDD, 0xDB, 0xD7,
									0xBE, 0xBD, 0xBB, 0xB7,
									0x7E, 0x7D, 0x7B, 0x77};

void Init_Keypad()
{
	KEYPAD_DDR	= 0xF0;
	KEYPAD_PORT	= 0x0F;
}									

uint8_t Keypad_GetButton()
{
	char buttonState;
	uint8_t buttonNumber;

	//Set ports and directions
	KEYPAD_DDR	= 0xF0;
	KEYPAD_PORT	= 0x0F;
	_delay_us(5);
	buttonState = KEYPAD_PIN;
	
	//Change ports and directions
	KEYPAD_DDR	= 0x0F;
	KEYPAD_PORT	= 0xF0;
	_delay_us(5);
	buttonState = buttonState | KEYPAD_PIN;

	if(buttonState!=0xFF)
	{
		//Shifted to work from 1-16
		for(buttonNumber = 1; buttonNumber <= KEYPAD_KEYCOUNT; buttonNumber = buttonNumber + 1)
		{
			if(keypad_States[buttonNumber-1]==buttonState)
			{
				break;
			}
		}

		if(buttonNumber == KEYPAD_KEYCOUNT+1)
		{
			buttonNumber=0;
		}
	}
	else
	{
		buttonNumber=0;
	}
	return buttonNumber;
}

char Keypad_NumToChar(uint8_t num)
{
	char value=0xFF;

	switch (num)
	{
		case 1 : 
			value=KEY_1;
			break;
		case 2:
			value=KEY_2;
			break;
		case 3:
			value=KEY_3;
			break;
		case 4:
			value=KEY_F;
			break;
		case 5:
			value=KEY_4;
			break;
		case 6:
			value=KEY_5;
			break;
		case 7:
			value=KEY_6;
			break;
		case 8:
			value=KEY_E;
			break;
		case 9:
			value=KEY_7;
			break;
		case 10:
			value=KEY_8;
			break;
		case 11:
			value=KEY_9;
			break;
		case 12:
			value=KEY_D;
			break;
		case 13:
			value=KEY_A;
			break;
		case 14:
			value=KEY_0;
			break;
		case 15:
			value=KEY_B;
			break;
		case 16:
			value=KEY_C;
			break;
	}

	return value;
}

bool Keypad_IsNum(char key)
{
	bool value = false;
	
	switch (key)
	{
		case '1':
		value=true;
		break;
		case '2':
		value=true;
		break;
		case '3':
		value=true;
		break;
		case '4':
		value=true;
		break;
		case '5':
		value=true;
		break;
		case '6':
		value=true;
		break;
		case '7':
		value=true;
		break;
		case '8':
		value=true;
		break;
		case '9':
		value=true;
		break;
		case '0':
		value=true;
		break;
	}
	return value;
}

#endif
