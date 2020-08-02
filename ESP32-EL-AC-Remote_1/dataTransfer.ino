
void DataTransfer(void * parameter) {
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      FirebaseJson updateData;
      
      updateData.set("PIR",String(pirStat));
      updateData.set("LDR", String(4095 - ldrVal));
      updateData.set("co2", String(co2));
      updateData.set("RH", String(RH));
      updateData.set("temp", String(Temp));
      updateData.set("ACStatus", String(OnOffButton));
      updateData.set("setTemp", String(TempSetting));
      updateData.set("setFan", String(FanSetting));

      if (Firebase.updateNode(firebaseData, "/nodemcu/status/-MAPI3e3ReHksFxyPmQb/", updateData)) {
          Serial.println(firebaseData.dataPath());
          Serial.println(firebaseData.dataType());
          Serial.println(firebaseData.jsonString()); 
          } else {
          Serial.println(firebaseData.errorReason());
        }

      
    }
    delay(200000);
  }

}


//void DataTransfer(void * parameter) {
//  while (1) {
//    if (WiFi.status() == WL_CONNECTED) {
// 
//      StaticJsonDocument<200> doc1;
//      HTTPClient http1;
//      http1.begin(url);
//      http1.addHeader("Content-Type", "application/json");
//
//      doc1["PIR"] = String(pirStat);
//      doc1["LDR"] = String(4095 - ldrVal);
//      doc1["co2"] = String(co2);
//      doc1["RH"] = String(RH);
//      doc1["temp"] = String(Temp);
//      doc1["ACStatus"] = String(OnOffButton);
//
//      String postBody;
//      serializeJson(doc1, postBody);
//      int httpResponseCode = http1.POST(postBody);
//      Serial.println(postBody);
//      Serial.println(httpResponseCode);
//
//    }
//    delay(20000);
//  }
//
//}
