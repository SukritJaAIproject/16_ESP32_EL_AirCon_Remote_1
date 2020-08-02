void reconnect(void * parameter) {
 while(1){
//    Serial.println("Check Connection");
    int counter = 0;

    while (WiFi.status() != WL_CONNECTED) { //during lost connection, print dots
      Serial.println(counter);
      WiFi.begin(ssid, password); 
       delay(500);
       Serial.print(".");
        counter++;
       if(counter>=60){ //30 seconds timeout - reset board
       ESP.restart();
     }
     delay(500);  
  }
       delay(10000);               
 }
}
