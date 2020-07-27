/*
  Simple example for receiving
  
  https://github.com/sui77/rc-switch/
*/

#include "RCSwitch.h"

RCSwitch mySwitch = RCSwitch();

#define LED 3

volatile long unsigned lastRcv;
volatile bool isOn = false;

void setup() {
  lastRcv = millis();
  
  pinMode(LED, OUTPUT);
  
  Serial.begin(9600);
//  mySwitch.setPulseLength(350);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2
}

void loop() {
  if (mySwitch.available()) {
    unsigned long recValue = mySwitch.getReceivedValue();
    
    Serial.print("Received ");
    Serial.print( recValue );
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    if(recValue == 6392) {
      unsigned long mnow = millis();
      if(mnow - lastRcv > 1000) {
        lastRcv = mnow;
        isOn = !isOn;
        digitalWrite(LED, isOn);
      }
      
    }
    delay(100);
    
    mySwitch.resetAvailable();
  }
}
