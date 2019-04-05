//System Variables
//===========================================
#define F_CPU 16000000UL // 16 MHz Clock
#define BAUD_1 38400
#define UBBR_0 F_CPU/16/BAUD_1-1
#define BAUD_2 9600
#define UBBR_1 F_CPU/16/BAUD_2-1
//===========================================

//Libraries
//===========================================
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//===========================================

//My Files
//===========================================
#include "macros.h"
#include "lcd.h"
#include "keypad.h"
#include "_3DES.h"
//===========================================

//Functions
//===========================================
void Initialise(void);
void Init_PWM();
void Init_Timer2(void);

char Keypad_GetKey(void);

void Init_UART0(unsigned int);
void UART0_Transmit_Char(char data);
void UART0_Transmit_String(char* string);
void UART0_CMD();
void UART0_KILL();
void UART0_RESTART();

void Init_UART1(unsigned int);
void UART1_Transmit_Char(char data);
void UART1_Transmit_String(char* string);
char UART1_Receive_Char(void);
void UART1_Receive_String(void);

void state_MainMenu();
void state_Request();
void state_InsertCard();
void state_GetPIN();
void state_Wait();
void state_RemoveCard();

void clear_Pin();
bool sc_Connected();
void sc_Reset();
void UART0_Message_Handler();
void UART1_Message_Handler();

//===========================================

//Variables Global
//===========================================
uint8_t state_terminal;				//Terminal FSM State
uint8_t state_keypad;				//Keypad FSM State
char finalKey=0xFF;		//Keypad Key

//UART 0 RX variables
volatile char UART0_RX_buffer[256];
volatile unsigned int UART0_RX_length = 0;
volatile char charRead;
volatile int messageRX_flag = 0;

volatile char UART1_RX_buffer[256];
volatile unsigned int UART1_RX_length = 0;
volatile char charRead_1;
volatile int messageRX_flag_1 = 0;

char *UART0_RX_message = NULL;
char *UART1_RX_message = NULL;
char *transaction_cost = NULL;
char *transaction_name = NULL;
bool transaction_ready = false;

// Pin information
char PIN[8];
int pin_Length = 0;

//Interrupt Variables
volatile unsigned int timer_Keypad;
//===========================================

ISR (TIMER2_COMPA_vect)
{
	if (timer_Keypad>0)
	{
		--timer_Keypad;
	}
}

ISR (USART0_RX_vect)
{
	// Read value from receive buffer
	charRead = UDR0;
	// Check if return carriage or newline
	if(charRead == '\r' || charRead == '\n')
	{
		// Set message flag
		messageRX_flag = 1;
		// Reset length counter
		UART0_RX_length = 0;
	}
	else
	{	
		// Add character to message buffer
		UART0_RX_buffer[UART0_RX_length] = charRead;
		// Increase buffer length
		UART0_RX_length++;
	}
}

