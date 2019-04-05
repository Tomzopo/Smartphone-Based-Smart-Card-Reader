/*
Author: Thomas Stewart
*/

#ifndef _3DES_H
#define _3DES_H
/*
File for running a 3DES encryption
//Need to add malloc
*/
#include "boxes.h"

//3DES Variables
//===========================================
//===========================================

//Libraries
//===========================================
//#include "lcd.h"
//#include "macros.h"
//#include <math.h>
//===========================================

//Functions
//===========================================

//Simple Functions
void bit2byte_1DArr(uint8_t bitArr[], size_t, uint8_t[]); //Tested - Working
void byte2bit_1DArr(uint8_t byteArr[], size_t, uint8_t[]); //Tested - Working
void flip(uint8_t[16][6], uint8_t[16][6]); //Tested - Working

//DES Functions
void encryption3DES(uint8_t[8], uint8_t[8], uint8_t[8], uint8_t*, uint8_t[8]); //Tested - Working
void decryption3DES(uint8_t[8], uint8_t[8], uint8_t[8], uint8_t*, uint8_t[8]); //Tested - Working
void shift_Left(uint8_t[28], uint8_t, uint8_t); //Tested - Working
void substitution_table(uint8_t[], uint8_t[], uint8_t*, uint8_t); //Tested - Working
uint8_t sbox_getIndex(uint8_t, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t); //Tested - Working
void substitution_sbox(uint8_t[64], uint8_t[32]); //Tested - Working
void key_schedule(uint8_t[8], uint8_t[16][6]); //Tested - Working
void feistel_function(uint8_t[4], uint8_t[6], uint8_t[4]); //Tested - Working
void feistel_loop(uint8_t[8], uint8_t[16][6], uint8_t[8]); //Tested - Working
void main_function_ECB(uint8_t[8], uint8_t[8], uint8_t[8], uint8_t*, uint8_t); //Tested - Working
void sendPin(uint8_t[8]);
void receivePin(uint8_t[8], uint8_t[8]);

//UART1
void UART1_Transmit_Char(char);
void UART1_Transmit_String(char*);
//===========================================
uint8_t key_1_plain_pin[8] = "THOMAS15";
uint8_t key_2_plain_pin[8] = "DAVIDAPR";
uint8_t key_3_plain_pin[8] = "STEWART4";

//Simple Functions
//===========================================
//Tested - Working
void bit2byte_1DArr(uint8_t bitArr[], size_t bitArr_Size , uint8_t byteArr[])
{
	// Byte Counter
	size_t i;
	// Bit Counter
	size_t j;
	
	for(i = 0; i < (bitArr_Size / 8); i++)
	{	
		uint8_t num = 0;
		for(j = 0; j < 8; j++)
		{
			// Perform 2^bit via left shift method
			// Add all the bits to num
			num = num + (1 << (7 - j)) * (bitArr[8*i + j]);	
		}
		// Store the value of num in the byte array
		byteArr[i] = num;
	}
}

//Tested - Working
void byte2bit_1DArr(uint8_t byteArr[], size_t byteArr_Size, uint8_t bitArr[])
{
	// Byte counter
	size_t i = 0;
	// Bit counter
	size_t j = 0;
	
	for(i = 0; i < byteArr_Size; i++)
	{
		uint8_t bin = 0;
		for(j = 0; j < 8; j++)
		{
			/* 
			Convert bytes to bits by performing remainder 
			calculations on 2. The number is then 
			divided by 2 and rounded down. 
			This is done from MSB to Least to determine 
			8-bit representation.
			*/
			bin = (byteArr[i] % 2);
			byteArr[i] = byteArr[i] / 2;
			/* 
			Each bit is added to the bit array, forming 1 
			array for all bytes.
			*/
			bitArr[(i*8) + 7 - j] = (bin);
		}
	}
}

