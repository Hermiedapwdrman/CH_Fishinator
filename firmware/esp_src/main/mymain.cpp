/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
//General Includes (IDF required)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


//Arduino specific includes.
#include <Arduino.h>
#include <SPI.h>


//Include third party libraries for specific program.
#include "RoboClaw.h"
#include "AMT20_ABSQUADENC_SPI.h"

void print_task(void *novars);  //Print task for encoder value.
void control_comm_task(void* novars); //Monitor incoming com task


/** Encoder Setup **/
AMT20_ABSQUADENC_SPI Aencoder(&SPI);
volatile DRAM_ATTR int32_t esp_quadenc_position = 0;  //Encoder global counter for esp quadrature counting
DRAM_ATTR int16_t AMT20_abs_position = -1;
DRAM_ATTR int32_t roboclaw_position = 0;

/** Roboclaw setup **/
#define roboclaw_addr 0x80
HardwareSerial HWSerial(1);
RoboClaw roboclaw(&HWSerial,10000);


/**Fishing rod testing functions **/
const int32_t rod_cast_begin_pos = 3450;
int32_t rod_cast_release_pos = 2600;
const int32_t rod_cast_finish_pos = 1250;
const int32_t rod_hook_pos = 2400;
const int32_t rod_neutral_pos = 1760;
const int32_t reel_pwm_drive = 0x4000;

void fishing_rod_cast_sequence();

void sync_encoders(){
    AMT20_abs_position = Aencoder.readEncoderPosition();
    esp_quadenc_position = AMT20_abs_position;
    roboclaw.SetEncM1(roboclaw_addr,AMT20_abs_position);
}

/** Solenoid GPI Config **/
const gpio_num_t solenoid_gpio_pin = GPIO_NUM_2;
const gpio_config_t Solenoid_GPIOconfig{ //Define Encoder B behavior, pin selection is arbitrary and might change
      .pin_bit_mask = (1<<solenoid_gpio_pin),
      .mode = GPIO_MODE_OUTPUT,
      .pull_up_en = GPIO_PULLUP_DISABLE,
      .pull_down_en = GPIO_PULLDOWN_DISABLE,
      .intr_type = GPIO_INTR_DISABLE
};


/**
 * IDF main function with used in conjuction with arduino libraries.
 */
extern "C" void app_main()
{
    esp_err_t erret;  //Standard return code variable.



    printf("Testing GPIO interrupts for encoder counting.!\n");
    //TESTING using easier to implement arduino serial.
//    Serial.begin(921600); //Use arduino serial class for ease.

    /** Trying arduino spi library **/
    SPI.begin();

    roboclaw.begin(460800);



// Set GPIO for encoder counting.
    gpio_config(&EncA_GPIOconfig);
    gpio_config(&EncB_GPIOconfig);
    gpio_config(&Solenoid_GPIOconfig);

    //Install gpio interrupt service
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(EncA_GPIO, &AMT20enc_isr_handler, NULL);
    gpio_isr_handler_add(EncB_GPIO, &AMT20enc_isr_handler, NULL);
    gpio_set_level(solenoid_gpio_pin,1);



    //Read abs encoder position and update
    do{
        sync_encoders();
        ets_delay_us(2000);
        sync_encoders();
    }
    while(AMT20_abs_position == -1);

    //Create threads to handle program tasks.
//    xTaskCreatePinnedToCore(&print_task, "print_task",configMINIMAL_STACK_SIZE,NULL,5,NULL,1);
    xTaskCreate(&print_task,"Print_Task",8192,NULL,1,NULL);
    xTaskCreate(&control_comm_task, "Comm task",8192,NULL, 2,NULL);
//    xTaskCreate(&spi_AMT_encoder_task,"Encoder Task",8192,NULL,5,NULL);



}


void print_task(void *novars) {
    bool RC_valid_flag = false;
    uint8_t RC_status = 0;


    for (;;) {

//        AMT20_abs_position = read_AMT20_position();
        AMT20_abs_position = Aencoder.readEncoderPosition();
        roboclaw_position = roboclaw.ReadEncM1(roboclaw_addr, &RC_status, &RC_valid_flag);

        printf("Abs. Pos: %i \t\t", AMT20_abs_position);
        printf("Quad Pos: %i \t\t", esp_quadenc_position);
        printf("Roboclaw Quad: %i \n", roboclaw_position);

//        esp_task_wdt_reset();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void control_comm_task(void* novars){

    while(1) {
        int inchar = 0;
        inchar = getchar();
        if (inchar != EOF) {
            printf("You sent: %c\n", inchar);
        }

        switch (inchar) {
            case 't':  //Re-calibrate encoders
                sync_encoders();
////                AMT20_abs_position = Aencoder.readEncoderPosition();
////                vTaskDelay(2/portTICK_PERIOD_MS);
//                AMT20_abs_position = Aencoder.readEncoderPosition();  //Why do we need to call twice?
//                esp_quadenc_position = AMT20_abs_position;
//                roboclaw.SetEncM1(roboclaw_addr,AMT20_abs_position);
                break;

            case 'a': // Go to cast start



                break;

            case 's': // Go to cast release location



                break;
            case 'd': // Go to cast finish location



                break;

            case 'p': // Cast, with second check of 'y'

                //Check if rod is within 20 counts of cast start, then delay and cast.
                if(abs(AMT20_abs_position - rod_cast_begin_pos) < 20){
                    printf("Casting!\n\n");
                }
                else
                {
                    printf("Not in position, no cast! \n\n");
                }

//                Serial.setTimeout(2000);
//                sanitycheck = Serial.parseInt();
//                if(sanitycheck == 1) {
//                    printf("Casting!\n\n");
//                    //fishing_rod_cast_sequence();
//                }

                break;

            case '=': //Fire solenoid for 1 second
                gpio_set_level(solenoid_gpio_pin,0);
                break;

            case 'u': //Reel for 30ms?
                roboclaw.DutyM2(roboclaw_addr, reel_pwm_drive);
                break;

            case 'q': //Advanced cast release 5 counts
                rod_cast_release_pos += 5;
                printf("Cast Release: %i\n", rod_cast_release_pos);
                break;

            case 'w': //Retard cast release 5 counts
                rod_cast_release_pos -= 5;
                printf("Cast Release: %i\n", rod_cast_release_pos);
                break;

            default:
                roboclaw.DutyM2(roboclaw_addr, 0);
                gpio_set_level(solenoid_gpio_pin,1);
                break;

        }

        vTaskDelay(30 / portTICK_PERIOD_MS); //Sleep thread for 30ms

    }


}




