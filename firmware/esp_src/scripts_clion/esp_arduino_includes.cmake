
#set(ESP_ARDUINO_PATH "/CH/development/sdk/arduino-esp32") #old absolute path for sdk indexing
set(ESP_ARDUINO_PATH "${PROJECT_SOURCE_DIR}/components/arduino-esp32") #new: sdks are now included as submodule in /components

#If variants make sense then you can include them from these paths
#include_directories("${ESP_ARDUINO_PATH}/variants/esp32thing")


#Includes files for Arduino-ESP
include_directories("${ESP_ARDUINO_PATH}")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/app_trace")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/app_update")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/bluedroid")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/bootloader_support")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/bt")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/coap")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/coap/coap")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/config")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/console")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/console/argtable3")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/console/linenoise")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/driver/driver")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/esp32")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/hwcrypto")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/rom")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/config")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/tie")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/xtav60")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/xtav110")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/xtav200")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/xtkc705")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp32/xtensa/xtml605")

include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/esp_adc_cal")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/ethernet")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/ethernet/eth_phy")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/expat")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/fatfs")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/freertos/freertos")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/heap")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/jsmn")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/json")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/log")


include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/apps")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/apps/sntp")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/arch")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/arpa")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/lwip")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/lwip/priv")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/netif")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/netif/ppp")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/netif/ppp/polarssl")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/netinet")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/lwip/sys")

include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/mbedtls/mbedtls")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/mbedtls_port/mbedtls")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/mdns")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/newlib")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/newlib/machine")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/newlib/sys")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/newlib/xtensa/config")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/nghttp")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/nghttp/nghttp2")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/nvs_flash")

include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/openssl/internal")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/openssl/openssl")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/openssl/platform")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/sdmmc")

include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/soc/soc")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/spi_flash")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/spiffs")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/tcpip_adapter")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/ulp/esp32")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/vfs")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/vfs/sys")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wear_levelling")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/crypto")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/wpa")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/wpa2")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/wpa2/eap_peer")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/wpa2/tls")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/wpa_supplicant/wpa2/utils")
include_directories("${ESP_ARDUINO_PATH}/tools/sdk/include/xtensa-debug-module")


include_directories("${ESP_ARDUINO_PATH}/cores/esp32")
include_directories("${ESP_ARDUINO_PATH}/cores/esp32/libb64")

include_directories("${ESP_ARDUINO_PATH}/libraries/ArduinoOTA/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/EEPROM")
include_directories("${ESP_ARDUINO_PATH}/libraries/ESP32/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/ESPmDNS/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/FS/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/HTTPClient/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/Preferences/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/SD/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/SD_MMC/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/SimpleBLE/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/SPI/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/SPIFFS/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/Update/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/WiFi/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/WiFiClientSecure/src")
include_directories("${ESP_ARDUINO_PATH}/libraries/Wire/src")


