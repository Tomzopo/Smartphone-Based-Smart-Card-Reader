/*
Author: Thomas Stewart
*/

#ifndef LCD_H
#define LCD_H

#include <util/delay.h>
#include "macros.h"

//Main Functions
//===========================================
void Init_LCD(void);

void LCD_Busy(void);
void LCD_Command(char);
void LCD_Command_NoBusy(char);
void LCD_Write(char var);
void LCD_String(char *var);
//===========================================

//Commands
//===========================================
void LCD_Clear(void);
void LCD_Return(void);
void LCD_Return_l2(void);
void LCD_EntryMode(void);
void LCD_DisplayOn_CursorOff(void);
void LCD_DisplayOn_CursorOn(void);
void LCD_DisplayOn_Blinking(void);
void LCD_DisplayOff(void);
void LCD_ShiftLeft(void);
void LCD_ShiftRight(void);
void LCD_SetFunction_8bit_1l_5x7(void);
void LCD_SetFunction_8bit_2l_5x7(void);
void LCD_SetFunction_4bit_1l_5x7(void);
void LCD_SetFunction_4bit_1l_5x7(void);
void LCD_SetFunction_8bit_1l_5x10(void);
void LCD_SetFunction_8bit_2l_5x10(void);
void LCD_SetFunction_4bit_1l_5x10(void);
void LCD_SetFunction_4bit_1l_5x10(void);

void LCD_Startup(void);
//===========================================


void Init_LCD()
{
	LCD_CNTRL_DDR |= (1<<LCD_RS);
	LCD_CNTRL_DDR |= (1<<LCD_RW);
	LCD_CNTRL_DDR |= (1<<LCD_EN);
	LCD_CNTRL_PORT = 0x00;
	
	LCD_DATA_DDR = 0xFF;
	LCD_DATA_PORT = 0x00;
	
	_delay_ms(50);
	
	LCD_SetFunction_8bit_2l_5x7();
	LCD_DisplayOn_CursorOff();
	LCD_Clear();
	LCD_EntryMode();
}

void LCD_Busy()
{
	LCD_DATA_DDR = 0x00;
	LCD_CNTRL_PORT &= ~(1<<LCD_RS);
	LCD_CNTRL_PORT |= (1<<LCD_RW);
	LCD_CNTRL_PORT |= (1<<LCD_EN);
	_delay_us(40);
	
	LCD_DATA_DDR = 0xFF;
}

void LCD_Command(char command)
{
	LCD_DATA_PORT = command;
	
	LCD_CNTRL_PORT &= ~(1<<LCD_RW);
	LCD_CNTRL_PORT &= ~(1<<LCD_RS);
	LCD_CNTRL_PORT |= (1<<LCD_EN);
	_delay_us(2);
	LCD_CNTRL_PORT &= ~(1<<LCD_EN);
	
	LCD_Busy();
}

void LCD_Command_NoBusy(char command)
{
	LCD_DATA_PORT = command;
	
	LCD_CNTRL_PORT &= ~(1<<LCD_RW);
	LCD_CNTRL_PORT &= ~(1<<LCD_RS);
	LCD_CNTRL_PORT |= (1<<LCD_EN);
	_delay_us(1);
	LCD_CNTRL_PORT &= ~(1<<LCD_EN);
}

void LCD_Write(char var)
{
	LCD_DATA_PORT = var;
	
	LCD_CNTRL_PORT |= (1<<LCD_RS);
	LCD_CNTRL_PORT &= ~(1<<LCD_RW);
	LCD_CNTRL_PORT |= (1<<LCD_EN);
	_delay_us(2);
	LCD_CNTRL_PORT &= ~(1<<LCD_EN);
	
	LCD_Busy();
}

void LCD_String(char *var)
{
	int temp = 0;
	while(var[temp]!='\0')
	{
		LCD_Write(var[temp]);
		temp++;
	}
}

void LCD_Clear()
{
	LCD_Command(0x01);
	_delay_ms(2);
	LCD_Return();
}

void LCD_Return()
{
	LCD_Command(0x03);
	_delay_ms(2);
}

void LCD_Return_l2()
{
	LCD_Command(0xC0);
	_delay_ms(2);
}

void LCD_ArrowUp()
{
	LCD_Command(0x8E);
	LCD_Write('F');
	LCD_Write(0x7F);
	_delay_us(10);
}

void LCD_ArrowDown()
{
	LCD_Command(0xCE);
	LCD_Write('E');
	LCD_Write(0x7E);
	_delay_us(10);
}

void LCD_EntryMode()
{
	LCD_Command(0x06);
	_delay_us(42);
}

void LCD_DisplayOn_CursorOff()
{
	LCD_Command(0x0C);
	_delay_us(42);
}

void LCD_DisplayOn_CursorOn()
{
	LCD_Command(0x0E);
	_delay_us(42);
}

void LCD_DisplayOn_Blinking()
{
	LCD_Command(0x0F);
	_delay_us(42);
}

void LCD_DisplayOff()
{
	LCD_Command(0x08);
	_delay_us(42);
}

void LCD_ShiftLeft()
{
	LCD_Command(0x18);
	_delay_us(42);
}

void LCD_ShiftRight()
{
	LCD_Command(0x1C);
	_delay_us(42);
}

void LCD_SetFunction_8bit_1l_5x7()
{
	LCD_Command(0x30);
	_delay_us(42);
}

void LCD_SetFunction_8bit_2l_5x7()
{
	LCD_Command(0x38);
	_delay_us(42);
}

void LCD_SetFunction_4bit_1l_5x7()
{
	LCD_Command(0x20);
	_delay_us(42);
}

void LCD_SetFunction_4bit_2l_5x7()
{
	LCD_Command(0x28);
	_delay_us(42);
}

void LCD_SetFunction_8bit_1l_5x10()
{
	LCD_Command(0x34);
	_delay_us(42);
}

void LCD_SetFunction_8bit_2l_5x10()
{
	LCD_Command(0x3C);
	_delay_us(42);
}

void LCD_SetFunction_4bit_1l_5x10()
{
	LCD_Command(0x24);
	_delay_us(42);
}

void LCD_SetFunction_4bit_2l_5x10()
{
	LCD_Command(0x2C);
	_delay_us(42);
}

void LCD_Startup()
{
	char var1[]="Welcome";
	char var2[]="Loading";
	char var3[]="Loading.";
	char var4[]="Loading..";
	char var5[]="Loading...";
	
	for(int i=0;i<2;i=i+1)
	{
		LCD_Clear();
		LCD_String(var1);
		LCD_Return_l2();
		LCD_String(var2);
		_delay_ms(200);
		LCD_Clear();
		LCD_String(var1);
		LCD_Return_l2();
		LCD_String(var3);
		_delay_ms(200);
		LCD_Clear();
		LCD_String(var1);
		LCD_Return_l2();
		LCD_String(var4);
		_delay_ms(200);
		LCD_Clear();
		LCD_String(var1);
		LCD_Return_l2();
		LCD_String(var5);
		_delay_ms(200);
	}
}

#endif
