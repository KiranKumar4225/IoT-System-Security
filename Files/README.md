# Lightweight Authentication System for Micro:Bit IoT Device

Module: IoT Systems Security 
Module code: UFCF8P-15-M

## Project Outline

The aim is to implement and demonstrate a lightweight IoT device authentication system using a pair of Micro:bits. This Project uses SHA256 Hashing algorithm and AES-256 encryption in CBC mode.

#### Command

1. Command 1 - Format of message : "1_xx"(where xx is the temperature of sender)
 The first command is Fire Alarm. The receiver microbit produces an alarming sound when the sender microbit's temperature exceeds the threshold value. The sender's temperature is stored in message1 variable and encrypted and sent to the receiver. The receiver decrypts the message1 and checks whether the value is higher than the threshold. If it is, the microbit sends analog output to the P0 pin which is connected to the external Piezo Buzzer. Thus producing a alarm. 

2. Command 2 - Format of message : "Command2"
 The second command is displaying Temperature. The message2 variable is initialized to "Command2" both in sender and receiver. When the receiver receives the encrypted message, it decrypts it and compares it with the message2 variable. If that matches "Command2", the temperature of the receiver is displayed in Celcius and Fahrenheit.

3. Command 3 - Format of message : "Command3"
 The third command is Compass.  Similar to the second command, message3 variable is initialized to "Command3". Tf the receiver matches the decrypted message and message3, a 4-digit OTP is generated and displayed in receiver microbit. 

#### DPK Generation
In Sender Microbit, 128-bit random salt was generated using the rand() function. The dictionary for the salt is "A-Z","a-z" and "0-9". The generated salt is then concatenated with the pin. The Pin is common for sender and receiver and should be shared before the communication(In this project, the Pin is hard-coded as 1234). The Pin+Salt array is hashed using SHA256 algorithm, thus producing 32 byte DPK.

In Receiver Microbit, the received salt is concatenated with the shared pin and DPK is generated using the same SHA256 algorithm.

#### Encrytion
The AES encryption requires Key and IV for encryption. The generated DPK is used as a key and a 16-byte hex value, defined globally is used as IV. The encryption is operated in CBC mode. All commands are encrypted and stored in their respective variables. When the user selects a command, its respective cipher text is sent to the receiver.

#### Decryption
Similar to encryption, the receiver uses the generated DPK and IV for decrypting the cipher text in CBC mode. The decrypted command is compared with all the command variables. If a match is found, the command is executed.

## Flashing

1. Import the Tx_CW and Rx_CW folder in the online Mbed Compiler.
2. Compile the main.cpp file present in both the program.
3. Once the compilation is done, a Hex file will be downloaded.
4. Connect the two microbits to your PC.
5. Copy the sender's Hex file to one microbit and the receiver's Hex file to another. 

## Run-Time

1. Both the sender and receiver Microbits signals to press Button A to switch to sender and receiver mode respectively.
2. To initiate salt sharing, press Button A in sender Microbit. The "COMM" will scroll in both the microbits once the salt sharing has finished.
3. There are three commands available to choose in sender. Press Button B for switching between commands. Press Button A for choosing the command.



