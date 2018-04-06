/***************************
    @file fish_prompt.h
    
    @brief Contains all the strings and UI text blocks used in the terminal based controls. These comments should not
    contain error information or programmatic feed back, that should be handled inside the functions themselves. This should
    be project general information and User interface instructions only.

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

#ifndef CH_FISHINATOR_ESP_FISH_PROMPTS_H
#define CH_FISHINATOR_ESP_FISH_PROMPTS_H

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
