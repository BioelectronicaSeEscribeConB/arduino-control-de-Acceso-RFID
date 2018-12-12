#include <MFRC522.h> //Descargar e incluir la librería
#include <SPI.h>   //libreria de protocolo de datos de comunicacion

#define SAD 10    //SDA
#define RST 5     //VCC
MFRC522 nfc(SAD, RST);  //Comunicacion


#define chapa    2    //Pin donde estara conectado el Rele


void setup() 
{
  pinMode(chapa , OUTPUT);      //chapa como salida
  SPI.begin();      //iniciamos protocolo
  Serial.begin(115200); //comunicacion a 115200
  Serial.println("Buscando RC522"); 
  nfc.begin();  //inicia a buscar el sensor RFID
  byte version = nfc.getFirmwareVersion(); //Buscando la version de RFID
  if (! version) 
  {//Entra si no encuentra el módulo.
    Serial.print("No ha encontrado RC522");
    while(1); //detener
  }
  Serial.print("Ha encontrado RC522");
  Serial.print("Firmware version 0x");
  Serial.print(version, HEX);  //version en exadesimal
  Serial.println(".");
}

#define AUTHORIZED_COUNT 2 //Para autoriazar más tarjetas ponemos el número aqui y la añadimos abajo
byte Authorized[AUTHORIZED_COUNT][6] = 
{
{0x67, 0x50, 0x5D, 0x04, 0x6E,}
,{0x77, 0xA5, 0x94, 0x04, 0x42,}
};
                          
void printSerial(byte *serial);//subrutina de impresion de datos
boolean isSame(byte *key, byte *serial); //subrutina de colocacion de datos
boolean isAuthorized(byte *serial); //subrutina de autorizar

void loop() 
{
  byte status;
  byte data[MAX_LEN];
  byte serial[5];
  boolean Abierto = false; //variable para abrir o cerrar
  digitalWrite(chapa, Abierto); //de momento se encuentra cerrado
  status = nfc.requestTag(MF1_REQIDL, data); //aduiere el registro ID
  if (status == MI_OK)    //si obtuvios una ID
  {
    status = nfc.antiCollision(data);
    memcpy(serial, data, 5);
    if(isAuthorized(serial))
    { 
      Serial.println("Autorizado");
      Abierto = true;     //abre la chapa
    }
    else
    { 
      printSerial(serial);
      Serial.println("NO autorizado");
      Abierto = false;   //mantiene cerrado
    }
    nfc.haltTag();
    digitalWrite(chapa, Abierto);
    delay(2000);
  }//if (status == MI_OK)
  delay(500);

}//void loop()

//rituna de adisicion de datos de ID
boolean isSame(byte *key, byte *serial)
{
    for (int i = 0; i < 4; i++) {
      if (key[i] != serial[i])
      { 
        return false; 
      }
    }
    
    return true;

}
//Rutina autorizacion de datos
boolean isAuthorized(byte *serial)
{
    for(int i = 0; i<AUTHORIZED_COUNT; i++)
    {
      if(isSame(serial, Authorized[i]))
        return true;
    }
   return false;
}
//Rutina de Lectura de Datos ID
void printSerial(byte *serial)
{
        Serial.print("Serial:");
    for (int i = 0; i < 5; i++) {// aumentar a 5 para leer el número de la tarjeta completo
      Serial.print(serial[i], HEX);
      Serial.print(" ");
    }
}
