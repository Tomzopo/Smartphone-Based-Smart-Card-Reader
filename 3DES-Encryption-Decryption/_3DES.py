"""
3DES Encryption
Author: Thomas Stewart
"""

from copy import deepcopy

import numpy as np

import boxes as bx


def bit2byte_1DArr(bitArr):
    byteArr = np.packbits(bitArr)
    return byteArr

def byte2bit_1DArr(byteArr):
    bitArr = np.fromstring(byteArr, dtype=np.uint8)
    bitArr = np.unpackbits(bitArr)
    return bitArr


def key_AddPad(key_plain):
    while len(key_plain) != 8:
        key_plain = key_plain + '0'
    return key_plain


def plaintext_AddPad(message_plain):
    while len(message_plain) % 8 != 0:
        message_plain = message_plain + '0'
    return message_plain


# Global Variables
# ============================================================
# key_1_plain = "01234567"
# key_2_plain = "abcdefgh"
# key_3_plain = "ZYXWVUTS"
# plaintext = "ABCDEFGH"

key_1_plain = "zxcvbnm,"
key_2_plain = "abcdefgh"
key_3_plain = "ZYXWVUTS"
plaintext = "GreetingGreetingGreetingGreeting"

key_1_plain = key_AddPad(key_1_plain)
key_2_plain = key_AddPad(key_2_plain)
key_3_plain = key_AddPad(key_3_plain)
plaintext = plaintext_AddPad(plaintext)

key_1_byteArr = np.fromstring(key_1_plain, dtype=np.uint8)
key_2_byteArr = np.fromstring(key_2_plain, dtype=np.uint8)
key_3_byteArr = np.fromstring(key_3_plain, dtype=np.uint8)
plain_byteArr = np.fromstring(plaintext, dtype=np.uint8)


# ============================================================

def encryptionDES(key_byteArr, message_byteArr):
    # Testing Information Line - Remove Later
    message_before = deepcopy(message_byteArr)
    print("\nEncryption Process")
    print("************************************************************************************************")
    subkeys_byteArr1 = key_schedule(key_byteArr)
    message_byteArr_Encrypted = feistel_loop(message_byteArr, subkeys_byteArr1)
    print("Message before encryption: " + str(message_before))
    print("Message after encryption: " + str(message_byteArr_Encrypted))
    print("************************************************************************************************\n\n")
    return message_byteArr_Encrypted


def decryptionDES(key1_byteArr, message_byteArr):
    # Testing Information Line - Remove Later
    message_before = deepcopy(message_byteArr)
    print("\nDecryption Process")
    print("************************************************************************************************")
    subkeys_byteArr1 = np.flip(key_schedule(key1_byteArr), axis=0)
    message_byteArr_Decrypted = feistel_loop(message_byteArr, subkeys_byteArr1)
    print("Message before decryption: " + str(message_before))
    print("Message after decryption: " + str(message_byteArr_Decrypted))
    print("************************************************************************************************\n\n")
    return message_byteArr_Decrypted


def encryption3DES(key1_byteArr, key2_byteArr, key3_byteArr, message_byteArr):
    # Testing Information Line - Remove Later
    message_before = deepcopy(message_byteArr)
    print("\nEncryption Process")
    print("************************************************************************************************")
    print("Encryption Step 1")
    print("================================================")
    subkeys_byteArr1 = key_schedule(key1_byteArr)
    # print(subkeys_byteArr1)
    print("Message Before: " + str(message_byteArr))
    message_byteArr_Step1 = feistel_loop(message_byteArr, subkeys_byteArr1)
    print("Message After: " + str(message_byteArr_Step1))
    print("================================================\n")

    print("Encryption Step 2")
    print("================================================")
    subkeys_byteArr2 = np.flip(key_schedule(key2_byteArr), axis=0)
    # print(subkeys_byteArr2)
    print("Message Before: " + str(message_byteArr_Step1))
    message_byteArr_Step2 = feistel_loop(message_byteArr_Step1, subkeys_byteArr2)
    print("Message After: " + str(message_byteArr_Step2))
    print("================================================\n")

    print("Encryption Step 3")
    print("================================================")
    subkeys_byteArr3 = key_schedule(key3_byteArr)
    # print(subkeys_byteArr3)
    print("Message Before: " + str(message_byteArr_Step2))
    message_byteArr_Step3 = feistel_loop(message_byteArr_Step2, subkeys_byteArr3)
    print("Message After: " + str(message_byteArr_Step3))
    print("================================================\n")

    print("Message before encryption: " + str(message_before))
    print("Message after encryption: " + str(message_byteArr_Step3))
    print("************************************************************************************************\n\n")
    return message_byteArr_Step3


