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
#include <strings.h>
#include <esp_log.h>
#include <esp_system.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//NVS flash api includes.
#include <nvs_flash.h>
#include <nvs.h>

//Arduino specific includes.
#include <Arduino.h>
#include <SPI.h>

//Include third party libraries for specific program.
#include "RoboClaw.h"
#include "AMT20_ABSQUADENC_SPI.h"

//General tag for this project
#include "fish_prompts.h"
static const char* TAG = "Fishinator";



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
int32_t rod_cast_start_pos = 3050;
int32_t rod_cast_release_pos = 2300;
int32_t rod_cast_retrieve_pos = 1000;
const int32_t rod_hook_pos = 2000;
const int32_t rod_neutral_pos = 1500;
const int32_t rod_MAX_pos = 3400;
const int32_t rod_MIN_pos = 500;

const int32_t reel_pwm_drive = 0x4000;
const int32_t reel_pwm_drive_slow = 0x2000;
const int32_t rod_slew_pwm_value = 1000;

uint32_t rod_acc_decc_max = 50000;
uint32_t rod_acc_decc_min = 1000;

//Fishing rod state declarations
volatile boolean casting_semaphore = false;
int fishstate = 0;
int rod_manualmove_counter = 0;
int reel_counter = 0;
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
//    ets_delay_us(3000);
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
    printf("Roboclaw Quad: %i \t\t", roboclaw_position);
//    printf("RC Flag: %i  RC Status: %i \n", RC_valid_flag,RC_status);
    printf("\n");

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

/** Utility functions and inits
 *
 *  This includes some user input functions as well as an simple NVS storage implementation for storing casing params.
 */
uint32_t comm_loop_delay = 20; //in msecs.  Determines reel behavior, test solenoid behavior
void control_comm_task(void* novars); //Monitor incoming com task

void fish_nvs_storevals_task(void* novars);
nvs_handle my_nvs_handle;
const char * nvs_cast_start = "nvs_cast_start";  //Redundant?
const char * nvs_retrieve = "nvs_retrieve";
const char * nvs_release = "nvs_release";

int get_inputchar(){
    int cc;
    do {
        cc = getchar();
        vTaskDelay(5/portTICK_PERIOD_MS);
    } while (cc == EOF);

    return cc;
}

esp_err_t fish_nvs_write(){

    esp_err_t err;
    err = nvs_set_i32(my_nvs_handle,nvs_cast_start,rod_cast_start_pos);
    printf((err != ESP_OK) ? "wCS NVS Failed!\n" : "+");
    err = nvs_set_i32(my_nvs_handle,nvs_retrieve,rod_cast_retrieve_pos);
    printf((err != ESP_OK) ? "wRET NVS Failed!\n" : ".");
    err = nvs_set_i32(my_nvs_handle,nvs_release,rod_cast_release_pos);
    printf((err != ESP_OK) ? "wREL NVS Failed!\n" : "+\n");

    err = nvs_commit(my_nvs_handle);
    printf((err != ESP_OK) ? "wFailed!\n" : "wDone\n");

    return err;

};

