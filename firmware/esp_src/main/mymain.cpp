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
//DRAM_ATTR int16_t AMT20_abs_position = -1;
//DRAM_ATTR int32_t roboclaw_position = 0;
int16_t AMT20_abs_position = -1;
int32_t roboclaw_position = 0;
volatile long cast_duration_timer = 0;

/** Roboclaw setup **/
#define roboclaw_addr 0x80
HardwareSerial HWSerial(1);
RoboClaw roboclaw(&HWSerial,10000);


/**Fishing rod testing variables **/
const int32_t rod_cast_begin_pos = 3300;
int32_t rod_cast_release_pos = 2300;
const int32_t rod_cast_finish_pos = 1000;
const int32_t rod_hook_pos = 2000;
const int32_t rod_neutral_pos = 1500;
const int32_t reel_pwm_drive = 0x4000;
const int32_t rod_MAX_pos = 3400;
const int32_t rod_MIN_pos = 850;

volatile boolean casting_semaphore = false;

typedef struct{
  uint32_t pid_P;
  uint32_t pid_I;
  uint32_t pid_D;
  uint32_t pid_maxI;
  uint32_t deadzone;
  uint32_t min_position;
  uint32_t max_position;
  uint32_t speed;
  uint32_t accel;
  uint32_t decel;
  uint32_t qpps;
}rod_control_params_t;

rod_control_params_t rod_slow_move_params ={4000, 4, 8000, 50, 20, rod_MIN_pos, rod_MAX_pos, 400, 800, 800,16000};
rod_control_params_t rod_CASTstart_move_params ={10000, 40, 80000, 1000, 20, rod_MIN_pos, rod_MAX_pos, 15000, 20000, 30000,16000};
//rod_control_params_t rod_CASTfinish_move_params ={8000, 10, 20000, 200, 20, rod_MIN_pos, rod_MAX_pos, 500, 20000};

/**Fishing rod control functions, encoder manip functions **/
void fishing_rod_cast_sequence();
void goto_rod_position(rod_control_params_t* params, int16_t destination, boolean override_current_move = true, boolean update_params = true);

void sync_encoders(){
    AMT20_abs_position = Aencoder.readEncoderPosition();
    esp_quadenc_position = AMT20_abs_position;
    roboclaw.SetEncM1(roboclaw_addr,AMT20_abs_position);
}

void print_encoder_info(){
    bool RC_valid_flag = false;
    uint8_t RC_status = 0;

    AMT20_abs_position = Aencoder.readEncoderPosition();
    roboclaw_position = roboclaw.ReadEncM1(roboclaw_addr, &RC_status, &RC_valid_flag);
    vTaskDelay(3/portTICK_PERIOD_MS);

    printf("Abs. Pos: %i \t\t", AMT20_abs_position);
    printf("Quad Pos: %i \t\t", esp_quadenc_position);
    printf("Roboclaw Quad: %i \t\t", roboclaw_position);
    printf("RC Flag: %i  RC Status: %i \n", RC_valid_flag,RC_status);

}

/** Solenoid GPI Config **/
const gpio_num_t solenoid_gpio_pin = GPIO_NUM_19;
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

// Set GPIO for encoder counting.
    gpio_config(&EncA_GPIOconfig);
    gpio_config(&EncB_GPIOconfig);
    gpio_config(&Solenoid_GPIOconfig);

    //Install gpio interrupt service
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(EncA_GPIO, &AMT20enc_isr_handler, NULL);
    gpio_isr_handler_add(EncB_GPIO, &AMT20enc_isr_handler, NULL);
    gpio_set_level(solenoid_gpio_pin,1);

    /** Trying arduino spi library **/
    SPI.begin(14,12,13,15);
    roboclaw.begin(460800);
//    roboclaw.begin(230400);



    //Read abs encoder position and update
    do{
        sync_encoders();
        ets_delay_us(2000);
        sync_encoders();
    }
    while(AMT20_abs_position == -1);
    roboclaw.SetM1VelocityPID(roboclaw_addr,1,1,1,rod_slow_move_params.qpps);
    ets_delay_us(2000);
    roboclaw.SetM1PositionPID(roboclaw_addr,rod_slow_move_params.pid_P,rod_slow_move_params.pid_I,
                              rod_slow_move_params.pid_D, rod_slow_move_params.pid_maxI,
                              rod_slow_move_params.deadzone, rod_slow_move_params.min_position,
                              rod_slow_move_params.max_position);
    ets_delay_us(2000);


    //Create threads to handle program tasks.
//    xTaskCreatePinnedToCore(&print_task, "print_task",configMINIMAL_STACK_SIZE,NULL,5,NULL,1);
//    xTaskCreate(&print_task,"Print_Task",8192,NULL,1,NULL);
    xTaskCreate(&control_comm_task, "Comm task",8192,NULL, 2,NULL);
//    xTaskCreate(&spi_AMT_encoder_task,"Encoder Task",8192,NULL,5,NULL);



}


