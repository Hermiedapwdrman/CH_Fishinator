#### Resources for Programming ESP32 using IDF and or Arduino

1. Remember that 'most' arduino libraries can be used if using the arduino-esp libraries and calling `initArduino()`

2. ESP Technical manual in the datasheets will describe the various core systems, peripherials, the interrupt system, as well asl all the config register.
However it does not describe any of the build software, ESP-IDF SDK, or the freeRTOS libraries.

3) Software/SDK for ESP-IDF documentation can be found, mainly look at API-guides: http://esp-idf.readthedocs.io/en/latest
 
4) Documentation is provided in sdk header files at function declarations.

5) FreeRTOS documnetation is can be found in the datasheets or online at: http://www.freertos.org/RTOS.html