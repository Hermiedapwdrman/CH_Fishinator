/***************************
    @file fish_CnC.h
    
    @brief Command and control for the Fishinator. This includes
     initialization and communication of peripherals, ISRs, and state machines.

    Peripherals include:
        - AMT20 Absolute Encoder via SPI (VSPI)
        - Roboclaw ION MC Motor Controller via UART 1
        - Solenoid Via GPIO

     @author Patrick Wagner
     @date 4/5/18

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

/**
 * fishing_serialcomm_control_task()
 *
 * @brief This is the task which is given to the scheduler for handling Fishinator
 * input and state transitions.  Behaviors include waiting for serial input
 * from the input stream, branch on input to perform, sensor calibration, status
 * output, test actuators, change casting variables, and initiate fish state machine
 * transitions.
 *
 * Control characters are listed in the fish_prompts.h 'fishp_help' var.
 *
 * @param novars: Null pointer present in all task functions in examples, why? I ONO.
 */
void fishing_serialcomm_control_task(void *novars);

/**
 *  fishStateMachine()
 *
 *  @brief This is the state machine to execute the standard fishing behavior or cast, retrieve, hook and reset.
 *  States are described in the fish help prompt, but consist of 4 locations: CAST START-8, RETRIEVE-4, NUETRAL-2
 *  HOOK-11.  These state numbers correspond roughly to their angular positions as denoted by an analog clock (12 is vertical)
 *
 *  Refer to external documentation TODO: add external doc link.
 *
 * @param key - Pass input character 'a','c' to indicate that a state transition is happening.
 * @param state - uint8_t state variable stored globably.
 * @return current state is returned, so standard call sytax is: state_var = fishStateMachine('char', state_var);
 */
int fishStateMachine(int key, int state);

/**
 *  fishing_rod_cast_sequence();
 *
 *  @brief Executes when the correct control character sequence is passed to the state machine, the SM is in the correct state
 *  and the rod is in the correct location.  This function is a series of commands to send to the motor controller,
 *  with posistion checks to perform a cast.
 *
 */
void fishing_rod_cast_sequence();

/**
 * goto_rod_position()
 *
 * @brief Helper function to handle communication with serial controlled motor controller, this function should handle
 * all position based, PID control based movements with respect to the rod motor.
 *
 * @param params - rod_control_params_t passed to MC if update_params = true
 * @param destination - ABS destination for the move.
 * @param override_current_move - True will issue command override; False = Move will occur AFTER current move.
 * @param update_params - Will update parameter registers in MC before move if true
 */
void goto_rod_position(rod_control_params_t* params, int16_t destination, boolean override_current_move = true, boolean update_params = true);

/**
 *  A method of syncronizing the two quadrature encoders with the absolute encoder. This will perform read of the AMT20
 *  abs location, and set the quad encoders equal.
 */
void sync_encoders();
//Utility Funcitons
/**
 *  Helper function to print out all three encoder values to the standard out (serial terminal). NOTE: Does not read
 *  from either peripheral, simply prints out saved variables.
 */
void print_encoder_info();
/**
 * get_inputchar()
 *
 *  Will check for a valid (non-EOF) character from std input stream. Will sleep the thread if none is found. NOTE:
 *  If this is being called from multiple tasks, it is possible they will fight over input chars.
 *  Currently only called from fish_serialcom_control_task and its helper functions.
 *
 * @return Input character from serial stream in.
 */
int get_inputchar();
/**
 * fish_nvs_write()
 *
 *  Writes cast start, finish and release location to non-volatile ram. These are physical position and should be saved.
 *
 * @return Standard esp error code from the NVS library error code set.
 */
esp_err_t fish_nvs_write();
/**
 * fish_nvs_write()
 *
 *  Reads cast start, finish and release location to non-volatile ram. These are physical position and should be saved.
 *
 * @return Standard esp error code from the NVS library error code set.
 */
esp_err_t fish_nvs_read();

#endif //CH_FISHINATOR_ESP_FISH_CNC_H
