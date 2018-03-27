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

//General tag for this project
#include "fish_prompts.h"
static const char* TAG = "Fishinator";

//Control and utility functions, mostly attached as tasks/threads to the RTOS scheduler.
uint32_t comm_loop_delay = 50; //in msecs.  Determines reel behavior, test solenoid behavior
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


/**Fishing rod position variables **/
const int32_t rod_cast_begin_pos = 3300;
int32_t rod_cast_release_pos = 2300;
const int32_t rod_cast_finish_pos = 1000;
const int32_t rod_hook_pos = 2000;
const int32_t rod_neutral_pos = 1500;
const int32_t rod_MAX_pos = 3400;
const int32_t rod_MIN_pos = 850;

const int32_t reel_pwm_drive = 0x4000;
const int32_t reel_pwm_drive_slow = 0x2000;
const int32_t rod_slew_pwm_value = 1000;

uint32_t rod_acc_decc_max = 50000;
uint32_t rod_acc_decc_min = 1000;

//Fishing rod state declarations
volatile boolean casting_semaphore = false;
int fishstate = 0;
char manualmove = 0;
int fishStateMachine(int key, int state);

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
    ets_delay_us(100);
    roboclaw_position = roboclaw.ReadEncM1(roboclaw_addr, &RC_status, &RC_valid_flag);
    vTaskDelay(3/portTICK_PERIOD_MS);  //See

    printf("Abs. Pos: %i \t\t", AMT20_abs_position);
    printf("Quad Pos: %i \t\t", esp_quadenc_position);
//    printf("Roboclaw Quad: %i \t\t", roboclaw_position);
//    printf("RC Flag: %i  RC Status: %i \n", RC_valid_flag,RC_status);

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
    roboclaw.SetM1VelocityPID(roboclaw_addr,0,0,0,rod_slow_move_params.qpps);  //QPPS apparently needs to be set even if not using VPID
    ets_delay_us(2000);
    roboclaw.SetM1PositionPID(roboclaw_addr,rod_slow_move_params.pid_P,rod_slow_move_params.pid_I,
                              rod_slow_move_params.pid_D, rod_slow_move_params.pid_maxI,
                              rod_slow_move_params.deadzone, rod_slow_move_params.min_position,
                              rod_slow_move_params.max_position);
    ets_delay_us(2000);


    //System Init should be complete: Print welcome:
    printf(fishp_intro);
    printf("Version: %i.%i\n\n",FISH_MAJOR_REV,FISH_MINOR_REV);
    printf(fishp_help);
//    esp_log_write(ESP_LOG_INFO,TAG,fishp_intro); //Should not be used according to the library, use log macro.
//    ESP_LOGI(TAG,fishp_test);

    /// Create threads to handle program tasks.
//    xTaskCreatePinnedToCore(&print_task, "print_task",configMINIMAL_STACK_SIZE,NULL,5,NULL,1);
    xTaskCreate(&control_comm_task, "Comm task",8192,NULL, 2,NULL);
//    xTaskCreate(&spi_AMT_encoder_task,"Encoder Task",8192,NULL,5,NULL);

}


