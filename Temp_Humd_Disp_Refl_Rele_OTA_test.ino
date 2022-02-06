#include <Wire.h> // responsável pela comunicação com a interface i2c
#include <LiquidCrystal_I2C.h> // responsável pela comunicação com o display LCD
//#include <dht.h>
#include <ESP8266WiFi.h> //lib do wifi para o ESP8266
#include <ESP8266WiFiMulti.h> //lib do wifi para o ESP8266
#include <ArduinoOTA.h> //lib do ArduinoOTA

//dht DHT;
#include "DHT.h"

#define DHTPIN 13     // Digital pin connected to the DHT sensor
// Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// Pin 15 can work but DHT must be disconnected during program upload.

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 3 (on the right) of the sensor to GROUND (if your sensor has 3 pins)
// Connect pin 4 (on the right) of the sensor to GROUND and leave the pin 3 EMPTY (if your sensor has 4 pins)
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);

ESP8266WiFiMulti wifiMulti;

const char* ssid = "Link Start"; //nome da rede
const char* password = "1234#563"; //senha da rede

#define DHT22_PIN 13
// Inicializa o display no endereço 0x27
//os demais parâmetros, são necessários para o módulo conversar com o LCD
//porém podemos utilizar os pinos normalmente sem interferência
//parâmetro: POSITIVE > > Backligh LIGADO | NEGATIVE > > Backlight desligado

#define SPresenca_PIN 2
#define S_Refletancia 16
#define Rele1 0
#define Rele2 12
#define Rele3 14

  unsigned long timer = 0;
  int auxInterruptor = 0;
  int opcaoInterruptor = 0;
  bool estadoPinLed1 = 1;
  bool estadoPinLed2 = 0;
  bool estadoPinLed3 = 0;
  unsigned long count_time = micros();
  int countDHT22Time = micros();

struct
{
    uint32_t total;
    uint32_t ok;
    uint32_t crc_error;
    uint32_t time_out;
    uint32_t connect;
    uint32_t ack_l;
    uint32_t ack_h;
    uint32_t unknown;
} stat = { 0,0,0,0,0,0,0,0};

//LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);
// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

void interruptorVerificacao(int estado,int *auxInterruptor,int *opcaoInterruptor, unsigned long *timer){
  
  if(estado == 1&& *auxInterruptor ==0){
    //Serial.println("Test4");
    *auxInterruptor = 1;
    *opcaoInterruptor += 1;
    *timer = micros();
  }
  if(!(estado)){
    //Serial.println("Test5");
    *auxInterruptor = 0;
  } 
  count_time = micros();
}



void interruptorAcao(int *opcaoInterruptor,bool *estadoPinLed1,bool *estadoPinLed2,bool *estadoPinLed3, unsigned long *timer){
  //Serial.println("test3");
  if(*opcaoInterruptor == 1){
    if(*estadoPinLed1){
      digitalWrite(Rele1, LOW);
      //Serial.println("Test1");
      *estadoPinLed1 = 0;
      Serial.println("LED01  DESLIGADO");
    }else{
      digitalWrite(Rele1, HIGH);
      //Serial.println("Test2");
      *estadoPinLed1 = 1;
      Serial.println("LED01  LIGADO");
    }
  }
  if(*opcaoInterruptor == 2){
    if(*estadoPinLed2){
      digitalWrite(Rele2, LOW);
      *estadoPinLed2 = 0;
      Serial.println("LED02  DESLIGADO");
    }else{
      digitalWrite(Rele2, HIGH);
      *estadoPinLed2 = 1;
      Serial.println("LED02  LIGADO");
    }
  }
  if(*opcaoInterruptor > 2){
    if(*estadoPinLed3){
      digitalWrite(Rele3, LOW);
      *estadoPinLed3 = 0;
      Serial.println("LED03  DESLIGADO");
    }else{
      digitalWrite(Rele3, HIGH);
      *estadoPinLed3 = 1;
      Serial.println("LED03  LIGADO");
    }
  }
  *opcaoInterruptor = 0; 
  *timer = 0;
  
}

void OTA_setup(){
  //inicia serial com 115200 bits por segundo
   
   
   Serial.println("Booting");   
   
   ////////////wifiMulti.addAP(ssid, password); 
   
   Serial.println("Connecting ...");
   
  /* while (wifiMulti.run() != WL_CONNECTED)
   {       
       delay(250);
       Serial.print('.');
   }
  */
  // A porta fica default como 8266
  // ArduinoOTA.setPort(8266);

  // Define o hostname (opcional)
  ArduinoOTA.setHostname("AutoCozinhaYGLC");

  // Define a senha (opcional)
  ArduinoOTA.setPassword("Autoyglc1");

  // É possível definir uma criptografia hash md5 para a senha usando a função "setPasswordHash"
  // Exemplo de MD5 para senha "admin" = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  //define o que será executado quando o ArduinoOTA iniciar
  ArduinoOTA.onStart([](){ startOTA(); }); //startOTA é uma função criada para simplificar o código 

  //define o que será executado quando o ArduinoOTA terminar
  ArduinoOTA.onEnd([](){ endOTA(); }); //endOTA é uma função criada para simplificar o código 

  //define o que será executado quando o ArduinoOTA estiver gravando
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) { progressOTA(progress,total); }); //progressOTA é uma função criada para simplificar o código 

  //define o que será executado quando o ArduinoOTA encontrar um erro
  ArduinoOTA.onError([](ota_error_t error){ errorOTA(error); });//errorOTA é uma função criada para simplificar o código 
  
  //inicializa ArduinoOTA
  ArduinoOTA.begin();

  //iniciALIZA O DHT
  dht.begin();
  
  //exibe pronto e o ip utilizado pelo ESP
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup()
{
    
    Serial.begin(115200);
/*    Serial.println("dht22_test.ino");
    Serial.print("LIBRARY VERSION: ");
    Serial.println(DHT_LIB_VERSION);
    Serial.println();
    Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)\tTime (us)");
  */
  //Wire.begin(0,2);//esp-01
  pinMode(Rele1 ,OUTPUT);
  pinMode(Rele2 ,OUTPUT);
  pinMode(Rele3 ,OUTPUT);
  pinMode(S_Refletancia, INPUT);
  pinMode(SPresenca_PIN, INPUT);
  Wire.begin();
  OTA_setup();
  //inicializa o display (16 colunas x 2 linhas)
  lcd.begin (16,2); // ou 20,4 se for o display 20x4
}

