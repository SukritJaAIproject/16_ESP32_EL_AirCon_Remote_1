/* Copyright 2017, 2018 David Conran
*
* An IR LED circuit *MUST* be connected to the ESP8266 on a pin
* as specified by kIrLed below.
*
* TL;DR: The IR LED needs to be driven by a transistor for a good result.
*
* Suggested circuit:
*     https://github.com/crankyoldgit/IRremoteESP8266/wiki#ir-sending
*
* Common mistakes & tips:
*   * Don't just connect the IR LED directly to the pin, it won't
*     have enough current to drive the IR LED effectively.
*   * Make sure you have the IR LED polarity correct.
*     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
*   * Typical digital camera/phones can be used to see if the IR LED is flashed.
*     Replace the IR LED with a normal LED if you don't have a digital camera
*     when debugging.
*   * Avoid using the following pins unless you really know what you are doing:
*     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
*     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
*     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
*   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
*     for your first time. e.g. ESP-12 etc.
*/
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Panasonic.h>

const uint16_t kIrLed = 23;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
IRPanasonicAc ac(kIrLed);  // Set the GPIO used for sending messages.

//void printState() {
//  // Display the settings.
//  Serial.println("Panasonic A/C remote is in the following state:");
//  Serial.printf("  %s\n", ac.toString().c_str());
//  // Display the encoded IR sequence.
//  unsigned char* ir_code = ac.getRaw();
//  Serial.print("IR Code: 0x");
//  for (uint8_t i = 0; i < kPanasonicAcStateLength; i++)
//    Serial.printf("%02X", ir_code[i]);
//  Serial.println();
//}

int Swing;

void setup() {
  ac.begin();
  Serial.begin(115200);
  delay(200);

//Set up what we want to send. See ir_Panasonic.cpp for all the options.
  Serial.println("Default state of the remote.");
//printState();
  Serial.println("Setting desired state for A/C.");
  ac.setModel(kPanasonicLke);
  ac.on();
  ac.setFan(kPanasonicAcFanAuto);
  ac.setMode(kPanasonicAcCool);
  ac.setTemp(26);
  ac.setSwingVertical(kPanasonicAcSwingVAuto);
  ac.setSwingHorizontal(kPanasonicAcSwingHAuto);
}

void loop() {
  // Now send the IR signal.
  
#if SEND_PANASONIC_AC

  ac.off();
//  ac.setTemp(25, true);
//  ac.setFan(kPanasonicAcFanMax);
//  ac.setSwingVertical(kPanasonicAcSwingVAuto);
  ac.send();
  delay(50);
  Serial.print("Set Temp: ");
  Serial.println(ac.getTemp());
  Serial.print("Set Fan: ");
  Serial.println(ac.getFan());
  Serial.print("Set Swing: ");
  Swing = ac.getSwingVertical(); 
  Serial.println(SwingValue(Swing));
//  printState();
  Serial.println("__________________________");
 
  
#endif

  delay(20000);
  
#if SEND_PANASONIC_AC
  ac.on();
//  ac.setTemp(26, true);
//  ac.setFan(kPanasonicAcFanMin);
//  ac.setSwingVertical(kPanasonicAcSwingVLowest);
  ac.send();
  delay(50);
  Serial.print("Set Temp: ");
  Serial.println(ac.getTemp());
  Serial.print("Set Fan: ");
  Serial.println(ac.getFan());
  Serial.print("Set Swing: ");
  Swing = ac.getSwingVertical(); 
  Serial.println(SwingValue(Swing));
//  printState();
  Serial.println("__________________________");
  
#endif  // SEND_PANASONIC_AC

  delay(20000);

}


String SwingValue(int SwingVal){
  String SwingString;
  switch(SwingVal){
    case 1: SwingString = "Highest";
            break;
    case 2: SwingString = "High";
            break;
    case 3: SwingString = "Middle";
            break;  
    case 4: SwingString = "Low";
            break;  
    case 5: SwingString = "Lowest";
            break;
    case 15: SwingString = "Auto";
            break;                                        
}
  return SwingString;

}
