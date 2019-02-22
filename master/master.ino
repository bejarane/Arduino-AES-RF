#include <RH_ASK.h>

RH_ASK driver; //esto es iniciar una instancia de RH_ASK con el TX en pin 12
//y el RX en pin 11. Por tanto, ya inicia las dos requeridas

char buffer[20];//el char es solo un buffer para meter y sacar el string
int idx = 0;//este int se usa para tener una variable para mandar al equipo
int last_idx = 0; //variable donde se almacena 

void setup()
{
  //el setup es el normal
  Serial.begin(9600);   // Debugging only
  if (!driver.init())
    Serial.println("init failed");
}

void loop()
{
  //la idea es hacer lo siguiente:
  //1. Enviar un numero X
  //2. Esperar a que reciba una respuesta
  //3. Imprimir esa respuesta
  //4. Enviar un nuevo numero X
  
  sprintf(buffer, "%d", idx); //se guarda el string con el numero X
  idx++; //se incrementa el numero X

  driver.send((uint8_t *)buffer, strlen(buffer)); //Se envia el numero X
  driver.waitPacketSent(); //Se espera a que el numero X sea enviado

  unsigned long wait_start = millis(); //se establece la hora de inicio de la espera
  int max_wait = 8000; //se establece una espera maxima de 8 segundos
  bool exceed_wait = true;

  while((millis() - wait_start)<max_wait){ //repetir esto durante la espera
    uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
    uint8_t buflen = sizeof(buf);
    while (driver.recv(buf,&buflen)){//hacer esto si hay algo para recibir
      buf[buflen]=0; //creo que es para finalizar el char array con /0 para ayudar al print
      //este es el mismo formato que hay en el slave para comparar lecturas
      Serial.print("Received(");
      Serial.print(buflen);
      Serial.print(",");
      int actual_idx=atoi(buf);
      Serial.print(actual_idx>last_idx ? "+" : "-");
      Serial.print(actual_idx-last_idx);
      last_idx=actual_idx;
      Serial.print("}: ");
      driver.printBuffer(buf, buf, buflen);

      exceed_wait = false; //se actualiza que un dato fue recibido
      }
      if (!exceed_wait){
        break; //se sale del while
        }
    }
  if (exceed_wait){
    Serial.println("No data received");
    }
  
  delay(500);
}
