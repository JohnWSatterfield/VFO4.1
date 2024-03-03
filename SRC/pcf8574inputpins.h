/* 
 * File:   pcf8574.h
 * Author: KI5IDZ / J. Satterfield
 *
 * Created on 2022/08/28
 */
 /*
  * for pcf8574 external memory expansion for ESP32
  * i2c normally set in .ino file
  * change i2c pins if if not using default pins
  * encoder count #define  EncoderStep set in .ino file
  * PCF8574 declaration below sets the i2c address default at 0x20
  * change address if PCF8574 using other i2c address
  * change PCF8574 interrupt pin below #define INTERRUPTED_PIN 
  * 
 */

#include "Arduino.h"    //  https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Arduino.h
#include "PCF8574.h"    //  https://github.com/xreef/PCF8574_library
#include <Wire.h>       //  https://github.com/esp8266/Arduino/tree/master/libraries/Wire
#include "config.h"

uint8_t buttonChange[] {HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH,HIGH};
volatile bool keyPressed = false;

#define numberInputPins 6

//Set the i2c address normally not used, default set in .ino file

//#define SCL 22 // i2c SCL ESP32 default 22; uses 13 for ESP32-S3 mini  
//#define SDA 21 // i2c SDA ESP32 default 21; uses 7 for ESP32-S3 mini

// Function interrupt ICACHE_RAM_ATTR 
void ICACHE_RAM_ATTR keyPressedOnPCF8574();

// Set i2c address
PCF8574 pcf8574(0x20, INTERRUPTED_PIN, keyPressedOnPCF8574);
 
void pinSetup(void) {
  pcf8574.pinMode(P0, INPUT_PULLUP);
  pcf8574.pinMode(P1, INPUT_PULLUP);
  pcf8574.pinMode(P2, INPUT_PULLUP);
  pcf8574.pinMode(P3, INPUT_PULLUP);
  pcf8574.pinMode(P4, INPUT_PULLUP);
  pcf8574.pinMode(P5, OUTPUT, LOW);
  pcf8574.pinMode(P6, OUTPUT, LOW);
  pcf8574.pinMode(P7, INPUT_PULLUP);
  if (pcf8574.begin()){  // Initialize pcf8574
    Serial.println("pcf8574 OK");
  }else{
    Serial.println("pcf8574 not OK");
  }           
  delay(10);
}

void readSetPins() {
    uint8_t val;
    val = pcf8574.digitalRead(P0);
    if (val==LOW) buttonChange[0] = LOW; else buttonChange[0] = HIGH;
    val = pcf8574.digitalRead(P1);
    if (val==LOW) buttonChange[1] = LOW; else buttonChange[1] = HIGH;
    val = pcf8574.digitalRead(P2);
    if (val==LOW) buttonChange[2] = LOW; else buttonChange[2] = HIGH;
    val = pcf8574.digitalRead(P3);
    if (val==LOW) buttonChange[3] = LOW; else buttonChange[3] = HIGH;
    val = pcf8574.digitalRead(P4);
    if (val==LOW) buttonChange[4] = LOW; else buttonChange[4] = HIGH;
    val = pcf8574.digitalRead(P7);
    if (val==LOW) buttonChange[7] = LOW; else buttonChange[7] = HIGH;
    delay(50);
}

void updatePins() {
  if (keyPressed) {
    PCF8574::DigitalInput di = pcf8574.digitalReadAll();
    //buttonChange[5] = di.p5;
    //buttonChange[6] = di.p6;
    //buttonChange[7] = di.p7;
    keyPressed = false; 
    delay(50);
  }
}

void keyPressedOnPCF8574(){
  // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
   keyPressed = true;
}