esp_err_t fish_nvs_read(){

    esp_err_t err = 0;
    err = nvs_get_i32(my_nvs_handle,nvs_cast_start,&rod_cast_start_pos);
    printf((err != ESP_OK) ? "rCS NVS Failed!\n" : ".");
    err = nvs_get_i32(my_nvs_handle,nvs_retrieve,&rod_cast_retrieve_pos);
    printf((err != ESP_OK) ? "rRET NVS Failed!\n" : "+");
    err = nvs_get_i32(my_nvs_handle,nvs_release,&rod_cast_release_pos);
    printf((err != ESP_OK) ? "rREL NVS Failed!\n" : ".\n");

    return err;

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
    ets_delay_us(5000);
//    roboclaw.begin(230400);
    roboclaw.DutyM1(roboclaw_addr,0);
    roboclaw.DutyM2(roboclaw_addr,0);

    // Initialize NVS
    esp_err_t err = nvs_flash_init();
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
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
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
    xTaskCreate(&control_comm_task, "Comm task",8192,NULL, 5,NULL);


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


void control_comm_task(void* novars){
    static int32_t state_debounce_counter = 0;  // Debounce counter for state machine transisition, to keep from advancing too quickly.

    while(true) {
        int inchar = 0;
        inchar = getchar();
        if (inchar != EOF) {
            printf("%c - ", inchar);
//            printf("You sent: %c\n", inchar);
        }

        switch (inchar) {
            /** Setup and control cases **/
            case ' ': //STOP all motor functions!
                roboclaw.DutyM1(roboclaw_addr, 0);
                ets_delay_us(200);
                roboclaw.DutyM2(roboclaw_addr, 0);
                printf("\n\nSTOPPING!!\n\n");
                break;

            case '?':  //Print help text
                printf(fishp_splash);
                printf(fishp_intro);
                printf("Version: %i.%i-%i\n\n",FISH_MAJOR_REV,FISH_MINOR_REV,FISH_DATE_REV);
                printf(fishp_help);
                printf("\nCURRENT STATE: %i\n\n",fishstate);
                break;

            case 'y':  //Print each encoder value
                print_encoder_info();
                break;

            case 't':  //Re-sycronize encoders to abs value.
                fish_nvs_read();
                ets_delay_us(1000);
                sync_encoders();
                ets_delay_us(5000);
                print_encoder_info();
                if (fishstate == 0) fishstate = 2;
                break;

            case '|':  //Set encoder zero, and sync encoders: NOTE Should be rod straight down.
                roboclaw.DutyM1(roboclaw_addr, 0);
                printf("Are you sure you want to overwrite encoder zero (Should be straight down)?\nPress 'y' to confirm: ");
                inchar = get_inputchar();
                putchar(inchar);
                if (inchar == 'y'){
                    Aencoder.setEncoderZero();
                    ets_delay_us(10000);
                    sync_encoders();
                    ets_delay_us(10000);
                    print_encoder_info();
                }
                else printf(" - Cancel!\n");

                printf(" \n");
                break;

            case '=': //Fire solenoid for 30 ms, hold for longer
                gpio_set_level(solenoid_gpio_pin,0);
                vTaskDelay(30);
                break;

            case ':': //Reset start and finish locations manually
                roboclaw.DutyM1(roboclaw_addr, 0);
                printf("Position the rod in the CAST START location and press 'y', press any other key to skip: ");
                inchar = get_inputchar();
                putchar(inchar);
                if(inchar == 'y'){
                    sync_encoders();
                    ets_delay_us(5000);
                    rod_cast_start_pos = Aencoder.readEncoderPosition();
                    printf("\nNew CAST START location = %i\n",rod_cast_start_pos);
                }
                else printf(" - SKIP!\n");
                printf("\nPosition the rod in the RETRIEVE location and press 'y', press any other key to skip: ");
                inchar = get_inputchar();
                putchar(inchar);
                if(inchar == 'y'){
                    sync_encoders();
                    ets_delay_us(5000);
                    rod_cast_retrieve_pos = Aencoder.readEncoderPosition();
                    printf("\nNew RETRIEVE location = %i\n",rod_cast_retrieve_pos);
                }
                else printf(" - SKIP!\n");

                printf("\n\n Saving cast start, retrieve and release to non-volitile storage \n\n");
                fish_nvs_write(); //Save new values to NV flash.

                break;

                ///Quadstick Mappings: See control document Fishing_Controls_StatMachine_dd.txt for other controls
                /// Center SNP keymap
            case 'a':  //Center Puff Hard ->  Advance one state
            case 'c':  //Center Sip Hard  ->  Retard one state
                if(state_debounce_counter <=0){
                    state_debounce_counter = 10;  //Delay this * comm_loop_delay msecs.
                    fishstate = fishStateMachine(inchar, fishstate);
                }
                else printf("Debounce state - You can only change state every 0.5 seconds.\n");

                break;

            case 'b':  //Center Puff Soft
                break;

            case 'e':  //Center Sip Soft  IMPLEMENTED ABOVE
                break;

                /// Right SNP keymap
            case 'f':  //Right Puff Hard  -> Cast speed decrease
                rod_CASTstart_move_params.accel = constrain((rod_CASTstart_move_params.accel - 1000), rod_acc_decc_min, rod_acc_decc_max);
                rod_CASTstart_move_params.decel = constrain((rod_CASTstart_move_params.decel - 1000), rod_acc_decc_min, rod_acc_decc_max);
                printf("New cast speed: %i\n", rod_CASTstart_move_params.accel);

                break;

            case 'g':  //Right Puff Soft
                break;

            case 'h':  //Right Sip Hard   -> Release retard
                rod_cast_release_pos = constrain((rod_cast_release_pos+5),rod_MIN_pos,rod_MAX_pos);
                printf("New cast release position: %i\n", rod_cast_release_pos);

                break;

            case 'i':  //Right Sip Soft
                break;

                /// Left SNP keymap
            case 'j':  //Left Puff Hard  -> Cast speed increase
                rod_CASTstart_move_params.accel = constrain((rod_CASTstart_move_params.accel + 1000), rod_acc_decc_min, rod_acc_decc_max);
                rod_CASTstart_move_params.decel = constrain((rod_CASTstart_move_params.decel + 1000), rod_acc_decc_min, rod_acc_decc_max);
                printf("New cast speed: %i\n", rod_CASTstart_move_params.accel);

                break;

            case 'k':  //Left Puff Soft
                break;

            case 'm':  //Left Sip Hard  -> Release position advance.
                rod_cast_release_pos = constrain((rod_cast_release_pos - 5),rod_MIN_pos,rod_MAX_pos);
                printf("New cast release position: %i\n", rod_cast_release_pos);

                break;

            case 'n':  //Left Sip Soft
                break;

                /// Joystick keymap
            case 'u':  //Joystick up:  Reel for 30ms
                roboclaw.DutyM2(roboclaw_addr, reel_pwm_drive);
                reel_counter = 4;
                break;

            case 'd':  //Joystick down: Reel for 30ms(go slower?
                roboclaw.DutyM2(roboclaw_addr, reel_pwm_drive_slow);
                reel_counter = 4;
                break;

            case 'x':
            case 'l':  //Joystick left: Move rod forward.
                roboclaw.DutyM1(roboclaw_addr, -rod_slew_pwm_value);
//                roboclaw.SpeedM1(roboclaw_addr,-100);
                rod_manualmove_counter = 4;  //Flag to prevent other control loop moves from being screwed with
                break;

            case 'z':
            case 'r':  //Joystick right: Move rod backwards.
                roboclaw.DutyM1(roboclaw_addr, rod_slew_pwm_value);
//                roboclaw.SpeedM1(roboclaw_addr,100);
                rod_manualmove_counter = 4;  //Flag to prevent other control loop moves from being screwed with
                break;

            case 'p':  //Lip switch:  Positive test for cast.
                break;


            default:
                /// Kludgy logic to simulate smooth rod movement when using left/right controls
                if(reel_counter > 1) {
                    reel_counter--;
                }
                else if (reel_counter == 1){
                    roboclaw.DutyM2(roboclaw_addr, 0);
                    reel_counter = 0;
                }
                else{reel_counter = 0;}

                /// Kludgy logic to simulate smooth rod movement when using left/right controls
                if(rod_manualmove_counter > 1) {
                    rod_manualmove_counter--;
                }
                else if (rod_manualmove_counter == 1){
                    roboclaw.DutyM1(roboclaw_addr, 0);
                    rod_manualmove_counter = 0;
                }
                else{rod_manualmove_counter = 0;}

                /// And make sure solenoid is off.
                gpio_set_level(solenoid_gpio_pin,1);
                break;

        }

        (state_debounce_counter <=0)?(state_debounce_counter = 0):state_debounce_counter--; //Decrement debounce or hold at 0;

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
//    goto_rod_position(&rod_CASTfinish_move_params,rod_cast_retrieve_pos,1,1);  //Start Casting
    goto_rod_position(&rod_slow_move_params,rod_cast_retrieve_pos,0,1);  //Start Casting
    vTaskDelay(2000/portTICK_PERIOD_MS);
//    fish_nvs_write();  //Store casting information to nv flash, that way a well tuned cast is preserved.
//    printf("Speed(cps): %i\n",M1speed);  //Test release speed printout
//    printf("Dur(us): %i\n",static_cast<uint32_t>(cast_duration_timer));
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
    int skipprint = 0;
    int cc;
    static const char* subTAG = "FSM";
    char nextstate_text[25];
    char astate_text[25];
    char cstate_text[25];


//    printf("FSkey: %c  FSstate: %i \n", key,state);

    if(key == 'a'){
//        Serial.print("Puff Advance");
        switch(state){
            case 2: //Neutral go to cast start
                nextstate = 8;
                strcpy(nextstate_text, "CAST START");
                strcpy(astate_text, "CAST!!");
                strcpy(cstate_text, "NEUTRAL");

                goto_rod_position(&rod_slow_move_params,rod_cast_start_pos,1,1);
                break;

            case 8: //Cast start, cast if success, go to retrieve
                if(abs(esp_quadenc_position - rod_cast_start_pos) <= 40) {
                    printf("Hit 'b'or 'p', CENTER PUFF SOFT or LIP SWITCH to cast, any other to cancel\n");
//                    do {
//                        cc = getchar();
//                    } while (cc == EOF);
                    cc = get_inputchar();

                    //Encoder sync for sanity... or insanity?
                    sync_encoders();
                    ets_delay_us(5000);
                    sync_encoders();
                    ets_delay_us(5000);

                    if (cc == 'b' || cc == 'p') {
                        printf("\n\nCASTING!!!!!!!\n\n");
                        nextstate = 4;
                        strcpy(nextstate_text, "RETRIEVE");
                        strcpy(astate_text, "NEUTRAL");
                        strcpy(cstate_text, "HOOK");
                        fishing_rod_cast_sequence();
                        break;
                    } else {
                        nextstate = 8;
                        printf("\nWrong confirmation, no cast! Stay in CAST START\n\n");
                        skipprint = 1;
                    }
                }
                else{
                    nextstate = 8;
                    printf("\nOUT OF SYNC: Start location should be: %i and is currently %i +/-30\n", rod_cast_start_pos,Aencoder.readEncoderPosition());
                    printf("Re-sync with 't' and try casting again! \n");
                    skipprint = 1;

                }
//                strcpy(nextstate_text, "CAST START");
//                strcpy(astate_text, "CAST!!!");
//                strcpy(cstate_text, "NEUTRAL");
                break;

            case 4:  //Retrieve, go to neutral
                nextstate = 2;
                strcpy(nextstate_text, "NEUTRAL");
                strcpy(astate_text, "CAST START");
                strcpy(cstate_text, "RETRIEVE");

                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;

            case 11: //Hook position go to neutral
                nextstate = 2;
                strcpy(nextstate_text, "NEUTRAL");
                strcpy(astate_text, "CAST START");
                strcpy(cstate_text, "RETRIEVE");

                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;
            case 0:
                printf("State is 0, you haven't synced encoders, press 't'\n");
                skipprint = 1;
                break;

            default:
                break;
        }
    }
    else if (key == 'c'){
        Serial.println("Sip Return");
        switch(state){
            case 2: //Neutral, go to retrieve
                nextstate = 4;
                strcpy(nextstate_text, "RETRIEVE");
                strcpy(astate_text, "NEUTRAL");
                strcpy(cstate_text, "HOOK!");

                goto_rod_position(&rod_slow_move_params,rod_cast_retrieve_pos,1,1);
                break;

            case 8: //Cast start, go to neutral
                nextstate = 2;
                strcpy(nextstate_text, "NEUTRAL");
                strcpy(astate_text, "CAST START");
                strcpy(cstate_text, "RETRIEVE");

                goto_rod_position(&rod_slow_move_params,rod_neutral_pos,1,1);
                break;

            case 4:  //Retrieve go to HOOK!
                nextstate = 11;
                strcpy(nextstate_text, "HOOK!");
                strcpy(astate_text, "NEUTRAL");
                strcpy(cstate_text, "RETRIEVE");

                goto_rod_position(&rod_CASTstart_move_params,rod_hook_pos,1,1);
                vTaskDelay(1500/portTICK_PERIOD_MS);
                break;

            case 11: //Hook position go to retrieve
                nextstate = 11;
                strcpy(nextstate_text, "HOOK!");
                strcpy(astate_text, "NEUTRAL");
                strcpy(cstate_text, "Nothing.");
                printf( "You must back out of hook with 'a'. Saftey first.\n");
                skipprint = 1;

//                goto_rod_position(&rod_slow_move_params,rod_cast_retrieve_pos,1,1);
                break;
            case 0:
                printf("State is 0, you haven't synced encoders, press 't'\n");
                skipprint = 1;
                break;

            default:
                break;
        }


    }
    else {printf("ERROR: Fish Stat Machine no case \n");} //Do nothing, however shouldnt reach here.

//    ESP_LOGI(subTAG,"CurrentState: %i, NextState: %i",state, nextstate);
    if (!skipprint) {
        printf("  Going to state: ");
        printf(nextstate_text);
        printf(", 'a' will then take you to: ");
        printf(astate_text);
        printf(", 'c' will take you to: ");
        printf(cstate_text);
        printf(". \n\n");
    }

    return nextstate;

}