void OnOffControl(void * parameter) {
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {
      
      
      // Now send the IR signal.
      //-------------------------------------------------------------------------------
      if (OldOnOffButton != OnOffButton) {
        if (OnOffButton == 1) {
    
#if SEND_MITSUBISHI_AC
          ac.setMode(kMitsubishiAcCool);
          ac.on();
          ac.send();
      
#endif  // SEND_MITSUBISHI_AC  
          OldOnOffButton = OnOffButton;
        }
        else {
#if SEND_MITSUBISHI_AC
          ac.off();
          ac.send();
          
#endif  // SEND_MITSUBISHI_AC  
          OldOnOffButton = OnOffButton;
        }
      }
      delay(30);
    }
    delay(20);
  }
}

void TempControl(void * parameter) {
  while (1) {
    if (WiFi.status() == WL_CONNECTED) {


      if (TempSetting != OldTempSetting) {

#if SEND_MITSUBISHI_AC
        ac.setTemp(TempSetting);

        ac.send();
#endif  // SEND_MITSUBISHI_AC     
        OldTempSetting = TempSetting;
      }
      delay(30);
    }
    delay(20);
  }
}

void FanControl(void * parameter) {
  while (1) {
    while (WiFi.status() == WL_CONNECTED) {
      if (FanSetting != OldFanSetting) {
#if SEND_MITSUBISHI_AC
        ac.setFan(FanSetting);
        ac.send();
#endif  // SEND_MITSUBISHI_AC       
        OldFanSetting = FanSetting;
      }
      delay(30);
    }
    delay(20);
  }
}
