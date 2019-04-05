/*
Author: Thomas Stewart
*/

//System Variables
//===========================================
#define F_CPU 16000000UL // 16 MHz Clock
#define BAUD_1 9600
#define UBBR_0 F_CPU/16/BAUD_1-1
#define BAUD_2 9600
#define UBBR_1 F_CPU/16/BAUD_2-1
//===========================================

//Libraries
//===========================================
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//===========================================

//My Files
//===========================================
#include "macros.h"
#include "lcd.h"
#include "boxes.h"
#include "_3DES.h"
//===========================================

//Functions
//===========================================
void Initialise(void);

uint8_t EEPROM_read_byte(int);
void EEPROM_read_block(uint8_t[8], int);

void EEPROM_write_byte(uint8_t, int);
//void EEPROM_write_block(uint8_t[8], int);

void Init_UART0(unsigned int);
void UART0_Transmit_Char(char data);
void UART0_Transmit_String(char* string);
char UART0_Receive_Char();
void UART0_Receive_String();

void Init_UART1(unsigned int);
void UART1_Transmit_Char(char data);
void UART1_Transmit_String(char* string);
//char UART1_Receive_Char(void);
//void UART1_Receive_String(void);
void UART1_Message_Handler();
void clear();
//===========================================

//Variables Global
//===========================================
int EEPROM_CURR_ADDRESS = 0;

uint8_t sys_state = STATE_WAITING;
double cost = 0;

volatile char UART0_RX_buffer[256];
volatile unsigned int UART0_RX_length = 0;
volatile char charRead;
volatile int messageRX_flag = 0;

volatile char UART1_RX_buffer[256];
volatile unsigned int UART1_RX_length = 0;
volatile char charRead_1;
volatile int messageRX_flag_1 = 0;

char *UART1_RX_message = NULL;
char *transaction_cost = NULL;
char encrypted_PIN[8];
char received_PIN[8];



int pin_Length = 0;
//===========================================

ISR (USART1_RX_vect)
{
	charRead_1 = UDR1;
	//UART0_Transmit_Char(charRead_1);                             
	
	if(charRead_1 == '\r' || charRead_1 == '\0')
	{
		// Set message flag
		messageRX_flag_1 = 1;
		UART1_RX_buffer[UART1_RX_length] = charRead_1;
		// Reset length counter
		UART1_RX_length = 0;
	}
	else
	{
		// Add character to message buffer
		UART1_RX_buffer[UART1_RX_length] = charRead_1;
		// Increase buffer length
		UART1_RX_length++;
	}
}

void Initialise()
{
	sys_state = STATE_WAITING;
	//LCD Initialisation
	//===========================================
// 	Init_LCD();
// 	LCD_Startup();		//Loading display
// 	LCD_Clear();		//Clear LCD
// 	LCD_Return();		//Return character to beginning
	//===========================================
	
	//UART0 Initialisation
	//===========================================
	Init_UART0(UBBR_0);
	Init_UART1(UBBR_1);
	//===========================================
	sei();
	
	//_delay_ms(1000);
	
}

int main(void)
{
	Initialise();

    //encryption3DES( (unsigned char*) "zxcvbnm,", (unsigned char*) "abcdefgh", (unsigned char*) "ZYXWVUTS", (unsigned char*) "ABCDEFGH");
	//decryption3DES( (unsigned char*) "zxcvbnm,", (unsigned char*) "abcdefgh", (unsigned char*) "ZYXWVUTS", (unsigned char*) "ABCDEFGH");
	
	/*EEPROM_write_byte(';' ,0);
	uint8_t d = EEPROM_read_byte(0);
	UART0_Transmit_Char(d);*/
	
	//EEPROM_write_block();
	//EEPROM_read_block();
	
	//UART0_Transmit_Char('Z');
	//char temp = UART0_Receive_Char();
	//encryption3DES(pin1, "12345678", pin3, "ABCDEFGH");
	//uint8_t temp[8]={27, 228, 18, 212, 151, 217, 30, 161};
	//decryption3DES(pin1, "12345678", pin3, temp);

	//UART1_Transmit_String("START");
	//UART1_Transmit_String("COST");
	//_delay_ms(1000);
	
	UART0_Transmit_String("ON\r\n");
	createBalance();
	
	while(1)
	{	
		UART1_Message_Handler();
		switch(sys_state)
		{
			case STATE_WAITING:
			{
				//Waiting for messages
				break;
			}
			
			case STATE_COST:
			{
				//Waiting for messages
				break;
			}
			
			case STATE_PIN:
			{
				//Waiting for messages
				break;
			}
			case STATE_FUNCTION:
			{
				if(PIN_Validate(received_PIN))
				{
					UART0_Transmit_String("PIN VALID\r\n");
 					sc_function(cost);
					clear();
				}
				else
				{
					UART0_Transmit_String("PIN INVALID\r\n");
					UART1_Transmit_String("DECLINE");
					clear();
				}		
				break;
			}
			default:
			{
				sys_state=STATE_WAITING;
				break;
			}			
		}
	}
}

