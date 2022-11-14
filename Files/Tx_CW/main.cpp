#include "main.hpp"
#include "AES.h"


/*
  Function: main
  Operation: Primary runtime. Waits till Button A is pressed.
             If pressed, it moves to sender mode.
*/
int main(){
    uBit.init();

    int flag=0;
    uBit.display.scroll("SENDER");
    uBit.display.scroll("<-");
    while(flag == 0){
        if(uBit.buttonA.isPressed()){
              sender();
              flag = 1;
        }
    }
    //Clear uBit threads.
    release_fiber();
}
/*
  Function: sendMode
  Operation: Controls the sender functions for the system.
*/
int sender(){
  
  //DPK generation calls.
  randsalt(salt, 128);
  dpkgen(salt);
  
  //AES Encryption call.
  aesencrypt();
  
  //enable Microbit radio
  uBit.radio.enable();
  ManagedString s;
  uBit.display.scroll("<-");
  
  //Perform Salt sharing
  while(state == 0){
    if(uBit.buttonA.isPressed()){
      for(int i = 0; i < SALTSIZE / 4; i++){
          s = salt[i];
          uBit.radio.datagram.send(s);
          uBit.display.print(s);
          uBit.sleep(100);
      }
      //signal end of Salt sharing, transition to Command sharing.
      uBit.radio.datagram.send("SALT");
      state = 1;
    }
  }
  //Sleep for soft barrier, signal Communication between microbits.
  uBit.sleep(100);
  uBit.display.scroll("COMM");
  int comm = 1;

  //Perform Command sharing
  while(state == 1){
    uBit.display.print(comm);
    if(uBit.buttonA.isPressed()){
      command(comm);
      uBit.sleep(300);
    } else if(uBit.buttonB.isPressed()){
      if(comm == 3){
        comm = 1;
        uBit.sleep(300);
      }else{
        comm++;
        uBit.sleep(300);
      }
    }
  }
  return 0;
}

/*
  Function: sendCommand
  Operation: External command passing. Receives interger from sender() and 
             sends the respective encrypted commands to receiver.
*/
void command(int comm)
{
  ManagedString s;

  for(int i = 0; i < 32; i++){
    switch(comm){
      case 1:
        s = ciphertxt1[i];
        break;
      case 2:
        s = ciphertxt2[i];
        break;
      case 3:
        s = ciphertxt3[i];
        break;
    }
      uBit.radio.datagram.send(s);
      uBit.display.print(s);
      uBit.sleep(100);
    }
    //Signals receiver end of command sharing.
    uBit.radio.datagram.send("ENDCOM");
      
}

/*
  Function: randsalt
  Operation: generates a variable length salt.
  Input: char *salt - pointer to variable where the salt should be stored.
         int length - length of the desired salt.
*/

int randsalt(char *salt, int length){
  
  //Initialize the dictionary for generating the salt.
  char *dict_salt = "ABCDEFGHIJKLMNOPQRSTUVWYXZabcdefghijklmnopqrstuvwyxz0123456789";
  
  //Generate random number between 0 and 62, assign character based on output.                        
  for (int i = 0; i < length / 4; i = i + 1){
    salt[i] = dict_salt[(int)(62.0 * rand()/(RAND_MAX + 1.0))];
  }
  return 0;
}

/*
  Function: dpkgen
  Operation: Handles the construction of the DPK. Merges the pin and salt values
             and the hashes them into SHA256.
  Input: char *salt - pointer to variable containing the salt.
*/

int dpkgen(char *salt){

  //Providing the hard coded value for the pin.
  pinSalt[0] = 1;
  pinSalt[1] = 2;
  pinSalt[2] = 3;
  pinSalt[3] = 4;

  //Combine salt and pin.
  int j = 0;
  for(int i = 4; i <= SALTSIZE / 4; i++){
    pinSalt[i] = salt[j];
    j++;
  }
  
  //Hashing Pin+Salt using sha256.
  sha256(pinSalt, sizeof(pinSalt), dpk);

  return 0;
}

/*
  Function: aesencrypt
  Operation: Encryptes the command using dpk and iv in AES_CBC mode.
*/

int aesencrypt()
{
    
    AES aes;
    
    // Encryption for Command 1
    int j=0;
    
    //cmd1 function call.
    cmd1();
    
    for(int i=0;i<sizeof(message1);i++)
    {
        ciphertxt1[j]=message1[i];
        j++;
    }
    aes.setup(dpk, AES::KEY_256, AES::MODE_CBC, iv);
    aes.encrypt(ciphertxt1, sizeof(ciphertxt1));
    aes.clear(); 

    // Encryption for Command 2
    j=0;
    for(int i=0;i<sizeof(message2);i++)
    {
        ciphertxt2[j]=message2[i];
        j++;
    }
    aes.setup(dpk, AES::KEY_256, AES::MODE_CBC, iv);
    aes.encrypt(ciphertxt2, sizeof(ciphertxt2));
    aes.clear(); 
    
    // Encryption for Command 3
    j=0;
    for(int i=0;i<sizeof(message3);i++)
    {
        ciphertxt3[j]=message3[i];
        j++;
    }
    aes.setup(dpk, AES::KEY_256, AES::MODE_CBC, iv);
    aes.encrypt(ciphertxt3, sizeof(ciphertxt3));
    aes.clear(); 
    
}

/*
  Function: cmd1
  Operation: Convert temperature from integer to char and store it in 
             message1 variable which is to be transmitted.
*/

int cmd1()
{
    char tmp[3];
    int temp_C = uBit.thermometer.getTemperature();
    
    //Int to char 
    snprintf(tmp, 10, "%d", temp_C);     
    
    //Format of message1 - 1_xx where xx is the temperature read from sender.
    message1[2]=tmp[0];
    message1[3]=tmp[1];

}