def decryption3DES(key1_byteArr, key2_byteArr, key3_byteArr, message_byteArr):
    # Testing Information Line - Remove Later
    message_before = message_byteArr

    print("\nDecryption Process")
    print("************************************************************************************************")
    print("Decryption Step 1")
    print("================================================")
    subkeys_byteArr1 = np.flip(key_schedule(key3_byteArr), axis=0)
    # print(subkeys_byteArr1)
    # print(subkeys_byteArr1)
    print("Message Before: " + str(message_byteArr))
    message_byteArr_Step1 = feistel_loop(message_byteArr, subkeys_byteArr1)
    print("Message After: " + str(message_byteArr_Step1))
    print("================================================\n")

    print("Decryption Step 2")
    print("================================================")
    subkeys_byteArr2 = key_schedule(key2_byteArr)
    # print(subkeys_byteArr2)
    print("Message Before: " + str(message_byteArr_Step1))
    message_byteArr_Step2 = feistel_loop(message_byteArr_Step1, subkeys_byteArr2)
    print("Message After: " + str(message_byteArr_Step2))
    print("================================================\n")

    print("Decryption Step 3")
    print("================================================")
    subkeys_byteArr3 = np.flip(key_schedule(key1_byteArr), axis=0)
    # print(subkeys_byteArr3)
    print("Message Before: " + str(message_byteArr_Step2))
    message_byteArr_Step3 = feistel_loop(message_byteArr_Step2, subkeys_byteArr3)
    print("Message After: " + str(message_byteArr_Step3))
    print("================================================\n")

    print("Message before decryption: " + str(message_before))
    print("Message after decryption: " + str(message_byteArr_Step3))
    print("************************************************************************************************\n\n")
    return message_byteArr_Step3


def shift_Left(inputBitArr, subkey_num):
    outputBitArr = np.zeros(28, dtype=np.uint8)
    if bx.Rotations[subkey_num] == 1:
        for x in range(0, 27):
            outputBitArr[x] = inputBitArr[x + 1]
        outputBitArr[27] = inputBitArr[0]

    else:
        for y in range(0, 26):
            outputBitArr[y] = inputBitArr[y + 2]
        outputBitArr[26] = inputBitArr[0]
        outputBitArr[27] = inputBitArr[1]
    return outputBitArr


def reordering_table(input_bitArr, table):
    output_bitArr = np.zeros(len(table), dtype=np.uint8)
    for post in range(0, len(table)):
        index = table[post]
        output_bitArr[post] = input_bitArr[index - 1]
    return output_bitArr


def sbox_getIndex(x0, y0, y1, y2, y3, x1):
    # X Value MSB and LSB
    if (x0 == 0 and x1 == 0):
        index1 = 0

    elif (x0 == 0 and x1 == 1):
        index1 = 16

    elif (x0 == 1 and x1 == 0):
        index1 = 32

    else:
        index1 = 48

    # Y values Middle 4 Bits
    index2 = (y0 * (2 ** 3)) + (y1 * (2 ** 2)) + (y2 * (2 ** 1)) + (y3 * (2 ** 0))

    index = index1 + index2
    return index