// EEPROM
//===========================================
uint8_t EEPROM_read_byte(int address)
{
	uint8_t dataByte;
	dataByte = eeprom_read_byte (( uint8_t *) address) ;
	return dataByte;
}

void EEPROM_read_block(uint8_t readData[8], int start_address)
{
	uint8_t tempRead[8];
	eeprom_read_block (( void *) tempRead, ( const void *) 0 , 8) ;
	
	//uint8_t user_PIN[8];

// 	uint8_t datato[8];
// 	for(int i=0; i<8; i++)
// 	{
// 		//user_PIN[i]=PIN[i];
// 		datato[i]=tempRead[i];
// 	}
	
	uint8_t data_dec[8]="";
 	decryption3DES("ABCDEFGH", PIN, "12345678", tempRead, data_dec);
	
	UART0_Transmit_String("\r\n");
	for(size_t j = 0; j < 8; j++)
	{
		readData[j]=data_dec[j];
		char buffer[3];
		itoa(readData[j] , buffer, 10);
		UART0_Transmit_String(buffer);
		UART0_Transmit_String("\t");
	}
	UART0_Transmit_String("\r\n");
}
	
void EEPROM_write_byte(uint8_t dataByte, int address)
{
	eeprom_update_byte (( uint8_t *) address, dataByte);
}
//===========================================

// UART0
// Need to setup specifics
//===========================================
void Init_UART0(unsigned int myUBBR)
{
	//Set Baud Rate
	UBRR0H = (char)(myUBBR>>8);
	UBRR0L = (char)myUBBR;

	//Enable receiver and transmitter and interrupt
	//UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	//Set frame format: 8data, 1stop bit
	UCSR0C = (3<<UCSZ00);
	
	DDRD &= ~(1<<PORTD0);
	DDRD |= (1<<PORTD1);
}

void UART0_Transmit_Char(char data)
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void UART0_Transmit_String(char* string)
{
	int length;

	for(length = 0; string[length] !='\0'; length++)
	{
		UART0_Transmit_Char(string[length]);
	}
	//UART0_Transmit_Char('\r');
}

char UART0_Receive_Char()
{
	UCSR0B &= ~(1<<TXEN0);
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	UCSR0B |= (1<<TXEN0);
	return UDR0;
}

void UART0_Receive_String()
{
	int post;
	UART0_RX_length = 0;
	UART0_RX_buffer[UART0_RX_length] = UART0_Receive_Char();
	UART0_RX_length++;
	
	for(post = UART0_RX_length; UART0_RX_buffer[post-1] !='\0'; post++)
	{
		UART0_RX_buffer[post] = UART0_Receive_Char();
	}
	UART0_RX_buffer[post-1]='\0';
}
//===========================================

//Smart card debug console
//===========================================
void Init_UART1(unsigned int myUBBR)
{
	//Set Baud Rate
	UBRR1H = (char)(myUBBR>>8);
	UBRR1L = (char)myUBBR;

	//Enable receiver and transmitter
	//UCSR1B |= (1<<RXEN1)|(1<<TXEN1);
	UCSR1B |= (1<<RXCIE1)|(1<<RXEN1)|(1<<TXEN1);

	//Set frame format: 8data, 1stop bit
	UCSR1C = (3<<UCSZ10);

	DDRD &= ~(1<<PORTD2);
	DDRD |= (1<<PORTD3);
}

