
set(ESP_SNIPPETS_PATH "${PROJECT_SOURCE_DIR}/components/esp32-snippets") #new: sdks are now included as submodule in /components

#NOTE: ESP snippets also provide a bunch of example code which we can simply copy into our programs, other than the utils,
# look at the folders with .c/.cpp files and you can copy those over.

#Includes files for Arduino-ESP
include_directories("${ESP_SNIPPETS_PATH}")
include_directories("${ESP_SNIPPETS_PATH}/c-utils")
include_directories("${ESP_SNIPPETS_PATH}/cpp_utils")
include_directories("${ESP_SNIPPETS_PATH}/curl/build_files_lib")
include_directories("${ESP_SNIPPETS_PATH}/filesystems/espfs/components/espfs")

# Lots of devices in hardware:
include_directories("${ESP_SNIPPETS_PATH}/hardware") #Need to add extensions

include_directories("${ESP_SNIPPETS_PATH}/memory")
include_directories("${ESP_SNIPPETS_PATH}/networking/bootwifi")  #other in networking
include_directories("${ESP_SNIPPETS_PATH}/vfs/spiffs")



