#Toolchain Build defines and other things for the indexer,
#Include AVR dude functionality
# Lifted shamelessly from https://github.com/acristoffers/CMake-AVR/blob/master/Blink/CMakeLists.txt

# program names
set(AVRCPP   avr-g++)
set(AVRC     avr-gcc)
set(AVRSTRIP avr-strip)
set(OBJCOPY  avr-objcopy)
set(OBJDUMP  avr-objdump)
set(AVRSIZE  avr-size)
set(AVRDUDE  avrdude)
set(MAKECMD  make )

# Sets the compiler
# Needs to come before the project function
#set(CMAKE_SYSTEM_NAME  Generic)
#set(CMAKE_CXX_COMPILER ${AVRCPP})
#set(CMAKE_C_COMPILER   ${AVRC})
#set(CMAKE_ASM_COMPILER   ${AVRC})

# Compiler flags
set(CSTANDARD "-std=gnu99")
set(CDEBUG    "-gstabs -g -ggdb")
set(CWARN     "-Wall -Wstrict-prototypes -Wl,--gc-sections -Wl,--relax")
set(CTUNING   "-funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -ffunction-sections -fdata-sections")
set(COPT      "-Os -lm -lprintf_flt")
#set(CMCU      "-mmcu=${MCU}")
set(CDEFS     "-DF_CPU=${F_CPU} -DBAUD=${BAUD}")

set(CFLAGS   "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CWARN} ${CSTANDARD} ${CTUNING}")
set(CXXFLAGS "${CMCU} ${CDEBUG} ${CDEFS} ${COPT} ${CTUNING}")

set(CMAKE_C_FLAGS   "${CFLAGS}")
set(CMAKE_CXX_FLAGS "${CXXFLAGS}")
set(CMAKE_ASM_FLAGS   "${CFLAGS}")

## Project setup
#include_directories(${INC_PATH} ${LIB_INC_PATH})
#set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")
#add_custom_target(build_all ${MAKECMD} all)
#add_custom_target(clean_all ${MAKECMD} clean)
#add_custom_target(dfu ${MAKECMD} upload)




#Below examples are more for CMake avr, we are using LUFA build system.
## Compiling targets
#add_custom_target(strip ALL     ${AVRSTRIP} "${PROJECT_NAME}.elf" DEPENDS ${PROJECT_NAME})
#add_custom_target(hex   ALL     ${OBJCOPY} -R .eeprom -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.hex" DEPENDS strip)
#add_custom_target(eeprom        ${OBJCOPY} -j .eeprom --change-section-lma .eeprom=0 -O ihex "${PROJECT_NAME}.elf" "${PROJECT_NAME}.eeprom" DEPENDS strip)
#add_custom_target(disassemble   ${OBJDUMP} -S "${PROJECT_NAME}.elf" > "${PROJECT_NAME}.lst" DEPENDS strip)
## Flashing targets
#add_custom_target(flash_usbtiny ${AVRDUDE} -c usbtiny -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
#add_custom_target(flash_usbasp  ${AVRDUDE} -c usbasp -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
#add_custom_target(flash_ardisp  ${AVRDUDE} -c avrisp -p ${MCU} -b 19200 -P ${USBPORT} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
#add_custom_target(flash_109     ${AVRDUDE} -c avr109 -p ${MCU} -b 9600 -P ${USBPORT} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
#add_custom_target(flash_eeprom  ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U eeprom:w:${PROJECT_NAME}.hex DEPENDS eeprom)
## Fuses (For ATMega328P-PU, Calculated using http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega328p)
#add_custom_target(reset         ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -e)
#add_custom_target(fuses_1mhz    ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0x62:m)
#add_custom_target(fuses_8mhz    ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xE2:m)
#add_custom_target(fuses_16mhz   ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xFF:m)
#add_custom_target(fuses_uno     ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U lfuse:w:0xFF:m -U hfuse:w:0xDE:m -U efuse:w:0x05:m)
#add_custom_target(set_eeprom_save_fuse   ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U hfuse:w:0xD1:m)
#add_custom_target(clear_eeprom_save_fuse ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -U hfuse:w:0xD9:m)
## Utilities targets
#add_custom_target(avr_terminal  ${AVRDUDE} -c ${PROG_TYPE} -p ${MCU} ${PROG_ARGS} -nt)

set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.lst")

# Config logging
message("* ")
message("* Project Name:\t${PROJECT_NAME}")
message("* Project Source:\t${SRC_PATH}")
message("* Project Include:\t${INC_PATH}")
message("* Library Include:\t${LIB_INC_PATH}")
message("* ")
message("* Project Source Files:\t${SRC_FILES}")
message("* Library Source Files:\t${LIB_SRC_FILES}")
message("* ")
message("* C Flags:\t${CMAKE_C_FLAGS}")
message("* ")
message("* CXX Flags:\t${CMAKE_C_FLAGS}")
message("* ")
