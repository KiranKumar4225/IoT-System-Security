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
int receiver();
void recv();
int aesdecrypt();

const int saltArraySize = SALTSIZE / 4;
const int pinSaltArraySize = saltArraySize + 4;

//Global Variables
int state = 0;
char salt[saltArraySize];
int recvSaltPos = 0;
int recvCommPos = 0;
char dpk[32];
char pinSalt[pinSaltArraySize];
char recvComm[32];
char cmd[32];
int thresh = 23;
const char iv[16] = {
     0x74, 0x11, 0xF0, 0x45, 0xD6, 0xA4, 0x3F, 0x69,
     0x18, 0xC6, 0x75, 0x42, 0xDF, 0x4C, 0xA7, 0x84
 };
 char message1[] = {
     "1_xx"
 };
 char message2[] = {
     "Command2"
 };
 char message3[] = {
     "Command3"
 };

//Command method prototyping. 
int temp_alarm();
void display_temp();
void otp_gen();


//external SHA256 hashing function.
extern void sha256(const void *data, size_t len, char *sha2digest);

