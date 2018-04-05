/***************************
    fish_CnC.h
    
    Purpose: Command and control for the Fishinator. This includes
     initialization and communication of peripherals, ISRs, and state machines.

    Peripherals include:
        - AMT20 Absolute Encoder via SPI (VSPI)
        - Roboclaw ION MC Motor Controller via UART 1
        - Solenoid Via GPIO




        Created by Patrick Wagner on 4/5/18.

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

#ifndef CH_FISHINATOR_ESP_FISH_CNC_H
#define CH_FISHINATOR_ESP_FISH_CNC_H

/** INIT AMT20_ABSQUADSPI Class Setup **/
#define PIN_MOSI_NUM  GPIO_NUM_13
#define PIN_MISO_NUM  GPIO_NUM_12
#define PIN_SCK_NUM  GPIO_NUM_14
#define PIN_CS_NUM  GPIO_NUM_15
#define PIN_ENCA_NUM GPIO_NUM_23
#define PIN_ENCB_NUM GPIO_NUM_18

//Encoder Class Instatiation.
extern AMT20_ABSQUADENC_SPI AMTencoder;
extern int16_t AMT20_abs_position;  //Local variable of abs position
extern int32_t roboclaw_position;     //Local var of RC quad posistion.
extern volatile long cast_duration_timer;

/** Roboclaw setup **/
#define roboclaw_addr 0x80
extern HardwareSerial HWSerial;
extern RoboClaw roboclaw;

/**Fishing rod position variables **/
extern int32_t rod_cast_start_pos;
extern int32_t rod_cast_release_pos;
extern int32_t rod_cast_retrieve_pos;
extern const int32_t rod_hook_pos;
extern const int32_t rod_neutral_pos;
extern const int32_t rod_MAX_pos;
extern const int32_t rod_MIN_pos;

///Direct control drive parameters.
extern const int32_t reel_pwm_drive;  //32bit value for Reel PWM Drive
extern const int32_t reel_pwm_drive_slow; //32bit value for slower Reel PWM Drive
extern const int32_t rod_slew_pwm_value; //32bit Value for slow reel slew

///Limits for acceleration paramters.
extern uint32_t rod_acc_decc_max;
extern uint32_t rod_acc_decc_min;

///Fishing cnc declarations
extern volatile boolean casting_semaphore;
extern int fishstate;  //State variable to indicate which state we are in
extern int rod_manualmove_counter; //A debouce counter for rod movement
extern int reel_counter;   //Debounce counter for reel movement.
extern uint32_t comm_loop_delay;

/** Solenoid GPI0 Config **/
extern const gpio_num_t solenoid_gpio_pin;
extern const gpio_config_t Solenoid_GPIOconfig;


//Fishing motion control Parameters
/**
 *  rod_control_params_t - Roboclaw Motion Control params
 *
 *  Used in conjuction with go_to_position();
 *
 *  @param Contains of the motion control variables.
 */
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

//Slew and castin motion control paramters.
extern rod_control_params_t rod_slow_move_params;
extern rod_control_params_t rod_CASTstart_move_params;

extern nvs_handle my_nvs_handle;
static const char * nvs_cast_start = "nvs_cast_start";  //Redundant?
static const char * nvs_retrieve = "nvs_retrieve";
static const char * nvs_release = "nvs_release";


/**Fishing rod control functions, encoder manip functions **/
int fishStateMachine(int key, int state);
void fishing_rod_cast_sequence();
void goto_rod_position(rod_control_params_t* params, int16_t destination, boolean override_current_move = true, boolean update_params = true);
void sync_encoders();
void print_encoder_info();
void fishing_serialcomm_control_task(void *novars);
void fishing_rod_cast_sequence();
void goto_rod_position(rod_control_params_t* params, int16_t destination, boolean override_current_move, boolean update_params);
int fishStateMachine(int key, int state);
//Utility Funcitons
int get_inputchar();
esp_err_t fish_nvs_write();
esp_err_t fish_nvs_read();

#endif //CH_FISHINATOR_ESP_FISH_CNC_H
