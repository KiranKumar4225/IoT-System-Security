#include "MicroBit.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define SALTSIZE 128
#define KEYSIZE 256

//Prototyping
MicroBit uBit;
int randsalt(char *salt, int length);
int dpkgen(char *salt);
int sender();
int aesencrypt();
int cmd1();
void command(int comm);

const int saltArraySize = SALTSIZE / 4;
const int pinSaltArraySize = saltArraySize + 4;

//Global Variables
int state=0;
char salt[saltArraySize];
char dpk[32];
char pinSalt[pinSaltArraySize];
const char iv[16] = {
     0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
     0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
 };
 char message1[] = { "1_xx" };
 
 char message2[] = {
     "Command2"
 };
 char message3[] = {
     "Command3"
 };
 
//Variables for Command encryption 
 char ciphertxt1[32];
 char ciphertxt2[32];
 char ciphertxt3[32];

//external SHA256 hashing function.
extern void sha256(const void *data, size_t len, char *sha2digest);

