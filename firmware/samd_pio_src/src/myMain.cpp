//
// Created by patrick on 2/27/18.
//

#include <Arduino.h>
#include <Usb.h>


int vled = 0;

void setup(){
 pinMode(LED_BUILTIN,OUTPUT);

}

void loop(){

    if(vled ==0){
        digitalWrite(LED_BUILTIN, vled);
        vled = 1;
    } else{
        digitalWrite(LED_BUILTIN,vled);
        vled = 0;
    }

    delay(121);


}
