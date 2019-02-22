#include <RH_ASK.h>
#include <SPI.h> // Not actualy used but needed to compile
#include <AES.h>

AES aes;
byte *key = (unsigned char*)"0123456789010123"; //this key must be unique and secretly shared
unsigned long long int my_iv = 36753562; //this initialization vector can be const, but the same between devices

RH_ASK driver; //instance for the RF driver object
const byte coded_max_payload = 60; //this is the expected maximum payload for rf channel
const byte coded_max_char = coded_max_payload / N_BLOCK * N_BLOCK; //this are the expected maximum of chars encrypted;

void setup()
{
    Serial.begin(9600); // Debugging only
    if (!driver.init())
         Serial.println("init failed");
    delay(500);
    byte max_payload = 0;
    Serial.print(F("RHlibrary maxpayload:"));
    Serial.println(driver.maxMessageLength());
    Serial.print(F("Coded RHlibrary maxpayload:"));
    Serial.println(coded_max_payload);
    Serial.print(F("CurrentAESblocksize:"));
    Serial.println(N_BLOCK);
    Serial.print(F("MaxBlocks:"));
    Serial.println(driver.maxMessageLength()/N_BLOCK);
    Serial.print(F("MaxChar:"));
    max_payload = (driver.maxMessageLength()/N_BLOCK)*N_BLOCK;
    Serial.println(max_payload);
    Serial.print(F("CodedMaxChar:"));
    Serial.println(coded_max_char);
}

void loop()
{
    uint8_t buf[coded_max_payload]=""; //it is equal to null for cleaning the spaced used for variable
    uint8_t buflen = sizeof(buf); //just saving its size
    if (driver.recv(buf, &buflen)) // Non-blocking
    {
      
      aes_decrypt(buf,128,buf); //here im rewriting the entry string, but not sure if it is a good idea. Actually works fine

      Serial.print(F("Message decoded:")); //just some printing
      char buffer; //this buffer is for reading the char string
      #define printable buf //for fast changing
      int count = 0;
      for (byte i = 0; i < sizeof(printable); ++i){
        buffer = printable[i];
        if (buffer != 0){//Stop in null character for char array
          Serial.print(buffer); //print char, could be saved too
          //Serial.println(buffer,HEX);
          ++ count;
        }else{
          break;
        }
        
       }

       
      Serial.print(F("===endofmessage"));//just some printing
      Serial.println(count);
      Serial.println();
    }   
}

/*
 * Here data is the encrypted data received, bits is just the expected size of AES, result is a pointer to the
 * string where the decrypted data should be saved
 */
void aes_decrypt(char* data, byte bits, byte* result) //fucntion for decryption
{
  int plainLength = coded_max_char; //set message size
  int padedLength = plainLength + N_BLOCK - plainLength % N_BLOCK; //the normal padded calculus
  aes.iv_inc(); //start of aes?
  byte iv [N_BLOCK] = ""; //clean array to avoid old variables contamination (sometimes, the reasigned pointer points to the same 
  //place is stack, so is like recylcing variables, thats why it is declared and initialized with empty spaces)
  
  Serial.print(F("MaxSizevar:"));
  Serial.print(padedLength);
  Serial.print(F("MaxSizemsg:"));
  Serial.print(plainLength);
  Serial.print(F(" Msg:"));

  char buffer; //this buffer is for reading the char string
  #define printable data //for fast changing
  for (byte i = 0; i < padedLength; ++i){
    buffer = printable[i];
    Serial.print(buffer); //print char, could be saved too
    //Serial.println(buffer,HEX);  
   }
   Serial.println();
  
  aes.set_IV(my_iv); //setting initial vector
  aes.get_IV(iv); //getting it
  aes.do_aes_decrypt(data,padedLength,result,key,bits,iv);//decrypting
}
