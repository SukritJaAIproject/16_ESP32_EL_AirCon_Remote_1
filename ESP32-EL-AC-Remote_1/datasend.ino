int current_day = 0; //day of week
int current_hour = 0;
int current_min = 0;

void DataSend(void * parameter){
 while(1){
    if (WiFi.status() == WL_CONNECTED) {
           pirStat = digitalRead(pirPin);
          time_t now = time(nullptr);
           struct tm* p_tm = localtime(&now);
  timeTostring = ctime(&now);                   //convert time data to string
  current_day = p_tm->tm_wday;
  current_hour = p_tm->tm_hour;
  current_min = p_tm->tm_min;
  delay(50);
   
     if (airSensor.dataAvailable())
  {
    co2 = airSensor.getCO2();
    Temp = airSensor.getTemperature();
    RH = airSensor.getHumidity();
    delay(50); 
 
  }
  else{
    Serial.println("Waiting for new data");
  }

     }
  delay(5000);
     
 }
}
