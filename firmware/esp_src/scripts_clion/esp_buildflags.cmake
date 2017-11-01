
set(XTENSA_TOOLCHAIN_PATH "/opt/esp/xtensa-esp32-elf")

#Where are these resolved
include_directories("${XTENSA_TOOLCHAIN_PATH}/include")
include_directories("${XTENSA_TOOLCHAIN_PATH}/lib/gcc/xtensa-esp32-elf/5.2.0/include")
include_directories("${XTENSA_TOOLCHAIN_PATH}/lib/gcc/xtensa-esp32-elf/5.2.0/include-fixed")



SET(CMAKE_C_COMPILER "${XTENSA_TOOLCHAIN_PATH}/bin/xtensa-esp32-elf-gcc")
SET(CMAKE_CXX_COMPILER "${XTENSA_TOOLCHAIN_PATH}/bin/xtensa-esp32-elf-g++")
SET(CMAKE_CXX_FLAGS_DISTRIBUTION "-fno-rtti -fno-exceptions -std=gnu++11 -Wall -Werror=all -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-error=unused-function -Og -g3 -nostdlib -Wpointer-arith -Wno-error=unused-but-set-variable -Wno-error=unused-variable -mlongcalls -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -DWITH_POSIX -DIDF_VER=&quot;2.200.0&quot; -DESP32 -DESP_PLATFORM -DF_CPU=240000000L -DHAVE_CONFIG_H -DMBEDTLS_CONFIG_FILE=&quot;mbedtls/esp_config.h&quot; -DPLATFORMIO=30300 -DESP32_DEV")
SET(CMAKE_C_FLAGS_DISTRIBUTION "-Wno-old-style-declaration -std=gnu99 -Wall -Werror=all -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare -Wno-error=unused-function -Og -g3 -nostdlib -Wpointer-arith -Wno-error=unused-but-set-variable -Wno-error=unused-variable -mlongcalls -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -DWITH_POSIX -DIDF_VER=&quot;2.200.0&quot; -DESP32 -DESP_PLATFORM -DF_CPU=240000000L -DHAVE_CONFIG_H -DMBEDTLS_CONFIG_FILE=&quot;mbedtls/esp_config.h&quot; -DPLATFORMIO=30300 -DESP32_DEV")
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DWITH_POSIX)
add_definitions(-DIDF_VER="2.200.0")
add_definitions(-DESP32)
add_definitions(-DESP_PLATFORM)
add_definitions(-DF_CPU=240000000L)
add_definitions(-DHAVE_CONFIG_H)
add_definitions(-DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h")
add_definitions(-DPLATFORMIO=30300)
add_definitions(-DESP32_DEV)
