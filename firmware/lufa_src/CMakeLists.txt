cmake_minimum_required(VERSION 3.6)
project(AAB_LUFA_TEMPLATE_LUFA)  #TODO: CHANGE THIS WITH NEW PROJECT
#Initial build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


#Set CMAKE to verbose output (IE see the compiler stdout)
set(CMAKE_VERBOSE_MAKEFILE ON)

##Port Settings
#set(PROG_TYPE avrisp)
#set(USBPORT /dev/tty.usbmodem411)
## extra arguments to avrdude: baud rate, chip type, -F flag, etc.
#set(PROG_ARGS -b 19200 -P ${USBPORT})

# Variables regarding the AVR chip
set(MCU   at90usb1287)
set(F_CPU 16000000)
set(BAUD  36400)
add_definitions(-DF_CPU=${F_CPU})
add_definitions(-DUSB_CAN_BE_DEVICE)
add_definitions(-DARCH=ARCH_AVR8)


### Define path to top level makefile your project, IE top level src directory
#set(BASE_PATH    "${${PROJECT_NAME}_SOURCE_DIR}")
##set(SRC_PATH     "${BASE_PATH}/src/KeyboardMouse")
##set(SRC_PATH     "${BASE_PATH}/src/VirtualSerial")
#set(SRC_PATH     "${BASE_PATH}/src/VirtualSerialMouse")
##and any library paths you want to include
#set(LIB_DIR_PATH "${BASE_PATH}/lib")


#include the cmake scripts with sdk includes, defines and build target macros
#include(scripts/avr_gcc_toolchain.cmake)
include(scripts/avr_toolchain.cmake)
include(scripts/avr_sdk_includes.cmake)
include(scripts/lufa_sdk_includes.cmake)
#include(scripts/duo_build_commands.cmake)


#Include any 3rd party libraries here for the indexer to well... index.  TODO: New library scripts go here.
#Blynk Android app library
include(scripts/lib_blynk_includes.cmake)

add_subdirectory(src)

#Kludge to get cmake indexer to run
add_executable(INDEX_full ZZ_NULL.cpp)

##We glob all the src files together
#set(SRC_FILES)
#set(SRC_INC_PATH)
## Files to be compiled
#file(GLOB SRC_FILES "${SRC_PATH}/*.cpp"
#        "${SRC_PATH}/*.cc"
#        "${SRC_PATH}/*.c"
#        "${SRC_PATH}/*.cxx"
#        "${SRC_PATH}/*.S"
#        "${SRC_PATH}/*.s"
#        "${SRC_PATH}/*.sx"
#        "${SRC_PATH}/*.asm")
#file(GLOB SOURCES "${SRC_PATH}/*")
##foreach(subdir ${SOURCES})
##    file(GLOB src_files "${subdir}/*.cpp"
##            "${subdir}/*.cc"
##            "${subdir}/*.c"
##            "${subdir}/*.cxx"
##            "${subdir}/*.S"
##            "${subdir}/*.s"
##            "${subdir}/*.sx"
##            "${subdir}/*.asm")
##    if(IS_DIRECTORY ${subdir})
##        list(APPEND SRC_INC_PATH  "${subdir}")
##    endif()
##    list(APPEND SRC_FILES "${src_files}")
##endforeach()
##Print Source files
#message(STATUS "Current Source: ${SRC_FILES}")
#
#
#set(LIB_SRC_FILES)
#set(LIB_INC_PATH)
#file(GLOB LIBRARIES "${LIB_DIR_PATH}/*")
#foreach(subdir ${LIBRARIES})
#    file(GLOB lib_files "${subdir}/*.cpp"
#            "${subdir}/*.cc"
#            "${subdir}/*.c"
#            "${subdir}/*.cxx"
#            "${subdir}/*.S"
#            "${subdir}/*.s"
#            "${subdir}/*.sx"
#            "${subdir}/*.asm")
#    if(IS_DIRECTORY ${subdir})
#        list(APPEND LIB_INC_PATH  "${subdir}")
#    endif()
#    list(APPEND LIB_SRC_FILES "${lib_files}")
#endforeach()
#message(STATUS "Library Sources: ${LIB_SRC_FILES}")
#
#
#
###And the target will be generate automatically..
#add_executable(${PROJECT_NAME} ${SRC_FILES} ${LIB_SRC_FILES})
#add_custom_target(build_all COMMAND ${MAKECMD} all -C ${SRC_PATH})
#add_custom_target(clean_all COMMAND ${MAKECMD} clean -C ${SRC_PATH})
#add_custom_target(upload COMMAND ${MAKECMD} dfu -C ${SRC_PATH} DEPENDS build_all)







##Include libraries, defines and flags for the indexer here.
##Defines required to pass to make file macros, will be parsed by firmware mikefile build system
#set(MAKE_DEFINES "PLATFORM=duo")
#LIST(APPEND MAKE_DEFINES "")
##Include target defines in cmake preprocessor indexer, this will not propogate to build system, this will make sure the indexer is resolving #pragma code.
#set(TARGET_DEFINES "")
#list(APPEND TARGET_DEFINES "")
#add_definitions(${TARGET_DEFINES})
##If you need to add compiler options(not likely) then you can add them for the indexer, NOTE: Does not pass to spark build system.
#add_compile_options()
##If yo
#
#
##Include directories with sources build  #TODO: REmember to include directory try in sub makelists.
#add_subdirectory(src)
#
####################################
##KLUDGE to get indexer to work, just call any of the project sources here.
#add_library(ZZ_donotdelete src/project_or_subproject/main.cpp)#TODO: Add main file here as library to force indexer to index the project
#
#
##message(WARNING "CMAKE compililation completed, no erros.")
