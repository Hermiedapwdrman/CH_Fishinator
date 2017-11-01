#Create SDK include paths variable
set(LUFA_SDK_INCLUDE_ROOT "/CH/development/sdk/lufa/")

set(LUFA_SDK_INCLUDE_PATH)
#Append all the SDK Paths needed for indexing

#Inclued all foklers to the indexer path for AVR8 architecture
#NOTE: XMEGA OR UC3 architecture paths are not included

list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Common")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Board")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Board/AVR8") #No files, just leads to boar times
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Board/AVR8/USBKEY")  #There are many more in this folder
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Board/UC3")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Board/XMEGA")

list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Misc")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Peripheral")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Peripheral/AVR8")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/Peripheral/XMEGA")

list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Class")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Class/Common")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Class/Device")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Class/Host")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/AVR8")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/AVR8/Template")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/UC3")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/UC3/Template")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/XMEGA")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Drivers/USB/Core/XMEGA/Template")

list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Platform")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Platform/UC3")
#list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/Platform/XMEGA")
list(APPEND LUFA_SDK_INCLUDE_PATH "${LUFA_SDK_INCLUDE_ROOT}/LUFA/util")

#Finally inlcude one directory back to conform to LUFAs library path scheme




#Include all directories for the sdk by unpacking variable
include_directories(${LUFA_SDK_INCLUDE_PATH})
message(STATUS "LUFA SDK include paths: ${LUFA_SDK_INCLUDE_PATH}")

#Alternate paths not needed for build but can be included for the indexer for debuging resolves.
#include_directories(${AVR_SDK_ALTERNATE_PATH})
#message(STATUS "SDK alternate include paths: ${AVR_SDK_ALTERNATE_PATH}")