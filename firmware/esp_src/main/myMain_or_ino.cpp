/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
//General Includes (IDF required)
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"

//Arduino specific includes.
#include <Arduino.h>
#include <HardwareSerial.h>

/**
 * IDF example main function.  Nyo arduino libraries used, purelly esp-idf sdk libraries.
 */


//extern "C" void app_main()
//{
//    printf("Hello world!\n");
//
//    /* Print chip information */
//    esp_chip_info_t chip_info;
//    esp_chip_info(&chip_info);
//    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
//           chip_info.cores,
//           (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
//           (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
//
//    printf("silicon revision %d, ", chip_info.revision);
//
//    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
//           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
//
//    for (int i = 10; i >= 0; i--) {
//        printf("EspIdf: Restarting in %d seconds...\n", i);
//        vTaskDelay(1000 / portTICK_PERIOD_MS);
//    }
//    printf("Restarting now.\n");
//    fflush(stdout);
//    esp_restart();
//}

/**
 * Example code for arduino as component of IDF build system.  initArduino() is required to use arduino libraries/classes.
 */


extern "C" void app_main()
{
    initArduino();

    Serial.begin(115200);


//    Serial.printf("Hello world!\n");  //Note will not work.
    Serial.write("Second Hello World...\n");

    for (int i = 10; i >= 0; i--) {
        Serial.printf("Arduino app_main: Restarting in %d seconds...\n", i);  //Serial class supports printf like formatting.
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    Serial.write("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}

/**
 * Example code for setup, loop arduino execution code could be run through arduino build system.
 */


//void setup(){
//    initArduino();
//
//    Serial.begin(115200);
//
//
////    Serial.printf("Hello world!\n");  //Note will not work.
//    Serial.write("Second Hello World...\n");
//
//    for (int i = 10; i >= 0; i--) {
//        Serial.printf("Arduino Setup: Restarting in %d seconds...\n", i);  //Serial class supports printf like formatting.
//        vTaskDelay(1000 / portTICK_PERIOD_MS);
//    }
//
//    Serial.write("Restarting now.\n");
//    fflush(stdout);
//    esp_restart();
//}
//
//void loop(){
//
//}