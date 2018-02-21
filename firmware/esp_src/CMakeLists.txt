cmake_minimum_required(VERSION 3.7)

##Define Project
project(AAA_Project_Template_ESP) #TODO: CHANGE THIS WITH NEW PROJECT

#include sdk scripts
# do note that this is only for the indexer and the esp-idf build system does resolve includes from these
include(scripts_clion/esp_buildflags.cmake)
include(scripts_clion/esp_arduino_includes.cmake)
include(scripts_clion/esp_idf_includes.cmake)
include(scripts_clion/esp_make_scripts.cmake)
include(scripts_clion/esp_snippets_includes.cmake)

#You can also include certain build flags for the indexter to help resolve code.
add_definitions(-DCONFIG_BT_ENABLED)  #EspIDF bluetooth enabled build flag

#include library scripts, if cmake lists are made for external libraries then you can include them here.
# do note that this is only for the indexer and the esp-idf build system does resolve includes from these
include("/CH/development/libraries/blynk-library/lib_blynk_includes.cmake")

#If you are using arduino and a specific board then you might need to include its pin map ala:
include_directories("${ESP_ARDUINO_PATH}/variants/feather_esp32")

#Set source files to your project for indexer.
#Not component.mk as well as cmakelists must be included to have build succeed.
#add_subdirectory(main)  #Not Needed
include_directories(components/HID_Joystick)
include_directories(components/example_library)

#Include source files in dumme exe to help with indexing.
set(SOURCE_FILES
        main/sdkconfig_arduino.h.bak
        main/myMain_or_ino.cpp
        )

add_executable(zSrcList ${SOURCE_FILES})

