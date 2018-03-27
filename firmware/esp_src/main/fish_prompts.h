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


const static char* fishp_intro = "\n\nThis is the beta software for the CH_Fishinator, build March 2018.\n\n";




const static char* fishp_help =
      " \n\nFISHINATOR CONTROL COMMANDS\n"
            "------------------------------------------------------------\n"
            " '?' - Print this help text\n"
            " 'y' - Print all three encoder current values\n"
            " 't' - SYNC: Set Quad encoders equal to absolute encoder.\n"
            " '|' - Reset zero for absolute encoder: !!!CAREFUL!!!\n"
            " '=' - Test Solenoid - Hold key to hold solenoid active\n"
            "\n"
            " 'a' - Adv. to next state - CENTER PUFF HARD \n"
            " 'c' - Ret. to prev. state - CENTER SIP HARD \n"
            " 'f' - Inc. cast speed +1000. Range(5000-50000) - RIGHT PUFF \n"
            " 'j' - Slow cast speed -1000. Range(5000-50000) - LEFT PUFF \n"
            " 'h' - Release +5 counts later.   ~10 counts per degree - RIGHT SIP \n"
            " 'm' - Release -5 counts earlier. ~10 counts per degree - LEFT SIP \n"
            " 'p' - Confirm CAST, after 'a' to start cast - LIP SWITCH \n"
            "\t NOTE: If sequence successful, WILL CAST IMMEDIATELY\n"
            "\n\n";

#endif //CH_FISHINATOR_ESP_FISH_PROMPTS_H
