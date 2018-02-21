##Esp-IDF and Arduino firmware build notes:

#### Project organization:

The esp-idf sdk is a comprehensive development framework distrubited in conjuction with Espressif's 
ESP32 family of IOT SoC's. Arduino-esp32 is a separate project creating an abstraction layer for the esp-idf sdk 
which implements the standard arduino libraries.  Used separately or together we should have lots of open source
libraries available for our use.

For our projects we want to make available both the idf and arduino components, while reducing the amount of copy pasting and
frustration trying to keep the various sdk and libraries up to date.

Basic esp firmware project structure is as follows:
* ./main: Contains all the current project source, sub directories are possible and will be build by standard calls.
* ./components: Contains any external libraries used in the project, these can be either cloned or included as submodules.
* ./components/esp-idf: The main sdk used for development, a submodule pulled from **/CH/development/sdk/esp-idf**, local clone the main repo: https://github.com/espressif/esp-idf
* ./components/arduino-esp32: Arduino port from espressif, a submodule pulled from **/CH/development/sdk/arduino-esp32** a local clone of main repo: https://github.com/espressif/arduino-esp32
* ./scripts_clion: contains cmake files with preproccor defines, include paths and compiler flags, SOLELY for the benifit of the indexer.
* ./cmake-build-xxxx: The directory the cmake build process builds, we are using make, not cmake, so this directory is not usefull.
* ./build: Build directory populated by esp-idf make build system. Has all the compiled objects and binaries.


#### Cloning project template for a new project

###### Note: There are now significant differences between esp-idf 2.1 and current master branch, see ReadMe_Components.md for direction on freezing sdk versions.

###### Note: If you switch IDF branches, you must update all submodules: See the component readme for instructions.

1) *git clone* this template into a new project directory.
2) Inside the **/firmware_ directory**, alter the **CMakeLists.txt** file:
    * The project name
    * The source files for the project
3) Modify the **Makefile** project name, this will be the name of the compiled binaries
    * Note: Refer to [IDF Make docs](https://esp-idf.readthedocs.io/en/v1.0/build_system.html) for more complex options
    * Refer to component.mk file instructions for building in other libraries
4) If using submodules you have to `git submodules init` `sync` and `update`
5) Mark the main directory (the main source directory), and components (sdk and libraries) as project source via right click menu.
3) In a terminal window call the `make menuconfig` from the **firmware** directory to open up the configuration GUI
    * This configuration is saved at **sdkconfig**
    * BT/BLE/WIFI, Arduino control and other 'sub modules/peripherials' are activated via the GUI
    * Note: Upload ports, serial speeds and some other interface stuff here too.

###### Reference docs: https://esp-idf.readthedocs.io/

#### Adding a main function

When working with C++, your calling function should also be written in C++.  Consider replacing your `main.c` with the following
`main.cpp` file:

```
extern "C" {
   void app_main();
}

void app_main() {
   // Your code goes here
}    
```


  
#### Regarding arduino-esp32 as a component:

###### Note: Check the repo branches, there might be minor difference betweeen IDF and Arduino templates.

1) If you are not using arduino, you can de-init the component submodule by using the command `git submodule deinit components/arduino-esp32` 
    * To reinit use `git supmodule init components/arduino-esp32 && git supmodule update`  

2) If you want to user setup() and loop() you must set it in the **sdkconfig**
    * __makemenu -> components --> start arduino on boot__
  
3) If the libraries will not compile, IE BT and Wifi, you will need
to activate them in the **sdkconfig --> components** then it will build.
  
4) In both cases, your main source fil needs to be a .cpp file, else the arduino sdk build will fail

4) Reference: https://github.com/espressif/arduino-esp32#using-as-esp-idf-component
  

#### Notes regarding CLion editor and its behavior.
1) All of the various targets should work with the indexer.  IF NOT, make sure the main/src director is marked as a 'project source'
defined via the RMB menu.
    1) Note that the components directory should note be marked as source headers, this makes the indexer behave weirdly.  Includes are
    done via the include scripts files.

2) The esp-idf sdk and arduino ports are constantly changing, check the CMAKE includes periodically to make sure the folder structure and includes haven changed.