//Tested - Working
/*
Function to flip subkey array for decryption.
Subkey[0] <-> Subkey[15]
Subkey[1] <-> Subkey[14]
Subkey[2] <-> Subkey[13]
Subkey[3] <-> Subkey[12]
etc...
*/
void flip(uint8_t input_arr[16][6], uint8_t output_arr[16][6])
{
	for(int j = 0; j < 16; j++)
	{
		for(int i = 0; i < 6; i++)
		{
			output_arr[15-j][i] = input_arr[j][i];
		}
	}
}
//===========================================


//Functions
//===========================================
//Tested - Working
void encryption3DES(uint8_t key_1_plain[8], uint8_t key_2_plain[8], uint8_t key_3_plain[8], uint8_t *plaintext, uint8_t message_encrypted[8])
{
	// Copy original for comparison
	//uint8_t message_before[strlen((const char*) plaintext)];
	//strncpy((char*) message_before, (const char*) plaintext, strlen((const char*) plaintext));

	//UART1_Transmit_String("\r\nEncryption Process\r\n");
	//UART1_Transmit_String("************************************************************************************************\r\n");
	
	//UART1_Transmit_String("Encryption Step 1 - Encrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");
	
	// Create subkey array
	uint8_t subkeys_byteArr1[16][6];
	key_schedule(key_1_plain, subkeys_byteArr1);
	//UART1_Transmit_String("Message Before: ");
	//UART1_Transmit_String((char*) plaintext);
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("Message Before (DECIMAL): ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(plaintext[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	
	// Create message array for first encryption step, then perform feistel loop
	uint8_t message_byteArr_Step1[8];
	
	//Encryption and printing of final result
	feistel_loop(plaintext, subkeys_byteArr1, message_byteArr_Step1);
	//UART1_Transmit_String("Message After: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step1[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');	
	}*/
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");


	//Decryption step
	//UART1_Transmit_String("Encryption Step 2 - Decrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");

	//Generate subkeys for key 2
	uint8_t subkeys_byteArr2[16][6];
	key_schedule(key_2_plain, subkeys_byteArr2);
	//Flip array for decryption
	uint8_t subkeys_byteArr2_flip[16][6];
	flip(subkeys_byteArr2, subkeys_byteArr2_flip);
	//Print message before
	//UART1_Transmit_String("Message Before: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step1[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	uint8_t message_byteArr_Step2[8];
	// Perform decryption
	// Print result
	feistel_loop(message_byteArr_Step1, subkeys_byteArr2_flip, message_byteArr_Step2);
	//UART1_Transmit_String("Message After: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step2[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");

	// Encryption step 3
	//UART1_Transmit_String("Encryption Step 3 - Encrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");
	// Generate subkeys with key 3
	uint8_t subkeys_byteArr3[16][6];
	key_schedule(key_3_plain, subkeys_byteArr3);
	//UART1_Transmit_String("Message Before: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step2[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	uint8_t message_byteArr_Step3[8];
	// Perform encryption
	feistel_loop(message_byteArr_Step2, subkeys_byteArr3, message_byteArr_Step3);	
	
	for(int i = 0; i < 8; i++)
	{
		message_encrypted[i] = message_byteArr_Step3[i];
	}
	
	//UART1_Transmit_String("Message After: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step3[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");

	//print comparison of details before and after for comparison
	//UART1_Transmit_String("Message before encryption: ");
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_before[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("Message after encryption: ");
	
	/*for(int i = 0; i < 8; i++)
	{
		char buffer[3];
		itoa(message_byteArr_Step3[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	}*/
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("************************************************************************************************\r\n");
	
	
}

//Tested - Working
void decryption3DES(uint8_t key_1_plain[8], uint8_t key_2_plain[8], uint8_t key_3_plain[8], uint8_t *ciphertext, uint8_t message_decrypted[8])
{
	//uint8_t message_before[strlen((const char*) ciphertext)];
	//strncpy((char*) message_before, (const char*) ciphertext, strlen((const char*) ciphertext));
	//UART1_Transmit_String("\r\nDecryption Process\r\n");
	//UART1_Transmit_String("************************************************************************************************\r\n");
	
	//UART1_Transmit_String("Decryption Step 1 - Decrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");
	uint8_t subkeys_byteArr1[16][6];
	//Generate subkeys with key 3
	key_schedule(key_3_plain, subkeys_byteArr1);
	uint8_t subkeys_byteArr1_flip[16][6];
	//Flip for decryption
	flip(subkeys_byteArr1, subkeys_byteArr1_flip);
	//UART1_Transmit_String("Message Before: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(ciphertext[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	uint8_t message_byteArr_Step1[8];
	//Perform decryption
	feistel_loop(ciphertext, subkeys_byteArr1_flip, message_byteArr_Step1);
	//UART1_Transmit_String("Message After: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step1[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");

	//UART1_Transmit_String("Decryption Step 2 - Encrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");
	uint8_t subkeys_byteArr2[16][6];
	// Generate subkeys with key 2
	key_schedule(key_2_plain, subkeys_byteArr2);
	//UART1_Transmit_String("Message Before: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step1[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	uint8_t message_byteArr_Step2[8];
	// Perform encryption
	feistel_loop(message_byteArr_Step1, subkeys_byteArr2, message_byteArr_Step2);
	//UART1_Transmit_String("Message After: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step2[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");

	
	//UART1_Transmit_String("Decryption Step 3 - Decrypt\r\n");
	//UART1_Transmit_String("================================================\r\n");
	uint8_t subkeys_byteArr3[16][6];
	// Generate subkeys with key3
	key_schedule(key_1_plain, subkeys_byteArr3);
	//UART1_Transmit_String("Message Before: ");
	// Flip array
	uint8_t subkeys_byteArr3_flip[16][6];
	flip(subkeys_byteArr3, subkeys_byteArr3_flip);
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step2[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	uint8_t message_byteArr_Step3[8];
	// Perform decryption
	feistel_loop(message_byteArr_Step2, subkeys_byteArr3_flip, message_byteArr_Step3);
	
	for(int i = 0; i < 8; i++)
	{
		message_decrypted[i] = message_byteArr_Step3[i];
	}
	//UART1_Transmit_String("Message After: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step3[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("================================================\r\n\n");

	//Print comparison of systems
	//UART1_Transmit_String("Message before decryption: ");
	
	
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_before[i] , buffer, 10);
		//UART1_Transmit_String(buffer);
		//UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("Message after decryption: ");
	//for(int i = 0; i < 8; i++)
	//{
	//	char buffer[3];
	//	itoa(message_byteArr_Step3[i] , buffer, 10);
	//	UART1_Transmit_String(buffer);
	//	UART1_Transmit_Char(' ');
	//}
	//UART1_Transmit_String("\r\n");
	//UART1_Transmit_String("************************************************************************************************\r\n");
}

//Tested - Working
void shift_Left(uint8_t inputBitArr[28], uint8_t size, uint8_t subkey_num)
{
	uint8_t temp1 = 0;
	uint8_t temp2 = 0;
	
	// Performs the circular left shift once
	if (Rotations[subkey_num] == 1)
	{
		temp1 = inputBitArr[0];		
		for(int x = 0; x < size - 1; x++)
		{
			inputBitArr[x] = inputBitArr[x + 1];
		}
		inputBitArr[size - 1] = temp1;
	}

	// Performs the circular left shift twice 
	else
	{
		temp1 = inputBitArr[0];
		temp2 = inputBitArr[1];
		for(int y = 0; y < size - 2; y++)
		{
			inputBitArr[y] = inputBitArr[y + 2];
		}
		inputBitArr[size - 2] = temp1;
		inputBitArr[size - 1] = temp2;
	}
}

//Tested - Working
void substitution_table(uint8_t input_bitArr[], uint8_t output_bitArr[], uint8_t *table, uint8_t table_size)
{	
	// Performs the reordering table
	for(uint8_t post = 0; post < table_size; post++)
	{
		uint8_t index = table[post];
		output_bitArr[post] = input_bitArr[index - 1];
	}
}

//Tested - Working
uint8_t sbox_getIndex(uint8_t x0, uint8_t y0, uint8_t y1, uint8_t y2, uint8_t y3, uint8_t x1)
{
	// Determines the sbox index to use based on the 6-bit value
	uint8_t index1 = 0;
	// X Value MSB and LSB
	if (x0 == 0 && x1 == 0)
		index1 = 0;		
	else if (x0 == 0 && x1 == 1)
		index1 = 16;
	else if (x0 == 1 && x1 == 0)
		index1 = 32;
	else
		index1 = 48;
	// Y values Middle 4 Bits
	uint8_t index2 = (y0 * 8) + (y1 * 4) + (y2 * 2) + y3;

	uint8_t index = index1 + index2;
	return index;
}

//Tested - Working
void substitution_sbox(uint8_t input_bitArr[64], uint8_t re_bitArr_SBOX[32])
{
	uint8_t sboxOutput[8];

	for(uint8_t x = 0; x < 8; x++)
	{
		//Get the values to determine the sbox index value
		uint8_t x0 = input_bitArr[6 * x];
		uint8_t y0 = input_bitArr[6 * x + 1];
		uint8_t y1 = input_bitArr[6 * x + 2];
		uint8_t y2 = input_bitArr[6 * x + 3];
		uint8_t y3 = input_bitArr[6 * x + 4];
		uint8_t x1 = input_bitArr[6 * x + 5];
		uint8_t sbox_Index = sbox_getIndex(x0, y0, y1, y2, y3, x1);
		//Perform sbox substitution
		sboxOutput[x] = sBox[x][sbox_Index];
	}

	uint8_t sbox_8bit[64];
	byte2bit_1DArr(sboxOutput, 8, sbox_8bit);

	//Concatenate final sbox array
	for(uint8_t y = 0; y < 8; y++)
	{
		re_bitArr_SBOX[4 * y] = sbox_8bit[8 * y + 4];
		re_bitArr_SBOX[4 * y + 1] = sbox_8bit[8 * y + 5];
		re_bitArr_SBOX[4 * y + 2] = sbox_8bit[8 * y + 6];
		re_bitArr_SBOX[4 * y + 3] = sbox_8bit[8 * y + 7];
	}
}

//Tested - Working
void key_schedule(uint8_t key_byteArr[8], uint8_t subkeys_byteArr[16][6])
{
	// 1. Convert to bit array
	uint8_t key_bitArr[64];
	byte2bit_1DArr(key_byteArr, 8, key_bitArr);

	// 2. PC-1 Bit select
	uint8_t PC_1_Output[PC_1_Size];
	substitution_table(key_bitArr, PC_1_Output, PC_1, PC_1_Size);

	// 3. Split 28 bits and 28 bits (56/2)
	uint8_t key_le[28];
	uint8_t key_re[28];

	for(size_t y = 0; y < 28; y++)
	{
		key_le[y] = PC_1_Output[y];
		key_re[y] = PC_1_Output[y + 28];
	}
	
	uint8_t key_combined[56];

	for(size_t subkey_num = 0; subkey_num < 16; subkey_num++)
	{
		// loop steps 4 and 5 for 16 subkeys
		// 4. Shift left
		shift_Left(key_le, 28, subkey_num);
		shift_Left(key_re, 28, subkey_num);
		
		for(size_t f = 0; f < 28; f++)
		{
			key_combined[f] = key_le[f];
			key_combined[f + 28] = key_re[f];
		}
		
		// 5. Compress with PC-2 to generate subkey
		uint8_t PC_2_Output[PC_2_Size]; 
		substitution_table(key_combined, PC_2_Output, PC_2, PC_2_Size);
		uint8_t key_byteArr_2[6];
		bit2byte_1DArr(PC_2_Output, 48, key_byteArr_2);
		
		for(size_t byte = 0; byte < 6; byte++)
		{
			subkeys_byteArr[subkey_num][byte] = key_byteArr_2[byte];
		} 
	}
}

//Tested - Working
void feistel_function(uint8_t re_byteArr[4], uint8_t subkey_byte[6], uint8_t re_byteArr_final[4])
{	
	// 1. Expansion Permutation
	uint8_t re_bitArr[32];
	byte2bit_1DArr(re_byteArr, 4, re_bitArr);

	uint8_t subkey_bitArr[48];
	byte2bit_1DArr(subkey_byte, 6, subkey_bitArr);
	
	uint8_t re_bitArr_E[E_Size];
	substitution_table(re_bitArr, re_bitArr_E, E, E_Size);
	
	// 2. Key mixing
	for(int x = 0; x < 48; x++)
	{
		re_bitArr_E[x] = re_bitArr_E[x] XOR subkey_bitArr[x];
	}

	// 3. Substitution
	// S-Box Function
	uint8_t re_bitArr_SBOX[32];
	substitution_sbox(re_bitArr_E, re_bitArr_SBOX);

	// 4. Permutation
	uint8_t re_bitArr_P[P_Size];
	substitution_table(re_bitArr_SBOX, re_bitArr_P, P, P_Size);

	bit2byte_1DArr(re_bitArr_P, P_Size, re_byteArr_final);
}

//Tested - Working
void feistel_loop(uint8_t message_byteArrInput[8], uint8_t subkeys_byteArr[16][6], uint8_t message_finalByteArr[8])
{
	//Input message
	uint8_t message_bitArr[64];
	byte2bit_1DArr(message_byteArrInput, 8, message_bitArr);
	
	//IP box
	uint8_t message_bitArr_IP[IP_Size];
	substitution_table(message_bitArr, message_bitArr_IP, IP, IP_Size);
	
	uint8_t message_byteArr[8];
	bit2byte_1DArr(message_bitArr_IP, 64, message_byteArr);

	//Split array
	uint8_t le_arr[17][4];
	uint8_t re_arr[17][4]; 
	
	for(int x = 0; x < 4; x++)
	{
		le_arr[0][x] = message_byteArr[x];
		re_arr[0][x] = message_byteArr[x + 4];
	}
	
	//Perform loop for 16 times
	uint8_t feistel_result[4];
	for(int rounds = 0; rounds < 16; rounds++)
	{
		//
		for(int byte = 0; byte < 4; byte++)
		{
			le_arr[rounds + 1][byte] = re_arr[rounds][byte];
			
		}
		
		//Subkey
		uint8_t subkey[6];
		
		//create subkey byte for use
		for(int j = 0; j < 6; j++)
		{
			subkey[j] = subkeys_byteArr[rounds][j];
		}
		
		// Create re temp to assist swap
		uint8_t re_temp[4];
		
		//Get re_arr temp
		for(int z = 0; z < 4; z++)
		{
			re_temp[z] = re_arr[rounds][z];
		}
			
		//Feistel function/round function
		feistel_function(re_temp, subkey, feistel_result);

		//Xor calculation
		for(int x = 0; x < 4; x++)
		{
			re_arr[rounds + 1][x] = feistel_result[x] XOR le_arr[rounds][x];
		}
		
		//Swap le and re
		for(int byte = 0; byte < 4; byte++)
		{
			le_arr[rounds + 1][byte] = re_arr[rounds][byte];		
		}
	}
	
	uint8_t le_temp[4];
	for(int i = 0; i < 4; i++)
	{
		le_temp[i] = le_arr[16][i];
		le_arr[16][i] = re_arr[16][i];
		re_arr[16][i] = le_temp[i];
	}
	
	// Concatenate
	uint8_t message_concat[8];
	for(int j = 0; j < 4; j++)
	{
		message_concat[j] = le_arr[16][j];
		message_concat[j+4] = re_arr[16][j];
	}
	
	//Perform final substitution table
	uint8_t message_finalBitArr[64];
	byte2bit_1DArr(message_concat, 8, message_finalBitArr);
	uint8_t message_finalBitArr_FP[64];
	substitution_table(message_finalBitArr, message_finalBitArr_FP, FP, FP_Size);
	
	bit2byte_1DArr(message_finalBitArr_FP, 64, message_finalByteArr);
}

void main_function_ECB(uint8_t key_1_plain[8], uint8_t key_2_plain[8], uint8_t key_3_plain[8], uint8_t* plaintext, uint8_t type)
{
	//Encryption
	if(type==0)
	{
		size_t num = 0;
		size_t cont=1;
		while (cont)
		{
			uint8_t msg[8];
			if(*plaintext == '\0' && num == 8)
			{
				cont=false;
				//encryption3DES(key_1_plain, key_2_plain, key_3_plain, msg);
			}
			else if(*plaintext == '\0' && num != 8)
			{
				msg[num]='0';
				plaintext = plaintext+1;
				num++;
			}
			else if(*plaintext != '\0' && num != 8)
			{
				msg[num]=*plaintext;
				num++;
			}
			else if(num==8)
			{
				//encryption3DES(key_1_plain, key_2_plain, key_3_plain, msg);
			}	
		}
	}
	//Decryption
	else
	{
		size_t num = 0;
		size_t cont=1;
		while (cont)
		{
			uint8_t msg[8];
			if(*plaintext == '\0' && num == 8)
			{
				cont=false;
				//decryption3DES(key_1_plain, key_2_plain, key_3_plain, msg);
			}
			else if(*plaintext == '\0' && num != 8)
			{
				msg[num]='0';
				plaintext = plaintext+1;
				num++;
			}
			else if(*plaintext != '\0' && num != 8)
			{
				msg[num]=*plaintext;
				num++;
			}
			else if(num==8)
			{
				//decryption3DES(key_1_plain, key_2_plain, key_3_plain, msg);
			}
		}
	}
}

void sendPin(uint8_t pin_User[8])
{
	uint8_t key_1[8];
	uint8_t key_2[8];
	uint8_t key_3[8];
	uint8_t pin[8];
	
	for(int i=0; i<8; i++)
	{
		key_1[i]=key_1_plain_pin[i];
		key_2[i]=key_2_plain_pin[i];
		key_3[i]=key_3_plain_pin[i];
		pin[i]=pin_User[i];
	}
	
	uint8_t pin_enc[8];
	encryption3DES(key_1, key_2, key_3, pin, pin_enc);
	
	char data[13];
	
	data[0]='P';
	data[1]='I';
	data[2]='N';
	data[3]=':';
	
	for(size_t y = 4; y < 12;y++)
	{
		data[y]=pin_enc[y - 4];
	}
	data[12]='\0';
	
	UART1_Transmit_String(data);
	
	//UART1_Transmit_String("\r\n");
	//Debug
	/*
	for(size_t j = 0; j < 8; j++)
	{
		char buffer[3];
		itoa(pin_enc[j] , buffer, 10);
		UART1_Transmit_String(buffer);
		UART1_Transmit_String(" ");
	}
	UART1_Transmit_String("\r\n");
	*/
	//receivePin(pin_enc);
}

void receivePin(uint8_t pin_User[8], uint8_t pin_fin[8])
{
	uint8_t key_1[8];
	uint8_t key_2[8];
	uint8_t key_3[8];
	uint8_t pin[8];
	
	for(int i=0; i<8; i++)
	{
		key_1[i]=key_1_plain_pin[i];
		key_2[i]=key_2_plain_pin[i];
		key_3[i]=key_3_plain_pin[i];
		pin[i]=pin_User[i];
	}

	uint8_t pin_dec[8];

	decryption3DES(key_1, key_2, key_3, pin, pin_dec);
	
	//Debug
	UART0_Transmit_String("\r\n");
	for(size_t j = 0; j < 8; j++)
	{
		pin_fin[j]=pin_dec[j];
		char buffer[3];
		itoa(pin_dec[j] , buffer, 10);
		UART0_Transmit_String(buffer);
		UART0_Transmit_String("\t");
	}
	UART0_Transmit_String("\r\n");
	//sendPin(pin_dec);
}

//===========================================

#endif