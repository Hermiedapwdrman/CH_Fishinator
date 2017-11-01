##Manual list of all the cmake folders in the SDK and library chains, A KLUDGE but it works.

set(BLYNK_SDK_PATH "/CH/development/libraries_external/blynk-library/src")

include_directories("${BLYNK_SDK_PATH}/")
include_directories("${BLYNK_SDK_PATH}/Adapters")
include_directories("${BLYNK_SDK_PATH}/Blynk")
include_directories("${BLYNK_SDK_PATH}/Utility")