void print_task(void *novars) {
//    bool RC_valid_flag = false;
//    uint8_t RC_status = 0;


    for (;;) {
        bool RC_valid_flag = false;
        uint8_t RC_status = 0;
//        AMT20_abs_position = read_AMT20_position();
        AMT20_abs_position = Aencoder.readEncoderPosition();
        ets_delay_us(30);
        roboclaw_position = roboclaw.ReadEncM1(roboclaw_addr, &RC_status, &RC_valid_flag);

        printf("Abs. Pos: %i \t\t", AMT20_abs_position);
        printf("Quad Pos: %i \t\t", esp_quadenc_position);
        printf("Roboclaw Quad: %i \t\t", roboclaw_position);
        printf("RC Flag: %i  RC Status: %i \n", RC_valid_flag,RC_status);


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
                ets_delay_us(100);
                print_encoder_info();
                break;
            case 'r':  //Print encoder info
                print_encoder_info();
                break;

            case 'a': // Go to cast start
                printf("Destination: %i \n", rod_cast_begin_pos);
                goto_rod_position(&rod_slow_move_params, rod_cast_begin_pos,1,1);
                break;

            case 's': // Go to cast release location
                printf("Destination: %i \n", rod_cast_release_pos);
                goto_rod_position(&rod_slow_move_params, rod_cast_release_pos,1,1);
                break;
            case 'd': // Go to cast finish location
                printf("Destination: %i \n", rod_cast_finish_pos);
                goto_rod_position(&rod_slow_move_params, rod_cast_finish_pos,1,1);
//                goto_rod_position(&rod_CAST_move_params, rod_cast_finish_pos);
                break;

            case 'p': // Cast, with second check of 'y'
                if(abs(AMT20_abs_position - rod_cast_begin_pos) <= 100) {
                    do {
                        inchar = getchar();
                    } while (inchar == EOF);

                    if (inchar == 'z') {
                        printf("Casting!\n\n");
                        fishing_rod_cast_sequence();
                    } else {
                        printf("Wrong sequence, no cast! \n\n");
                    }
                }
                else printf("Not in cast start position,  no cast! \n\n");

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
            case 'n': //Advanced accell 1000 qpps
                rod_CASTstart_move_params.accel += 1000;
                rod_CASTstart_move_params.decel += 1000;
                printf("Cast Accel: %i\n", rod_CASTstart_move_params.accel);
                break;

            case 'm': //Deccel 1000 qpps
                rod_CASTstart_move_params.accel -= 1000;
                rod_CASTstart_move_params.decel -= 1000;
                printf("Cast Accel: %i\n", rod_CASTstart_move_params.accel);
                break;
            case 'z':  //Roboclaw energency stop?
            case 'x':
            case ' ':

//            case '?':  //Set encoder zero, and sync encoders
//                Aencoder.setEncoderZero();
//                ets_delay_us(100);
//                sync_encoders();
//                ets_delay_us(100);
//                print_encoder_info();
//                break;

            default:
//                putchar('+');
                roboclaw.DutyM2(roboclaw_addr, 0);
                gpio_set_level(solenoid_gpio_pin,1);
                break;

        }

        vTaskDelay(30 / portTICK_PERIOD_MS); //Sleep thread for 30ms

    }


}



void goto_rod_position(rod_control_params_t* params, int16_t destination, boolean override_current_move, boolean update_params){
    bool RC_valid_flag = false;
    uint8_t RC_status = 0;

    if(update_params) roboclaw.SetM1PositionPID(roboclaw_addr,params->pid_P,params->pid_I,params->pid_D,
                              params->pid_maxI,params->deadzone, params->min_position,params->max_position);

    vTaskDelay(3/portTICK_PERIOD_MS);
//    ets_delay_us(20);
    roboclaw.SpeedAccelDeccelPositionM1(roboclaw_addr,params->accel,params->speed, params->decel, destination, override_current_move);
}

void fishing_rod_cast_sequence(){
    //Some sort of enable flag and/or semaphore take?
    bool RC_valid_flag = false;
    uint8_t RC_status = 0;
    int32_t M1speed = 0;

    sync_encoders();  //Sync encoders
//    vTaskDelay(100/portTICK_PERIOD_MS);  //Yield for encoder sync to finish
    gpio_set_level(solenoid_gpio_pin,0); //Low Active.
    vTaskDelay(200/portTICK_PERIOD_MS);  //Yield for encoder sync to finish and solenoid to engage

    goto_rod_position(&rod_CASTstart_move_params,rod_neutral_pos,1,1);  //Start Casting
    cast_duration_timer = micros();
    vTaskDelay(100/portTICK_PERIOD_MS); //Yield for a while, while the motor gets going.

    casting_semaphore = true;  //Set semaphore flag which will trigger a comparison in Esp encoder ISR to release solenoid
    while(casting_semaphore) {
//        putchar('.');
//        vTaskDelay(1 / portTICK_PERIOD_MS);  //Block(yield cpu) while waiting for solenoid release.
        ets_delay_us(50);
    }
    gpio_set_level(solenoid_gpio_pin,1); //Release solenoid
    M1speed = roboclaw.ReadSpeedM1(roboclaw_addr,&RC_status,&RC_valid_flag);
    vTaskDelay(1200/portTICK_PERIOD_MS);
//    goto_rod_position(&rod_CASTfinish_move_params,rod_cast_finish_pos,1,1);  //Start Casting
    goto_rod_position(&rod_slow_move_params,rod_cast_finish_pos,0,1);  //Start Casting
    vTaskDelay(1000/portTICK_PERIOD_MS);
    printf("Speed(cps): %i\n",M1speed);  //Test release speed printout
    printf("Dur(us): %i\n",static_cast<uint32_t>(cast_duration_timer));
}

void IRAM_ATTR AMT20enc_isr_handler(void* args){
    static DRAM_ATTR int8_t lookup_table[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
    static DRAM_ATTR int enc_val = 0;
    enc_val = enc_val << 2;
    enc_val = enc_val | ((gpio_get_level(EncA_GPIO) << 1) + gpio_get_level(EncB_GPIO));
    esp_quadenc_position = esp_quadenc_position + lookup_table[enc_val & 0b1111];

    if(casting_semaphore){
        if(esp_quadenc_position == rod_cast_release_pos){
            casting_semaphore = 0;
            gpio_set_level(solenoid_gpio_pin,1);
            cast_duration_timer = (cast_duration_timer- micros());
        }
    }

}

