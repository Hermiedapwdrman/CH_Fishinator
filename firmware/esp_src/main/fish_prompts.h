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
#define FISH_DATE_REV 180330

const static char* fishp_splash ="\n"
      "  ______ __  __ ______     ______ ____ _____  __  __ ____ _   __ ___   ______ ____   ____ \n"
      " /_  __// / / // ____/    / ____//  _// ___/ / / / //  _// | / //   | /_  __// __ \\ / __ \\\n"
      "  / /  / /_/ // __/      / /_    / /  \\__ \\ / /_/ / / / /  |/ // /| |  / /  / / / // /_/ /\n"
      " / /  / __  // /___     / __/  _/ /  ___/ // __  /_/ / / /|  // ___ | / /  / /_/ // _, _/ \n"
      "/_/  /_/ /_//_____/    /_/    /___/ /____//_/ /_//___//_/ |_//_/  |_|/_/   \\____//_/ |_|  \n"
      "                                                                                          ";


const static char* fishp_intro = "\nThis is the beta software for the CH_Fishinator system.\n"
    "\t This software is to be controlled via serial terminal with the on board keyboard or with a quadstick.\n\n";




const static char* fishp_help =
      " \n\nFISHINATOR CONTROL COMMANDS\n"
      "--------------------------------------------------------------------------------------\n"
      // " 'CNTL-]' - To exit the termimal, 'make monitor' to reconnect"
      " 'SPACE'- EMERGENCY STOP\n"
      " '?' - Print this help text\n"
      " 'y' - Print all three encoder current values\n"
      " 't' - SYNC: Set Quad encoders equal to absolute encoder. SIDE STRAW SIP\n"
      " '|' - Reset zero for absolute encoder: !!!CAREFUL!!!\n"
      " ':' - Reset cast start and release positions, follow prompts. AND save values to non-vol storage.\n"
      "\n"
      " '=' - Test Solenoid - Hold key to hold solenoid active\n"
      " 'u' - Reel at full speed - JOYSTICK UP\n"
      " 'd' - Reel at half speed - JOYSTICK DOWN\n"
      " 'l' or 'x' - Move rod forward slowly - JOYSTICK LEFT\n"
      " 'r' or 'z' - Move rod backward slowly - JOYSTICK RIGHT\n"
      "\n"
      " 'a' - Adv. to next state - CENTER PUFF HARD \n"
      " 'c' - Ret. to prev. state - CENTER SIP HARD \n"
      " 'f' - Slow cast speed -1000. Range(5000-50000) - RIGHT PUFF \n"
      " 'j' - Inc. cast speed +1000. Range(5000-50000) - LEFT PUFF \n"
      " 'h' - Release +5 counts earlier.   ~10 counts per degree - RIGHT SIP \n"
      " 'm' - Release -5 counts later. ~10 counts per degree - LEFT SIP \n"
      " 'b' - Confirm CAST, after 'a' to start cast - CENTER PUFF SOFT \n"
      " 'p' - Confirm CAST, after 'a' to start cast - LIP SWITCH OR SIDE PUFF\n"
      "\t NOTE: IF SEQUENCE IS CORRECT, IT WILL CAST IMMEDIATELY!\n"
      "\n"
      " NOTE: Cast moves in a decreasing direction, CAST_START~3000, RELEASE~2300, RETRIEVE~1000\n"
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