ISR (USART1_RX_vect)
{
	charRead_1 = UDR1;
	//UART0_Transmit_Char(charRead_1);
	
	if(charRead_1 == '\r' || charRead_1 == '\0')
	{
		// Set message flag
		messageRX_flag_1 = 1;
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
/*
Initialisation of:
1) System State
2) LCD
3) Keypad
4) UART
5) Global Interrupt
*/
void Initialise()
{
	//System Initialisation
	//===========================================
	state_terminal = STATE_MAINMENU;
	//===========================================
	
	//LCD Initialisation
	//===========================================
	Init_LCD();
	LCD_Startup();		//Loading display
	LCD_Clear();		//Clear LCD
	LCD_Return();		//Return character to beginning
	//===========================================

	//Keypad Initialisation
	//===========================================
	Init_Keypad();
	Init_Timer2();		//Timer for debouncing
	state_keypad = KPState_NotPushed;
	//===========================================

	//UART Initialisation
	//===========================================
	Init_UART0(UBBR_0);
	Init_UART1(UBBR_1);
	//===========================================
	
	//Smart Card Switch
	DDRC  &= ~(1<<SC_Switch_I);
	PORTC |= (1<<SC_Switch_I);
	DDRC  &= ~(1<<SC_Reset_I);
	PORTC |= (1<<SC_Reset_I);
	
	//Variable Initialisation
	transaction_name = NULL;
	transaction_cost = NULL;
	transaction_ready = false;
	
	//PWM 
	//===========================================
	Init_PWM();
	//===========================================
	
	clear_Pin();
	
	//Global Interrupt Enable
	sei();
	
	_delay_ms(100);
}

//Timer for Keypad debouncing
void Init_Timer2()
{
	//Timer/Counter2, Output Compare A Match Interrupt Enable
	TIMSK2 = (1<<OCIE2A);
	//Counter number
	OCR2A = 254;
	//Prescalers:	1- /1, 2- /8, 3- /32, 4- /64, 5- /128, 6- /256, 7- /1024
	TCCR2B = 2;
	//Compare Mode
	TCCR2A = (1<<WGM21);
	//Set Counter
	timer_Keypad = KEYPAD_TIMER;
}

//PWM for smart card interface
void Init_PWM()
{
	// Set output port for PWM
	DDRD |= (1 << PWM_Port_P);
	// Set counter value
	OCR1A = 4;
	// Enable CTC mode and fast PWM
	TCCR1A |= (1 << COM1A0)|(1 << WGM11) | (1 << WGM10);
	TCCR1B |= (1 << WGM12)|(1 << WGM13)|(1 << CS10);
}

//Main program and FSM for system
int main(void)
{
	//Initialisation
	Initialise();
	//_delay_ms(1000);
	
	char keypad_key=0xFF;
	state_MainMenu();
	
	//Testing UART SC
// 	while(1)
// 	{
//  		
// 		
// 		keypad_key = Keypad_GetKey();
// 		switch(keypad_key)
// 		{
// 			case 'A':
// 			{
// 				UART1_Transmit_String("ACK");	
// 				break;
// 			}
// 			case 'B':
// 			{
// 				LCD_Clear();
// 				break;
// 			}
// 			case 'C':
// 			{
// 				//sc_Reset();
// 				break;
// 			}
// 			default:
// 			break;
// 		}		
// 	}
	
	//Encryption Testing
	
// 	while(1)
// 	{
// 		PIN[0]='A';
// 		PIN[1]='B';
// 		PIN[2]='C';
// 		PIN[3]='D';
// 		PIN[4]='E';
// 		PIN[5]='F';
// 		PIN[6]='G';
// 		PIN[7]='\0';
// 		keypad_key = Keypad_GetKey();
// 		switch(keypad_key)
// 		{
// 			case 'A':
// 			{
// 				LCD_Clear();
// 				LCD_String("SEND PIN");
// 				sendPin(PIN);				
// 				break;
// 			}
// 			case 'B':
// 			{
// 				LCD_Clear();
// 				LCD_String("RECEIVE PIN");
// 				receivePin(PIN);
// 				break;
// 			}
// 			default:
// 				break;
// 		}
// 	}
	
	while(1)
	{
		UART0_Message_Handler();
		
		//Get keypad key in case pushed
		keypad_key = Keypad_GetKey();
		
		//Check system state
		switch (state_terminal)
		{
			
			case STATE_MAINMENU:
			{	
				switch(keypad_key)
				{								
					case 'C':
					{
						UART0_RESTART();
						LCD_Clear();
						LCD_String("Bluetooth");
						LCD_Return_l2();
						LCD_String("Rebooted");
						_delay_ms(1000);
						state_MainMenu();
						break;
					}
					default:
						break;
				}
				break;
			}
			
			case STATE_REQUEST:
			{
				switch(keypad_key)
				{
					case 'A':
					{
						state_InsertCard();
						break;
					}
						
					// Cancel Transaction
					case 'C':
					{
						UART0_Transmit_String("SBSCR_CANCEL");
						LCD_Clear();
						LCD_String("Transaction");
						LCD_Return_l2();
						LCD_String("Cancelled");
						_delay_ms(1000);
						state_MainMenu();
						break;
					}
					default:
						break;
				}
				break;
			}
			
			case STATE_INSERTCARD:
			{
				// Check for smart card insertion
				// With switch
				if(sc_Connected() == true)
				{
					state_GetPIN();				
				}
				switch(keypad_key)
				{
					case 'C':
					{
						UART0_Transmit_String("SBSCR_CANCEL");
						LCD_Clear();
						LCD_String("Transaction");
						LCD_Return_l2();
						LCD_String("Cancelled");
						_delay_ms(1000);
						state_MainMenu();
						break;
					}
					default:
						break;
				}			
				break;
			}

			case STATE_GETPIN:
			{
				// Cancel transaction
				if(keypad_key == 'C')
				{
					UART0_Transmit_String("SBSCR_CANCEL");
					LCD_Clear();
					LCD_String("Transaction");
					LCD_Return_l2();
					LCD_String("Cancelled");
					clear_Pin();
					_delay_ms(1000);
					state_MainMenu();
				}
						
				// Clear Pin Entry
				else if(keypad_key == 'B')
				{
					state_GetPIN();
				}
						
				// Pin Complete
				else if(keypad_key == 'A' && pin_Length >= 4 && pin_Length <= 8)
				{
					//PIN[7] = '\0';
					//sendPin(PIN);		
					state_Wait();
				}
						
				// Ignore these entries
				else if(keypad_key == 'D' || keypad_key == 'E' || keypad_key == 'F' || keypad_key == 0xFF)
				{
					// Do Nothing
				}
						
				// Add number to PIN
				else if(pin_Length < 8 && Keypad_IsNum(keypad_key) == true)
				{
					PIN[pin_Length] = keypad_key;
					LCD_Write('*');
					//LCD_Write(PIN[pin_Length]);
					pin_Length++;
					//LCD_Write('*');
					//LCD_Write(PIN[pin_Length-1]);
				}
				else
				{
					//Do Nothing
				}				
				break;
			}
			
			case STATE_WAIT:
			{
				UART1_Message_Handler();
				break;	
			}
			
			case STATE_REMOVECARD:
			{
				if(sc_Connected() == false)
				{
					LCD_Clear();
					LCD_String("Thank You");
					clear_Pin();
					_delay_ms(2000);
					state_MainMenu();
				}	
				break;
			}
			
			default:
			{
				LCD_Clear();
				char err[]="Error Term State";
				LCD_String(err);
				break;
			}
		}
	}
	return 0;
}

//FSM and debouncing code for reading a key from the keypad
char Keypad_GetKey()
{
	char currentKey=0xFF;
	uint8_t key=0;
	
	if(timer_Keypad == 0)
	{
		timer_Keypad=KEYPAD_TIMER;
		key=Keypad_GetButton();
		
		switch(state_keypad)
		{
			//State 1 - Not Pushed
			case KPState_NotPushed:
			{
				if(key!=0)
				{
					state_keypad=KPState_Detect;
				}
				break;
			}
			
			//State 2 - Detect
			case KPState_Detect:
			{
				if(key!=0)
				{
					state_keypad=KPState_Pushed;
					key=Keypad_GetButton();
					finalKey=Keypad_NumToChar(key);
				}	
				else
				{
					state_keypad=KPState_NotPushed;
				}
				break;
			}
			
			//State 3 - Pushed
			case KPState_Pushed:
			{
				if(key==0)
				{
					state_keypad=KPState_Released;					
				}
				break;
			}
			
			//State 4 - Released
			case KPState_Released:
			{
				if(key==0)
				{
					state_keypad=KPState_NotPushed;
					currentKey=finalKey;
					finalKey=0xFF;
					_delay_us(5);
				}
				else
				{
					state_keypad=KPState_Pushed;
				}
				break;
			}
			default:
				state_keypad=KPState_NotPushed;
				break;
		}
	}
	return currentKey;
}

//Bluetooth
//===========================================
void Init_UART0(unsigned int myUBBR)
{
	//Set Baud Rate
	UBRR0H = (char)(myUBBR>>8);
	UBRR0L = (char)myUBBR;

	//Enable receiver and transmitter and interrupt
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	//UCSR0B = (1<<RXEN0)|(1<<TXEN0);
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
	UART0_Transmit_Char('\r');
	_delay_ms(15);
}

void UART0_CMD()
{
	UART0_Transmit_Char('$');
	UART0_Transmit_Char('$');
	UART0_Transmit_Char('$');
	_delay_ms(15);
}

void UART0_KILL()
{
	_delay_ms(15);
	UART0_CMD();
	UART0_Transmit_String("K,");
	_delay_ms(15);
	UART0_Transmit_String("---");
}

void UART0_RESTART()
{
	_delay_ms(15);
	UART0_CMD();
	UART0_Transmit_String("R,1");
	_delay_ms(15);
	UART0_Transmit_String("---");
}
//===========================================

//Smart card
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

char UART1_Receive_Char()
{
	//UCSR1B &= ~(1<<TXEN1);
	/* Wait for data to be received */
	while ( !(UCSR1A & (1<<RXC1)) );
	/* Get and return received data from buffer */
	//UCSR1B |= (1<<TXEN1);
	return UDR1;
}

void UART1_Transmit_String(char* string)
{
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

void UART1_Receive_String()
{
	int post;
	UART1_RX_length = 0;
	UART1_RX_buffer[UART1_RX_length] = UART1_Receive_Char();
	UART1_RX_length++;
	
	//Change to end of char when using smart card
	for(post = UART1_RX_length; UART1_RX_buffer[post-1] !='\0'; post++)
	{
		UART1_RX_buffer[post] = UART1_Receive_Char();
	}
	
	UART1_RX_buffer[post-1]='\0';
}
//===========================================

//Function Operations
//===========================================
void clear_Pin()
{
	pin_Length = 0;
	
	for(int i=0;i<8;i++)
	{
		PIN[i] = '0';
	}
}

bool sc_Connected()
{
	//If connected
	if((SC_Switch & (1 << SC_Switch_I)) == 0)
		return true;
	else
		return false;
}

void sc_Reset()
{
	PORTC &= ~(1<<SC_Reset_I);
	_delay_ms(500);
	PORTC |= (1<<SC_Reset_I);
}

void UART0_Message_Handler()
{
	if(messageRX_flag == 1)
	{
		UART0_RX_message = strdup(UART0_RX_buffer);
		
		//Determine interrupt read
		if(memcmp(UART0_RX_message,"CMD", strlen("CMD")) == 0)
		messageRX_flag = 0;
		else if(memcmp(UART0_RX_message, "END", strlen("END")) == 0)
		messageRX_flag = 0;
		else if(memcmp(UART0_RX_message, "AOK", strlen("AOK")) == 0)
		messageRX_flag = 0;
		else if(memcmp(UART0_RX_message, "KILL", strlen("KILL")) == 0)
		messageRX_flag = 0;
		
		else if(memcmp(UART0_RX_message,"SBSCR_CANCEL", strlen("SBSCR_CANCEL")) == 0)
		{
			LCD_Clear();
			LCD_String("Transaction");
			LCD_Return_l2();
			LCD_String("Cancelled");
			clear_Pin();
			_delay_ms(1000);
			state_MainMenu();
			//UART0_KILL();
			messageRX_flag = 0;
		}
		else if(memcmp(UART0_RX_message,"SBSCR_PAYMENTREQUEST", strlen("SBSCR_PAYMENTREQUEST\0")) == 0 && state_terminal == STATE_MAINMENU)
		{
			state_Request();
			messageRX_flag = 0;
		}
		else if(strstr(UART0_RX_message, "SBSCR_NAME: ") != NULL && state_terminal == STATE_REQUEST)
		{
			transaction_name = strdup(&UART0_RX_message[12]);
			LCD_String(transaction_name);
			messageRX_flag = 0;
			UART0_Transmit_String("SBSCR_GETCOST");
		}
		else if (strstr(UART0_RX_message, "SBSCR_COST: ") != NULL && state_terminal == STATE_REQUEST)
		{
			transaction_cost = strdup(&UART0_RX_message[12]);
			LCD_Return_l2();
			LCD_String(transaction_cost);
			transaction_ready = true;
			messageRX_flag = 0;
		}
		else
		{
			//UART0_rx_length = 0;
			messageRX_flag = 0;
		}		
	}
}

void UART1_Message_Handler()
{
	if(messageRX_flag_1 == 1)
	{
		_delay_ms(50);
		UART1_RX_message = strdup(UART1_RX_buffer);
		//LCD_String(UART1_RX_message);
		if(memcmp(UART1_RX_message,"START", strlen("START")) == 0)
		{
			UART1_Transmit_String("ACK");
			//state_GetPIN();
			messageRX_flag_1 = 0;
		}
		else if(memcmp(UART1_RX_message,"GETCOST", strlen("GETCOST")) == 0)
		{
			//LCD_Clear();
			//LCD_String(transaction_cost);
			//strlen(transaction_cost);
			//UART1_Transmit_String(transaction_cost);
			size_t length=0;
			while(transaction_cost[length]!='\0')
			{
				length++;
			}
			char data[5+length];
			data[0]='C';
			data[1]='O';
			data[2]='S';
			data[3]='T';
			data[4]=':';
			
			for(size_t y = 5;y<length + 5;y++)
			{
				data[y]=transaction_cost[y-5];
			}
 			UART1_Transmit_String(data);
			messageRX_flag_1 = 0;
		}
		else if(memcmp(UART1_RX_message,"GETPIN", strlen("GETPIN")) == 0)
		{
			sendPin(PIN);
			messageRX_flag_1 = 0;
		}
		//ACCEPTED
		else if(memcmp(UART1_RX_message,"ACCEPTED", strlen("ACCEPTED")) == 0)
		{
			LCD_Clear();
			LCD_String("Payment");
			LCD_Return_l2();
			LCD_String("accepted");
			UART0_Transmit_String("SBSCR_ACCEPTED");
			messageRX_flag_1 = 0;
			_delay_ms(2000);
			state_RemoveCard();			
		}
		//DECLINE
		else if(memcmp(UART1_RX_message,"DECLINE", strlen("DECLINE")) == 0)
		{
			LCD_Clear();
			LCD_String("Pin incorrect");
			UART0_Transmit_String("SBSCR_FAILED");
			messageRX_flag_1 = 0;
			_delay_ms(2000);
			state_RemoveCard();
		}
		//INSUFFICIENT
		else if(memcmp(UART1_RX_message,"INSUFFICIENT", strlen("INSUFFICIENT")) == 0)
		{
			LCD_Clear();
			LCD_String("Balance");
			LCD_Return_l2();
			LCD_String("insufficient");
			UART0_Transmit_String("SBSCR_FAILED");
			messageRX_flag_1 = 0;
			_delay_ms(2000);
			state_RemoveCard();
		}
		else
		{
			messageRX_flag_1 = 0;
		}
	}
}

//===========================================

//STATE FUNCTIONS
//===========================================
void state_MainMenu()
{
	state_terminal = STATE_MAINMENU;
	messageRX_flag = 0;
	UART0_KILL();
	LCD_Clear();
	char LCD_Main_L1[]="Waiting for ";
	LCD_String(LCD_Main_L1);
	LCD_Return_l2();
	char LCD_Main_L2[]="request";
	LCD_String(LCD_Main_L2);
	transaction_name = NULL;
	transaction_cost = NULL;
	transaction_ready = false;
}

void state_Request()
{
	state_terminal = STATE_REQUEST;
	LCD_Clear();
	UART0_Transmit_String("SBSCR_GETNAME");
}

void state_InsertCard()
{
	state_terminal=STATE_INSERTCARD;
	LCD_Clear();
	LCD_String("Insert ");
	LCD_Return_l2();
	LCD_String("smart card");
}

void state_GetPIN()
{
	state_terminal = STATE_GETPIN;
	clear_Pin();
	LCD_Clear();
	LCD_String("Enter Pin:");
	LCD_Return_l2();
}

void state_Wait()
{
	state_terminal = STATE_WAIT;
	
	LCD_Clear();
	LCD_String("Waiting for");
	LCD_Return_l2();
	LCD_String("Smart card");
	_delay_ms(1000);
	UART1_Transmit_String("START");	
}

void state_RemoveCard()
{
	state_terminal=STATE_REMOVECARD;
	LCD_Clear();
	LCD_String("Remove");
	LCD_Return_l2();
	LCD_String("smart card");
}
//===========================================

