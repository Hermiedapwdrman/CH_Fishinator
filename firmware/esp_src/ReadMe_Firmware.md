##Notes for cloning project template

####Note: Check the repo branches, there might be minor difference betweeen IDF and Arduino templates.

1) *git clone* this template into a new project directory.
2) Inside the **/firmware_ directory**, alter the **CMakeLists.txt** file:
    * The project name
    * The source files for the project
3) Modify the **Makefile** project name, this will be the name of the compiled binaries
    * Note: Refer to [IDF Make docs](https://esp-idf.readthedocs.io/en/v1.0/build_system.html) for more complex options
    * Refer to component.mk file instructions for building in other libraries
3) In a terminal window call the *make menuconfig* from the **firmware** directory to open up the configuration GUI
    * This configuration is saved at **sdkconfig**
    * BT/BLE/WIFI, Arduino control and other 'sub modules/peripherials' are activated via the GUI
    * Note: Upload ports, serial speeds and some other interface stuff here too.
    
5) Note: app_main() is declared as a c function, if your main source is a .cpp file, make sure to prepend  **extern "C"** onto function definition.

5) If you are not using arduino, you can delete symbolic link and comment out include directory in top level **CMakeLists.txt**


    #####Reference docs: https://esp-idf.readthedocs.io/

  
###Regarding Arduino-esp as a component:
1) The IDF build system will find arduino-esp if it is sybolically linked:
  *  _ln -s /CH/development/sdk/arduino-esp32_
2) If you want to user setup() and loop() you must set it in the **sdkconfig**
  * __makemenu -> components --> start arduino on boot__
  
3) If the libraries will not compile, IE BT and Wifi, you will need
to activate them in the **sdkconfig --> components** then it will build
  
4) In both cases, your main source fil needs to be a .cpp file, else the arduino sdk build will fail
    * If you are using app_main(), it needs to be preceeded by **extern "C"**
    
4) Reference: https://github.com/espressif/arduino-esp32#using-as-esp-idf-component
  
5) If you are not using arduino, you can delete symbolic link and comment out include directory in top level **CMakeLists.txt**

###Notes regarding CLion editor and its behavior.
1) All of the various targets should work with the indexer.  IF NOT, make sure the main/src director is marked as a 'project source'
defined via the RMB menu.