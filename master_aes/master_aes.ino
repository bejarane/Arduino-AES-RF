#include <RH_ASK.h>
#include <AES.h>
#include "./printf.h"

//radiohead has a limit to send of 60 characters

AES aes;
byte *key = (unsigned char*)"0123456789010123"; //this key must be unique and secretly shared
unsigned long long int my_iv = 36753562;//this initialization vector can be const, but the same between devices

RH_ASK driver; //initializing an instance of RF driver, both RX and TX being RX on pin 11 and TX on pin 12

byte max_payload=0; //here is saved the max payload for comparison

void setup()
{
  Serial.begin(9600);   // Debugging only
  if (!driver.init()){
    Serial.println(F("init failed"));
  }

  printf_begin();
  Serial.print(F("RHlibrary maxpayload:"));
  Serial.println(driver.maxMessageLength());
  Serial.print(F("CurrentAESblocksize:"));
  Serial.println(N_BLOCK);
  Serial.print(F("MaxBlocks:"));
  Serial.println(driver.maxMessageLength()/N_BLOCK);
  Serial.print(F("MaxChar:"));
  max_payload = (driver.maxMessageLength()/N_BLOCK)*N_BLOCK;
  Serial.println(max_payload);
  
  
}

void loop()
{ 
  byte data[60] = ""; //variable for the data to send
  byte message[] = "This string was decoded Cant be bigger than 46";//the string we are going to send
  strcpy(data,message); //just a string copy
  byte data_l = strlen(data)+1; //the +1 is for saving the length of null terminator in the string
  Serial.print(F("Payload:"));
  Serial.println(data_l);
  
  if (data_l<max_payload){
    send_encrypted(data,128,data_l);
  }else{
    Serial.println(F("Exceeded RHlibrary maxpayload"));
  }  
  delay(2000); 
}

void send_encrypted(char* data,byte bits, byte length){
  byte padedLength = length + N_BLOCK - length % N_BLOCK;
  aes.iv_inc();
  byte iv [N_BLOCK] ;
  byte plain_p[padedLength];
  byte cipher [padedLength] ;
  byte check [padedLength] ;
  unsigned long ms = micros ();
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_encrypt(data,length,cipher,key,bits,iv);

//this is for check encrypting and decrypting
  aes.set_IV(my_iv);
  aes.get_IV(iv);
  aes.do_aes_decrypt(cipher,padedLength,check,key,bits,iv);
  printf("\nCIPHER:");
  aes.printArray(cipher,(bool)false);

//sending the ciphered to RF
  
  driver.send((uint8_t *)cipher,sizeof(cipher));
  driver.waitPacketSent();
  Serial.print("Size of var:");
  Serial.print(padedLength);
  Serial.print("Size of send");
  Serial.print(sizeof(cipher));
  Serial.print("Msg:");
  
//printing the ciphered result
  char result;
  #define printable check
  for (byte i = 0; i < sizeof(printable); ++i){
    //sprintf(result,"%c",printable[i]);
    result = printable[i];
    Serial.print(result);
    }
  Serial.println("===endof");
}
