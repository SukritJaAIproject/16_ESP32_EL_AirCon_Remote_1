
TaskHandle_t TempControlFunction;
TaskHandle_t DataTransferFunction;
TaskHandle_t FanControlFunction;
TaskHandle_t reconnectFunction;
TaskHandle_t AirDataCollectFunction;
TaskHandle_t PirDataCollectFunction;
TaskHandle_t LdrDataCollectFunction;
TaskHandle_t getDataFunction;
TaskHandle_t OnOffControlFunction;
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <time.h>
#include <FirebaseESP32.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>
#include <ir_Mitsubishi.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include "SparkFun_SCD30_Arduino_Library.h"
//------------------------------------------------------------------------------
#define FIREBASE_HOST "database-test-277802.firebaseio.com"
#define FIREBASE_AUTH "qJLp1qMmrlROZweNrHZ8FWpZjjx2MHLxxxxxxxxx"
//Define Firebase Data object
FirebaseData firebaseData;
FirebaseJson json;
int n = 0;
//------------------------------------------------------------------------------bwYBVEznRiYcrA344B6HjffNpzFuq3TvCYQt5tIA
#define interruptPin 36
//------------------------------------------------------------------------------
//Sensor
SCD30 airSensor;
//------------------------------------------------------------------------------
//PIR
#define pirPin 16
int pirStat = 0;
//------------------------------------------------------------------------------
#define ldrPin 36
int ldrVal;
//------------------------------------------------------------------------------
String url = "https://us-central1-database-test-277802.cloudfunctions.net/fromEsp32";
//------------------------------------------------------------------------------
//Connection
char auth[] = "aN223q_9Hh4ww-yDoizZ4P6V74flZ1dv";
char pass[] = "11082501";
//set time parameter
int timezone = 7 * 3600;                    //setting Thailand time-zone
int dst = 0;                                //setting data swing time
String timeTostring;                        //setting time data as string
//const char* ssid     = "Woodito";          // The SSID (name) of the Wi-Fi network you want to connect to
//const char* password = "13572468";        // The password of the Wi-Fi network
const char* ssid     = "wutdito";          // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "11082501";        // The password of the Wi-Fi network
//------------------------------------------------------------------------------
//Blynk button
boolean   OnOffButton;                          // On/Off
uint16_t  TempSetting = 25;                          // Temp(celcius)
uint16_t  FanSetting;                           // Swing
float Temp;                                    //Test
float co2;
float RH;
boolean   OldOnOffButton;                          // On/Off
uint16_t  OldTempSetting;
uint16_t  OldFanSetting;

//------------------------------------------------------------------------------
const uint16_t kIrLed = 23;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRMitsubishiAC ac(kIrLed);  // Set the GPIO used for sending messages.
void printState() {
  // Display the settings.
  Serial.println("Mitsubishi A/C remote is in the following state:");
  Serial.printf("  %s\n", ac.toString().c_str());
  // Display the encoded IR sequence.
  unsigned char* ir_code = ac.getRaw();
  Serial.print("IR Code: 0x");
  for (uint8_t i = 0; i < kMitsubishiACStateLength; i++)
    Serial.printf("%02X", ir_code[i]);
  Serial.println();
}
//------------------------------------------------------------------------------
int Swing;
DynamicJsonDocument doc(200);
void setup() {
  //---------------------------------------
  //firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  pinMode(pirPin, INPUT);
  pinMode(interruptPin, OUTPUT);
  ac.begin();
  Serial.begin(115200);
  delay(200);

  // Set up what we want to send. See ir_Mitsubishi.cpp for all the options.
  Serial.println("Default state of the remote.");
//  printState();
  Serial.println("Setting desired state for A/C.");
//  ac.off();
//  ac.setFan(1);
//  ac.setMode(kMitsubishiAcCool);
//  ac.setTemp(TempSetting);
//  ac.setVane(kMitsubishiAcVaneAuto);

  WiFi.mode(WIFI_AP);

  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(500);
    Serial.print('.');
  }
  //Sensor
  int restartCount = 0;
  Wire.begin();
  if (airSensor.begin() == false)
  {
    Serial.println("Air sensor not detected. Please check wiring. Freezing...");
    while (1)
      ;
  }
  else {
    Serial.print("Sensor Connected");
  }
  //------------------------------------------------------------------------------
 


  xTaskCreatePinnedToCore(OnOffControl, "OnOff", 10000, NULL, 3,&OnOffControlFunction, 1);
  xTaskCreatePinnedToCore(TempControl, "TempControl", 10000, NULL, 2,&TempControlFunction, 1);
  xTaskCreatePinnedToCore(DataTransfer, "DataTransfer", 10000, NULL, 4,&DataTransferFunction, 0);
  xTaskCreatePinnedToCore(FanControl, "TempControl", 10000, NULL, 2,&FanControlFunction, 1);
  xTaskCreatePinnedToCore(reconnect, "reconnect", 10000, NULL, 4,&reconnectFunction, 0);
  xTaskCreatePinnedToCore(AirDataCollect, "AirDataCollect", 10000, NULL, 4,&AirDataCollectFunction, 0);
  xTaskCreatePinnedToCore(PirDataCollect, "PirDataCollect", 10000, NULL,1,&PirDataCollectFunction, 0);
  xTaskCreatePinnedToCore(LdrDataCollect, "LdrDataCollect", 10000, NULL,2,&LdrDataCollectFunction, 0);
  xTaskCreatePinnedToCore(getData, "getData", 10000, NULL, 3, &getDataFunction, 0);

  

  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  timeTostring = ctime(&now);                   //convert time data to string
//
//    if(Firebase.getJSON(firebaseData, "/nodemcu/status/-MAPCsPueXDWFdx-hz6F/")){
//    
//    deserializeJson(doc, firebaseData.jsonString());
//    OldOnOffButton = doc["AC"];
//    OnOffButton = doc["AC"];
//    TempSetting = doc["T"];
//    OldTempSetting = doc["T"];
//    FanSetting = doc["FS"];
//    OldFanSetting = doc["FS"];
//    Serial.println("OldOnOffButton: ");
//    Serial.println(OldOnOffButton);
//    Serial.println("OnOffButton: ");
//    Serial.println(OnOffButton);
//    }
//ac.setMode(kMitsubishiAcCool);
}

void loop() {
  delay(1000);
}
