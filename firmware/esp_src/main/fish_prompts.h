//
// Created by patrick on 3/27/18.
//

#ifndef CH_FISHINATOR_ESP_FISH_PROMPTS_H
#define CH_FISHINATOR_ESP_FISH_PROMPTS_H

/** This is a list of prompts I will be using to help Peter navigate the beta software
 *
 *  Generally the variable structure will be fishp_xxxxx for each of the strings.
 *
 *  Note: The ESP Loggind macros which do colored formated strings must be passed a macro string, not a const string.
 *  Otherwise use printf()
 */

//#define fishp_test "simple test \n\n\n\n"   //Must use macro if using logging software.
#define FISH_MAJOR_REV 0
#define FISH_MINOR_REV 2
#define FISH_DATE_REV 180327


const static char* fishp_intro = "\n\nThis is the beta software for the CH_Fishinator.\n"
    "\t This software is to be controlled via serial terminal with the on board keyboard or with a quadstick.\n\n";




const static char* fishp_help =
      " \n\nFISHINATOR CONTROL COMMANDS\n"
            "--------------------------------------------------------------------------------------\n"
            " 'SPACE'- EMERGENCY STOP\n"
            " '?' - Print this help text\n"
            " 'y' - Print all three encoder current values\n"
            " 't' - SYNC: Set Quad encoders equal to absolute encoder.\n"
            " '|' - Reset zero for absolute encoder: !!!CAREFUL!!!\n"
            " ':' - Reset cast start and release positions, follow prompts.  Will not save on reboot.\n"
            "\n"
            " '=' - Test Solenoid - Hold key to hold solenoid active\n"
            " 'u' - Reel at full speed - JOYSTICK UP\n"
            " 'd' - Reel at half speed - JOYSTICK DOWN\n"
            " 'l' or 'x' - Move rod forward slowly - JOYSTICK LEFT\n"
            " 'r' or 'z' - Move rod backward slowly - JOYSTICK RIGHT\n"
            "\n"
            " 'a' - Adv. to next state - CENTER PUFF HARD \n"
            " 'c' - Ret. to prev. state - CENTER SIP HARD \n"
            " 'f' - Inc. cast speed +1000. Range(5000-50000) - RIGHT PUFF \n"
            " 'j' - Slow cast speed -1000. Range(5000-50000) - LEFT PUFF \n"
            " 'h' - Release +5 counts later.   ~10 counts per degree - RIGHT SIP \n"
            " 'm' - Release -5 counts earlier. ~10 counts per degree - LEFT SIP \n"
            " 'p' - Confirm CAST, after 'a' to start cast - LIP SWITCH \n"
            "\t NOTE: IF SEQUENCE IS CORRECT, IT WILL CAST IMMEDIATELY!\n"
            "\n"
            " There are 4 states in the Fishing state machine:\n"
            "\tNeutral: 2 o'clock\n"
            "\tCast Start: 8 o'clock\n"
            "\tRetrieve: 4 o'clock\n"
            "\tHook: 12 o'clock\n"
            "\n"
            "Refer to drawings for the control flow. Reminders will print when a terminal is open.\n"
            "--------------------------------------------------------------------------------------\n"
            "\n\n";

#endif //CH_FISHINATOR_ESP_FISH_PROMPTS_H