void startOTA()
{
   String type;
   
   //caso a atualização esteja sendo gravada na memória flash externa, então informa "flash"
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "flash";
    else  //caso a atualização seja feita pela memória interna (file system), então informa "filesystem"
      type = "filesystem"; // U_SPIFFS

    //exibe mensagem junto ao tipo de gravação
    Serial.println("Start updating " + type);
}

//exibe mensagem
void endOTA()
{
  Serial.println("\nEnd");
}

//exibe progresso em porcentagem
void progressOTA(unsigned int progress, unsigned int total)
{
   Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

//caso aconteça algum erro, exibe especificamente o tipo do erro
void errorOTA(ota_error_t error)
{  
      Serial.printf("Error[%u]: ", error);
      
      if (error == OTA_AUTH_ERROR) 
        Serial.println("Auth Failed");
      else
      if (error == OTA_BEGIN_ERROR)
        Serial.println("Begin Failed");
      else 
      if (error == OTA_CONNECT_ERROR)
        Serial.println("Connect Failed");
      else
      if (error == OTA_RECEIVE_ERROR) 
        Serial.println("Receive Failed");
      else 
      if (error == OTA_END_ERROR)
        Serial.println("End Failed");
}

void funcDHT22(){
  /*// READ DATA
    Serial.print("DHT22, \t");

    uint32_t start = micros();
    int chk = DHT.read22(DHT22_PIN);
    uint32_t stop = micros();

    stat.total++;
    switch (chk)
    {
    case DHTLIB_OK:
        stat.ok++;
        Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        stat.crc_error++;
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        stat.time_out++;
        Serial.print("Time out error,\t");
        break;
    default:
        stat.unknown++;
        Serial.print("Unknown error,\t");
        break;
    }
    // DISPLAY DATA
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t");
    Serial.print(DHT.temperature, 1);
    Serial.print(",\t");
    Serial.print(stop - start);
    Serial.println();

    if (stat.total % 20 == 0)
    {
        Serial.println("\nTOT\tOK\tCRC\tTO\tUNK");
        Serial.print(stat.total);
        Serial.print("\t");
        Serial.print(stat.ok);
        Serial.print("\t");
        Serial.print(stat.crc_error);
        Serial.print("\t");
        Serial.print(stat.time_out);
        Serial.print("\t");
        Serial.print(stat.connect);
        Serial.print("\t");
        Serial.print(stat.ack_l);
        Serial.print("\t");
        Serial.print(stat.ack_h);
        Serial.print("\t");
        Serial.print(stat.unknown);
        Serial.println("\n");
    }*/


    // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  

      //acende o backlight do LCD
  lcd.setBacklight(HIGH);
  //posiciona o cursor para escrita
  //.setCursor(coluna, linha)
  lcd.setCursor(0,0);
  lcd.print("Temperatura:");
  lcd.print(t);
  lcd.setCursor(0,1);
  lcd.print("Humidade:");
  lcd.print(h);///////////////////////////
  delay(0); //intervalo de 1s
  //desliga o backlight do LCD
  //lcd.setBacklight(LOW);
  delay(0); //intervalo de 1s

    
     
}


void loop()
{
  if(wifiMulti.run() != WL_CONNECTED)
   {       
       wifiMulti.addAP(ssid, password); 
       delay(250);
       Serial.print('.');
   }
  //Handle é descritor que referencia variáveis no bloco de memória
  //Ele é usado como um "guia" para que o ESP possa se comunicar com o computador pela rede
  ArduinoOTA.handle();


//////////////////
  
  if(micros() - count_time > 600){
    if(digitalRead(S_Refletancia)){
      interruptorVerificacao(1,&auxInterruptor, &opcaoInterruptor, &timer);
      //Serial.println(pinSensor1); 
    }else{
      interruptorVerificacao(0, &auxInterruptor, &opcaoInterruptor, &timer);
      //Serial.println("Branco");
    }
  }
  //Serial.println(opcaoInterruptor);
  if(micros() - timer >1000000 && timer > 0){
    //Serial.println(micros() - timer);
    interruptorAcao(&opcaoInterruptor, &estadoPinLed1, &estadoPinLed2, &estadoPinLed3, &timer);
    
  }
//Serial.println(micros() - timer);
//delay(1000);

   if((micros()- countDHT22Time) > 500000){
      Serial.println((micros()- countDHT22Time));
      funcDHT22();
      countDHT22Time = micros();  
    }

  
}