void control_comm_task(void* novars){

    while(1) {
        int inchar = 0;
        inchar = getchar();
        if (inchar != EOF) {
            printf("You sent: %c\n", inchar);
        }

        switch (inchar) {
            /** Setup and control cases **/
            case '?':
                printf(fishp_help);
                break;

            case 'y':  //Print each encoder value
                print_encoder_info();
                break;

            case 't':  //Re-sycronize encoders to abs value.
                sync_encoders();
                ets_delay_us(100);
                print_encoder_info();
                fishstate = 2;
                break;

            case '|':  //Set encoder zero, and sync encoders: NOTE Should be rod straight down.
                Aencoder.setEncoderZero();
                ets_delay_us(5000);
                sync_encoders();
                ets_delay_us(5000);
                print_encoder_info();
                break;

            case '=': //Fire solenoid for 30 ms, hold for longer
                gpio_set_level(solenoid_gpio_pin,0);
                break;

                ///Quadstick Mappings: See control document Fishing_Controls_StatMachine_dd.txt for other controls
                /// Center SNP keymap
            case 'a':  //Center Puff Hard ->  Advance one state
                fishstate = fishStateMachine(inchar, fishstate);
                break;

            case 'b':  //Center Puff Soft
                break;
            case 'c':  //Center Sip Hard  ->  Retard one state
                fishstate = fishStateMachine(inchar, fishstate);
                break;

            case 'e':  //Center Sip Soft
                break;

                /// Right SNP keymap
            case 'f':  //Right Puff Hard  -> Cast speed increase
                rod_CASTstart_move_params.accel = constrain((rod_CASTstart_move_params.accel + 1000), rod_acc_decc_min, rod_acc_decc_max);
                rod_CASTstart_move_params.decel = constrain((rod_CASTstart_move_params.decel + 1000), rod_acc_decc_min, rod_acc_decc_max);
                printf("Cast Speed: %i\n", rod_CASTstart_move_params.accel);

                break;

            case 'g':  //Right Puff Soft
                break;

            case 'h':  //Right Sip Hard   -> Release Advanced
                rod_cast_release_pos = constrain((rod_cast_release_pos-5),rod_MIN_pos,rod_MAX_pos);
                printf("Cast Release: %i\n", rod_cast_release_pos);

                break;

            case 'i':  //Right Sip Soft
                break;

                /// Left SNP keymap
            case 'j':  //Left Puff Hard  -> Cast speed decrease
                rod_CASTstart_move_params.accel = constrain((rod_CASTstart_move_params.accel - 1000), rod_acc_decc_min, rod_acc_decc_max);
                rod_CASTstart_move_params.decel = constrain((rod_CASTstart_move_params.decel - 1000), rod_acc_decc_min, rod_acc_decc_max);
                printf("Cast Speed: %i\n", rod_CASTstart_move_params.accel);

                break;

            case 'k':  //Left Puff Soft
                break;

            case 'm':  //Left Sip Hard  -> Release position retard.
                rod_cast_release_pos = constrain((rod_cast_release_pos+5),rod_MIN_pos,rod_MAX_pos);
                printf("Cast Release: %i\n", rod_cast_release_pos);

                break;

            case 'n':  //Left Sip Soft
                break;

                /// Joystick keymap
            case 'u':  //Joystick up:  Reel for 30ms
                roboclaw.DutyM2(roboclaw_addr, reel_pwm_drive);
                break;

            case 'd':  //Joystick down: Reel for 30ms(go slower?
                roboclaw.DutyM2(roboclaw_addr, reel_pwm_drive_slow);
                break;

            case 'l':  //Joystick left: Move rod forward.
//                roboclaw.DutyM1(roboclaw_addr, -rod_slew_pwm_value);
//                manualmove = 1;  //Flag to prevent other control loop moves from being screwed with
                break;

            case 'r':  //Joystick right: Move rod backwards.
//                roboclaw.DutyM1(roboclaw_addr, rod_slew_pwm_value);
//                manualmove = 1;  //Flag to prevent other control loop moves from being screwed with
                break;

            case 'p':  //Lip switch:  Positive test for cast.
                break;


            default:
                roboclaw.DutyM2(roboclaw_addr, 0);
//                if(manualmove){ roboclaw.DutyM1(roboclaw_addr, 0);}
//                else manualmove = 0;
                gpio_set_level(solenoid_gpio_pin,1);
                break;

        }

        vTaskDelay(comm_loop_delay / portTICK_PERIOD_MS); //Sleep thread for 30ms

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

int fishStateMachine(int key, int state){
    int nextstate = 0;
    int exitwait = 0;
    int cc;
    static const char* subTAG = "FSM";


//    printf("FSkey: %c  FSstate: %i \n", key,state);

    if(key == 'a'){
//        Serial.print("Puff Advance");
        switch(state){
            case 2: //Precast, retrieve complete
                nextstate = 8;
//                Serial.println("Puff, 2");
//                myRod.gotoPosition(caststart, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_cast_begin_pos,1,1);
                break;

            case 8: //Cast start

                if(abs(AMT20_abs_position - rod_cast_begin_pos) <= 500) {
                    printf("Hit 'p', lip switch to cast, any other to cancel\n");
                    do {
                        cc = getchar();
                    } while (cc == EOF);

                    if (cc == 'p') {
                        printf("\nCasting!\n");
                        nextstate = 4;
                        fishing_rod_cast_sequence();
                    } else {
                        nextstate = 8;
                        printf("\nWrong sequence, no cast! \n\n");
                    }
                }
                else{
                    printf("Not in cast start position, re-sync with 't'! \n");
                    printf("Start location should be: %i give or take 30.\n", rod_cast_begin_pos);
                }
                break;

//                Serial.println("Puff, 8");
//                //Cast here;
////				myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
////
//                Serial.print(F("Hit lip switch to cast"));
//                while (!exitwait) {
//                    if (Serial.available()) {
//                        cc = Serial.read();
//                        if (cc == 'p' && myRod.isEncoderSafe()) {
//                            Serial.println("CASTING!!!!!");
//                            nextstate = 4;
//                            CAST(castacc, castdec, castspeed);
//                            cursp = myRod.getPositionCurrent();
//
//
//                        } else {
//                            nextstate = 8;
//                            Serial.println(F("Encoder not safe! NO CAST"));
//                            Serial.print(F("EnVal = "));
//                            Serial.println(myRod.isEncoderSafe());
//                            Serial.println(cc);
//                        }
//                        exitwait = 1;
//                    }
//                }
//                break;

            case 4:  //Cast complete, retreive position
                nextstate = 2;
//                Serial.println("Sip, 4");
//                myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;

            case 11: //Hook position
                nextstate = 2;
//                Serial.println("Puff, 11");
//                myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;
            case 0:
                printf("State is 0, you haven't synced encoders\n");
                break;

            default:
                break;
        }
    }
    else if (key == 'c'){
        Serial.println("Sip Return");
        switch(state){
            case 2: //Precast, retrieve complete
                nextstate = 4;
//                Serial.println("Sip, 2");
//                myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_cast_finish_pos,1,1);
                break;

            case 8: //Cast start
                nextstate = 2;
//                Serial.println("Sip, 8");
//                myRod.gotoPosition(reelfinish, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;

            case 4:  //Hook!
//                Serial.println("Sip, 4");
                nextstate = 11;
                //Hook here more violently
//                myRod.gotoPosition(hooklocation,10,10, 15,0,0);
////				myRod.gotoPosition(hooklocation, accel, decel, velmax,0,1);
//                myRod.setMotorPWM(Brake,1023);
                goto_rod_position(&rod_CASTstart_move_params,rod_hook_pos,1,1);
                break;

            case 11: //Hook position
                nextstate = 4;
//                Serial.println("Sip, 11");
//                myRod.gotoPosition(castfinish, accel, decel, velmax,0,1);
                goto_rod_position(&rod_slow_move_params,rod_cast_finish_pos,1,1);
                break;
            case 0:
                printf("State is 0, you haven't synced encoders\n");
                break;

            default:
                break;
        }

    }
    else {printf("Fish Stat Machine no case \n");} //Do nothing, however shouldnt reach here.

    ESP_LOGI(subTAG,"CurrentState: %i, NextState: %i",state, nextstate);

    return nextstate;

}