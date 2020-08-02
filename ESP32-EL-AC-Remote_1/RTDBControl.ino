void getData(void * parameter) {
  while (1) {
  
    if(Firebase.getJSON(firebaseData, "/nodemcu/status/-MAPCsPueXDWFdx-hz6F/")){
    deserializeJson(doc, firebaseData.jsonString());
    OnOffButton = doc["AC"];
    TempSetting = doc["T"];
    FanSetting = doc["FS"];
//    Serial.print("ACStatus");
//    Serial.print(OldOnOffButton);
//    Serial.println(OnOffButton);
    }
    else{
      Serial.println(firebaseData.errorReason());
    }
    delay(1000);

    
  }
}
