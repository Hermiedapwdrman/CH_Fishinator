//
// Created by patrick on 2/27/18.
//

#include <Arduino.h>
//#include <Usb.h>


int vled = 0;
unsigned long next_time = 0;

void setup(){
    pinMode(LED_BUILTIN,OUTPUT);
    Serial1.begin(115200);
    Serial.begin(115200);
    SERIAL_PORT_MONITOR.begin(115200);
    SerialUSB.begin(115200);
    next_time = millis();

}

void loop(){

    if(vled ==0){
        digitalWrite(LED_BUILTIN, vled);
        vled = 1;
    } else{
        digitalWrite(LED_BUILTIN,vled);
        vled = 0;
    }
    delay(421);

    Serial1.print(F("U5U5"));
    Serial.print(F("X2X2"));
    SERIAL_PORT_MONITOR.print(F("ABAB"));
    SerialUSB.print(F("HGHG"));

}