def substitution_sbox(input_bitArr):
    sboxOutput = np.zeros(8, dtype=np.uint8)

    for x in range(0, 8):
        x0 = input_bitArr[6 * x]
        y0 = input_bitArr[6 * x + 1]
        y1 = input_bitArr[6 * x + 2]
        y2 = input_bitArr[6 * x + 3]
        y3 = input_bitArr[6 * x + 4]
        x1 = input_bitArr[6 * x + 5]
        sbox_Index = sbox_getIndex(x0, y0, y1, y2, y3, x1)
        sboxOutput[x] = bx.sBox[x][sbox_Index]

    sbox_8bit = byte2bit_1DArr(sboxOutput)
    output_bitArr = np.zeros(32, dtype=np.uint8)
    for y in range(0, 8):
        output_bitArr[4 * y] = sbox_8bit[8 * y + 4]
        output_bitArr[4 * y + 1] = sbox_8bit[8 * y + 5]
        output_bitArr[4 * y + 2] = sbox_8bit[8 * y + 6]
        output_bitArr[4 * y + 3] = sbox_8bit[8 * y + 7]

    return output_bitArr


def key_schedule(key_byteArr):
    # 1. Convert to bit array
    key_bitArr = byte2bit_1DArr(key_byteArr)
    # 2. PC-1 Bit select
    PC_1_Output = reordering_table(key_bitArr, bx.PC_1)

    # 3. Split 28 bits and 28 bits (56/2)
    key_le = np.zeros(28, dtype=np.uint8)
    key_re = np.zeros(28, dtype=np.uint8)
    for y in range(0, 28):
        key_le[y] = PC_1_Output[y]
        key_re[y] = PC_1_Output[y + 28]

    subkey_bits = np.zeros(48, dtype=np.uint8)
    subkeys_byteArr = np.zeros((16, 6), dtype=np.uint8)
    key_combined = np.zeros(56, dtype=np.uint8)

    for subkey_num in range(0, 16):
        # loop steps 4 and 5 for 16 subkeys
        # 4. Shift leftc

        key_le = shift_Left(key_le, subkey_num)
        key_re = shift_Left(key_re, subkey_num)

        for f in range(0, 28):
            key_combined[f] = key_le[f]
            key_combined[f + 28] = key_re[f]
            # 5. Compress with PC-2 to generate subkey
        PC_2_Output = reordering_table(key_combined, bx.PC_2)
        subkeys_byteArr[subkey_num] = bit2byte_1DArr(PC_2_Output)

    return subkeys_byteArr


def round_function(re_byteArr, subkey_byte):
    # 1. Expansion Permutation
    re_bitArr = byte2bit_1DArr(re_byteArr)
    subkey_bitArr = byte2bit_1DArr(subkey_byte)
    re_bitArr_expanded = reordering_table(re_bitArr, bx.E)
    # 2. Key mixing
    for x in range(0, 48):
        re_bitArr_expanded[x] = re_bitArr_expanded[x] ^ subkey_bitArr[x]

    # 3. Substitution
    # S-Box Function
    re_bitArr_SBOX = substitution_sbox(re_bitArr_expanded)

    # 4. Permutation
    re_bitArr_final = reordering_table(re_bitArr_SBOX, bx.P)
    return bit2byte_1DArr(re_bitArr_final)


def feistel_loop(message_byteArrInput, subkeys_byteArr):
    message_bitArr = byte2bit_1DArr(message_byteArrInput)
    message_bitArr = reordering_table(message_bitArr, bx.IP)

    message_byteArr = bit2byte_1DArr(message_bitArr)

    le_arr = np.zeros((17, 4), dtype=np.uint8)
    re_arr = np.zeros((17, 4), dtype=np.uint8)

    for x in range(0, 4):
        le_arr[0][x] = message_byteArr[x]
        re_arr[0][x] = message_byteArr[x + 4]

    feistel_result = np.zeros(4, dtype=np.uint8)

    for rounds in range(0, 16):
        le_arr[rounds + 1] = deepcopy(re_arr[rounds])
        feistel_result = round_function(re_arr[rounds], subkeys_byteArr[rounds])

        for x in range(0, 4):
            re_arr[rounds + 1][x] = feistel_result[x] ^ le_arr[rounds][x]
        le_arr[rounds + 1] = deepcopy(re_arr[rounds])

    le_temp = deepcopy(le_arr)
    le_arr = deepcopy(re_arr)
    re_arr = le_temp

    message_finalByteArr = np.concatenate((le_arr[16], re_arr[16]), axis=0)

    message_finalBitArr = byte2bit_1DArr(message_finalByteArr)
    message_finalBitArr = reordering_table(message_finalBitArr, bx.FP)
    message_finalByteArr = bit2byte_1DArr(message_finalBitArr)

    return message_finalByteArr