void UART1_Transmit_Char(char data)
{
	// 	/* Wait for empty transmit buffer */

	UCSR1B &= ~(1<<RXEN1);
	while ( !( UCSR1A & (1<<UDRE1)) );
	// 	/* Put data into buffer, sends the data */
	UDR1 = data;

	
	UCSR1B |= (1<<RXEN1);
 	while ( !(UCSR1A & (1<<RXC1)) );
 	char temp=UDR1;
}

// char UART1_Receive_Char()
// {
// 	//UCSR1B &= ~(1<<TXEN1);
// 	/* Wait for data to be received */
// 	while ( !(UCSR1A & (1<<RXC1)) );
// 	/* Get and return received data from buffer */
// 	//UCSR1B |= (1<<TXEN1);
// 	return UDR1;
// }

void UART1_Transmit_String(char* string)
{
	UART0_Transmit_String("SC:\r\n");
	UART0_Transmit_String(string);
	UART0_Transmit_String("\r\n");
	cli();
	for(int length = 0; string[length]!='\0'; length++)
	{
		UART1_Transmit_Char(string[length]);
	}
	UART1_Transmit_Char('\0');
	//while(!(UCSR1A&(1<<TXC1)));
	sei();
	//UART1_Receive_String();
}

// void UART1_Receive_String()
// {
// 	int post;
// 	UART1_RX_length = 0;
// 	UART1_RX_buffer[UART1_RX_length] = UART1_Receive_Char();
// 	UART1_RX_length++;
// 	
// 	//Change to end of char when using smart card
// 	for(post = UART1_RX_length; UART1_RX_buffer[post-1] !='\0'; post++)
// 	{
// 		UART1_RX_buffer[post] = UART1_Receive_Char();
// 	}
// 	
// 	UART1_RX_buffer[post-1]='\0';
// }
//===========================================

void UART1_Message_Handler()
{
	if(messageRX_flag_1 == 1)
	{
		_delay_ms(50);
		UART1_RX_message = strdup(UART1_RX_buffer);
		UART0_Transmit_String("TERM:\r\n");
		UART0_Transmit_String(UART1_RX_message);
		UART0_Transmit_String("\r\n");
		if(memcmp(UART1_RX_message,"START", strlen("START")) == 0)
		{
			UART1_Transmit_String("START");
			messageRX_flag_1 = 0;
		}
		else if(memcmp(UART1_RX_message,"ACK", strlen("ACK")) == 0)
		{
			UART1_Transmit_String("GETCOST");
			sys_state = STATE_COST;
			messageRX_flag_1 = 0;
		}
		else if(memcmp(UART1_RX_message,"COST:", strlen("COST:")) == 0 && sys_state == STATE_COST)
		{
			transaction_cost = strdup(&UART1_RX_message[5]);
			UART0_Transmit_String(transaction_cost);
			cost = strtod(transaction_cost, NULL);		
			sys_state = STATE_PIN;
			messageRX_flag_1 = 0;
			UART1_Transmit_String("GETPIN");
		}
		else if(memcmp(UART1_RX_message,"PIN:", strlen("PIN:")) == 0 && sys_state == STATE_PIN)
		//else if(memcmp(UART1_RX_message,"PIN:", strlen("PIN:")) == 0)
		{
			//encrypted_PIN = strdup(&UART1_RX_message[5]);
			for(size_t t=0;t<8;t++)
			{			
				encrypted_PIN[t]=UART1_RX_message[t+4];
			}
			receivePin(encrypted_PIN, received_PIN);
			sys_state = STATE_FUNCTION;
			messageRX_flag_1 = 0;
		}
		else
		{
			messageRX_flag_1 = 0;
		}
	}
}

void clear()
{
	sys_state=STATE_WAITING;
	cost=0;
	transaction_cost=NULL;
	for(size_t i=0;i<8;i++)
	{
		received_PIN[i]=0;
		encrypted_PIN[i]=0;
	}
}