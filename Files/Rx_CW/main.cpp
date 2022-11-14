#include "main.hpp"
#include "AES.h"

/*
  Function: recv
  Operation: Processes the incoming radio communication.
  Inputs: MicrobitEvent - the radio input.
*/

void recv(MicroBitEvent)
{
  ManagedString s = uBit.radio.datagram.recv();
  
  //Check for lock state, reset communication variables, return to state 1.
  if(state == 2){
   char recvComm[65];
    recvCommPos = 0;
    state = 1;
  }

  //Once sender has finished salt sharing its transmits "SALT" to notify.
  if(s == "SALT"){
    
    //Perform dpk generation on the received salt with the shared pin.
    dpkgen(salt);
    state = 2;
    recvCommPos = 0;
    uBit.display.scroll("COMM");
  }
   if(s == "ENDCOM"){
    
    //Perform decryption on the command received
    aesdecrypt();
    printf("%c",cmd[0]);
    //If the received command's first character is 1(message1[0]), perform fire alarm task.
    if(cmd[0] == message1[0]){
        
        temp_alarm();    
    }
    
    //If the received command is Command2(message2), perform temperature display.
    else if(strcmp(cmd, message2) == 0){
    
        display_temp();

    }
    
    //If the received command is Command3(message3), perform compass function.
    else if(cmd[0], message3[0]){

        otp_gen();
  
    }
    else{
        
        uBit.display.scroll("ERROR");
      
    }

    //After performing command, enter lock state to avoid data collision.
    state = 2;
  }
  
   if(state == 0){
    salt[recvSaltPos] = s.charAt(0);
    uBit.display.print(salt[recvSaltPos]);
    recvSaltPos++;
  }
  //State 1 = Communicating, characters are received and stored.
  if(state == 1){
    recvComm[recvCommPos] = s.charAt(0);
    uBit.display.print(recvComm[recvCommPos]);
    recvCommPos++;
  }
}

/*
  Function: main
  Operation: Primary runtime. Waits till Button A is pressed.
             If pressed, it moves to receiver mode.
*/

int main(){
    uBit.init();

    int flag=0;
    uBit.display.scroll("RECEIVER");
    uBit.display.scroll("<-");
    while(flag == 0){
        if(uBit.buttonA.isPressed()){
              receiver();
              flag = 1;
        }
    }
    //Clear uBit threads.
    release_fiber();
}

/*
  Function: recieveMode
  Operation: Handles the recieve radio functionality.
*/

int receiver(){
    
    //enable Microbit radio reciever, designate processing function.
    uBit.messageBus.listen(MICROBIT_ID_RADIO,MICROBIT_RADIO_EVT_DATAGRAM, recv);
    uBit.radio.enable();
    
  //await recieve.
  while(true){
    uBit.sleep(1000);
  }
}

/*
  Function: dpkgen
  Operation: Handles the construction of the DPK. Merges the pin and salt values
             and the hashes them into SHA256.
  Input: char *salt - pointer to variable containing the salt.
*/

int dpkgen(char *salt){
 
  //Providing the hard coded value for the pin which is shared by sender.
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
  Function: aesdecrypt
  Operation: Decryptes the command using the generated dpk and iv in AES_CBC mode.
*/

int aesdecrypt()
{
    AES aes;
    
    int j=0;
    
    //Storing the received encrypted command in cmd variable and then decrypting it.
    for(int i=0;i<sizeof(recvComm);i++)
    {
        cmd[j]=recvComm[i];
        j++;
    }
    aes.setup(dpk, AES::KEY_256, AES::MODE_CBC, iv);
    aes.decrypt(cmd, sizeof(cmd));
    aes.clear();
}

/*
  Function: temp_alarm
  Operation: Displays Temperature and if the temperature exceeds the threshold,
             sends analog signals to pin P0 which causes Piezzo Buzzer to produce sound.
*/

int temp_alarm()
{
    //Initialize variables
    int flg=0;
    char tmp_cmd[3];
    int temp_val;
    
    //Convert the temperature received from char to int.
    tmp_cmd[0]=cmd[2];
    tmp_cmd[1]=cmd[3];
    sscanf(tmp_cmd,"%d", &temp_val);
    
    //Initialize P0 pin.
    MicroBitPin P0(MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_ALL);
    while(flg==0)
    {
        //Exit when Button A is pressed
        if(uBit.buttonA.isPressed()){
           uBit.io.P0.setDigitalValue(0);
           flg=1;
        }
        //Fire Alarm operation when Button B is pressed.
        else if (uBit.buttonB.isPressed()){
            uBit.display.scroll("TEMP=");
            uBit.display.scroll(temp_val);
            
            //PWM signal is sent in P0 pin when temp exceeds threshold.
            if(temp_val > thresh){
                uBit.io.P0.setServoValue(180);
            }
            else{
                uBit.io.P0.setDigitalValue(0);
            }
        }
    }
    uBit.display.scroll("EXIT");
}

/*
  Function: display_temp
  Operation: Displays Temperature both in Celcius and Fahrenheit.
*/

void display_temp()
{
    int temp_F;
    int flg=0;
    int temp_C = uBit.thermometer.getTemperature();
    
    //Celcius to Fahrenheit conversion
    temp_F = (temp_C * 9/5) + 32;
    
    while(flg==0)
    {
        uBit.display.scroll("TEMP in C=");
        uBit.display.scroll(temp_C);
        uBit.display.scroll("TEMP in F=");
        uBit.display.scroll(temp_F);
        
        //Exit when Button A is pressed. 
        for(int i=0;i<10000;i++)
        {
            if(uBit.buttonA.isPressed()){
                flg=1;
            }
        }
    }   
    uBit.display.scroll("EXIT");
}

/*
  Function: compass
  Operation: Toggle compass functionality.
*/

void otp_gen(){
  
  int j,random=0;
  uBit.display.scroll("OTP-");
  for(j=0;j<4;j++)                                        // Loop for generating four digit otp using rand()
    {
            random=(int) (10.0*rand()/(RAND_MAX + 1.0));
            uBit.display.scroll(random);
           
    }
}