def main_function_ECB(key1_byteArr, key2_byteArr, key3_byteArr, message_byteArr):
    # message_byteArr_Enc = np.zeros(8, dtype=np.uint8)
    # message_byteArr_Dec = np.zeros(8, dtype=np.uint8)
    curr_message = np.zeros(8, dtype=np.uint8)

    message_len = int(len(message_byteArr) / 8)

    for x in range(message_len):
        for y in range(0, 8):
            curr_message[y] = message_byteArr[8 * x + y]
        curr_message_byteArr_Enc = encryption3DES(key1_byteArr, key2_byteArr, key3_byteArr, curr_message)
        if x == 0:
            message_byteArr_Enc = deepcopy(curr_message_byteArr_Enc)
        else:
            message_byteArr_Enc = np.concatenate((message_byteArr_Enc, curr_message_byteArr_Enc), axis=0)
        message_len = message_len - 1

    enc_message_len = int(len(message_byteArr_Enc) / 8)

    for x in range(enc_message_len):
        for y in range(0, 8):
            curr_message[y] = message_byteArr_Enc[8 * x + y]
        curr_message_byteArr_Dec = decryption3DES(key1_byteArr, key2_byteArr, key3_byteArr, curr_message)
        if x == 0:
            message_byteArr_Dec = curr_message_byteArr_Dec
        else:
            message_byteArr_Dec = np.concatenate((message_byteArr_Dec, curr_message_byteArr_Dec), axis=0)
        enc_message_len = enc_message_len - 1
    # message_byteArr_Enc = encryptionDES(key1_byteArr, message_byteArr)
    # message_byteArr_Dec = decryptionDES(key1_byteArr, message_byteArr_Enc)

    print("\n\nEncryption -> Decryption")
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
    if (message_byteArr_Dec == message_byteArr).all():
        print("Plain: " + str(message_byteArr))
        print("|")
        print("V")
        print("Encry: " + str(message_byteArr_Enc))
        print("|")
        print("V")
        print("Decry: " + str(message_byteArr_Dec))
        print("Working!!!")
    else:
        print("BROKEN")
    print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")


def ValidationTest():
    x0 = np.zeros(8, dtype=np.uint8)
    x16 = np.zeros(8, dtype=np.uint8)

    x0[0] = 0x94
    x0[1] = 0x74
    x0[2] = 0xb8
    x0[3] = 0xe8
    x0[4] = 0xc7
    x0[5] = 0x3b
    x0[6] = 0xca
    x0[7] = 0x7d
    print(x0)

    x16[0] = 0x1b
    x16[1] = 0x1a
    x16[2] = 0x2d
    x16[3] = 0xdb
    x16[4] = 0x4c
    x16[5] = 0x64
    x16[6] = 0x24
    x16[7] = 0x38
    print(x16)

    x = deepcopy(x0)
    for i in range(16):
        pin = deepcopy(x)
        if i % 2 == 0:
            x = encryptionDES(pin, x)  # if i is even, x[i+1] = E(x[i], x[i])
        else:
            x = decryptionDES(pin, x)  # if i is odd, x[i+1] = D(x[i], x[i])

    print(str(x0))
    print(str(x))
    print(str(x16))


ValidationTest()

# main_function_ECB(key_1_byteArr, key_2_byteArr, key_3_byteArr, plain_byteArr)

# encryption3DES(key_1_byteArr, key_2_byteArr, key_3_byteArr, plain_byteArr)
# decryption3DES(key_1_byteArr, key_2_byteArr, key_3_byteArr, plain_byteArr)
