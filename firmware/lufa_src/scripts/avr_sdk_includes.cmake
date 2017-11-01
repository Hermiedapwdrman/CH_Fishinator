#Create SDK include paths variable
set(AVR_SDK_INCLUDE_ROOT "/usr/lib/avr/include")

set(AVR_SDK_INCLUDE_PATH)
set(AVR_SDK_ALTERNATE_PATH)
#Append all the SDK Paths needed for indexing

list(APPEND AVR_SDK_INCLUDE_PATH "${AVR_SDK_INCLUDE_ROOT}")
list(APPEND AVR_SDK_INCLUDE_PATH "${AVR_SDK_INCLUDE_ROOT}/avr")
list(APPEND AVR_SDK_INCLUDE_PATH "${AVR_SDK_INCLUDE_ROOT}/compat")
list(APPEND AVR_SDK_INCLUDE_PATH "${AVR_SDK_INCLUDE_ROOT}/util")


#Alternate paths not needed for build but can be included below when looking through the source for the indexer.
list(APPEND  AVR_SDK_ALTERNATE_PATH "${AVR_SDK_INCLUDE_ROOT}")


#Include all directories for the sdk by unpacking variable
include_directories(${AVR_SDK_INCLUDE_PATH})
message(STATUS "AVR SDK include paths: ${AVR_SDK_INCLUDE_PATH}")

#Alternate paths not needed for build but can be included for the indexer for debuging resolves.
#include_directories(${AVR_SDK_ALTERNATE_PATH})
#message(STATUS "SDK alternate include paths: ${AVR_SDK_ALTERNATE_PATH}")