
void PirDataCollect(void * parameter) {
  while (1) {
      pirStat = digitalRead(pirPin);      
      delay(600);
      }
    } 

void LdrDataCollect(void * parameter) {
  while (1) {    
      ldrVal = analogRead(ldrPin);    
      delay(5000);
      Serial.println(ldrVal);
      }
    } 


void AirDataCollect(void * parameter) {
  while (1) {
    if (airSensor.dataAvailable())
      {
        co2 = airSensor.getCO2();
        Temp = airSensor.getTemperature();
        RH = airSensor.getHumidity();
        Serial.print("co2: ");
        Serial.println(co2);
      }
      else{
        Serial.print("cannot get air data");
      }
      delay(8000);
  }
}
