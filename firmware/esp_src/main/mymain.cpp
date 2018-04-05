/***************************
    mymain.cpp

    Project: Craig hospital Fishinator

    Purpose: This project is the command and control firmware for the ESP32 based Fishinator; a computer controlled
    fishing rod built at Craig Hospital.  The


    Created by Patrick Wagner on 1/1/18.

---------------------------------------------
MIT License:
Copyright 2018 by Patrick Wagner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************/
//General Includes (IDF required)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <esp_log.h>
#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_err.h>

//NVS flash api IDF includes.
#include <nvs_flash.h>
#include <nvs.h>

//Arduino - esp specific includes. For roboclaw UART and Encoder SPI use.
#include <Arduino.h>
#include <SPI.h>

//Include third party libraries for specific program.  Uses Arduino libraries.
#include "RoboClaw.h"
#include "AMT20_ABSQUADENC_SPI.h"

//Include Fishinator files.
#include "fish_prompts.h"
#include "fish_CnC.h"

//Program log tag.
static const char* TAG = "Fishinator";


/**
 * IDF main function with used in conjuction with arduino libraries.
 */
extern "C" void app_main()
{
    esp_err_t err;  //Standard return code variable.


    // Set GPIO for solenoid behavior.
    gpio_config(&Solenoid_GPIOconfig);
    gpio_set_level(solenoid_gpio_pin,1);

    //Start AMT20 enocoder
    AMTencoder.begin();

    /** Trying arduino spi library **/
    roboclaw.begin(460800);
    ets_delay_us(5000);
//    roboclaw.begin(230400);
    roboclaw.DutyM1(roboclaw_addr,0);
    roboclaw.DutyM2(roboclaw_addr,0);

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    //Open NVS
    err = nvs_open("fishinator", NVS_READWRITE, &my_nvs_handle);
    if (err != ESP_OK) {
//        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        printf("Error (%i) opening NVS handle!\n", static_cast<int32_t>(err));
    } else {
        printf("NVS Opened\n");
    }


    //Read abs encoder position and update
    do{
        sync_encoders();
        ets_delay_us(2000);
        sync_encoders();
    }
    while(AMT20_abs_position == -1);
    roboclaw.SetM1VelocityPID(roboclaw_addr,0,0,0,rod_slow_move_params.qpps);  //QPPS apparently needs to be set even if not using VPID
    ets_delay_us(2000);
    roboclaw.SetM1PositionPID(roboclaw_addr,rod_slow_move_params.pid_P,rod_slow_move_params.pid_I,
                              rod_slow_move_params.pid_D, rod_slow_move_params.pid_maxI,
                              rod_slow_move_params.deadzone, rod_slow_move_params.min_position,
                              rod_slow_move_params.max_position);
    ets_delay_us(2000);


    //System Init should be complete: Print welcome:===============
    printf(fishp_splash);
    printf(fishp_intro);
    printf("Version: %i.%i-%i\n\n",FISH_MAJOR_REV,FISH_MINOR_REV,FISH_DATE_REV);
    printf(fishp_help);

    /// Create threads to handle program tasks.
//    xTaskCreatePinnedToCore(&print_task, "print_task",configMINIMAL_STACK_SIZE,NULL,5,NULL,1);
//    xTaskCreate(&fish_nvs_storevals_task, "NVS task",8192,NULL, 6,NULL);
    xTaskCreate(&fishing_serialcomm_control_task, "Comm task",8192,NULL, 5,NULL);


}

//void fish_nvs_storevals_task(void* novars){
//
////    esp_err_t err = fish_nvs_write();
////    printf(".+");
//    while(true) {
//        printf(".+");
//        vTaskDelay(3000 / portTICK_PERIOD_MS);
//    }
//}




void IRAM_ATTR AMT20enc_isr_handler(void* args){
    static DRAM_ATTR int8_t lookup_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    static DRAM_ATTR int enc_val = 0;
    enc_val = enc_val << 2;
    enc_val = enc_val | ((gpio_get_level(PIN_ENCA_NUM) << 1) + gpio_get_level(PIN_ENCB_NUM));
    esp_quadenc_position = esp_quadenc_position + lookup_table[enc_val & 0b1111];

    if(casting_semaphore){  //A flag to watch for a specific event(location), action is to raise GPIO pin.
        if(esp_quadenc_position == rod_cast_release_pos){
            casting_semaphore = 0;
            gpio_set_level(solenoid_gpio_pin,1);
            cast_duration_timer = (cast_duration_timer- micros());
        }
    }

}

