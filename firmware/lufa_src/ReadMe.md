##Notes for cloning template

There are x number of thing needing your attention to clone a project

1) Git clone to a new directory, rename directory
1) Change name of the *Project()* in top *CmakeLists.txt*
1) Add any definitions such as add_definitions(-DUSB_CAN_BE_DEVICE) to help the indexer resolve the included files

1) add_subdirectory() for any subproject in the src directory.
1) Two Files must be included and altered withing the subproject directory for the correct compiling in clion
* *src/subproject/CmakeLists.txt* -  Should be copied from a previous project. Then change the  (**TRAGET**) variable
 and the **SRC_PATH** Variable.
* *src/subproject/makefile* - makefile from lufa class drivers. Can copy from previous project or re-define the following. 
* MCU, SRC LUFA_PATH
1)Finally set the src directory via right clicking to the **project sources label** for the indexer.

**NOTE**: If you switch to the ZZ_null target, the indexer does a much better job of resolving.  Additionally, mark the
**SRC** directory as project sources to help the indexer, done via RMB.


**Note**: If you are starting a new Lufa project, look at the templates located in the sdk/lufa/CodeTemplates directory.

TODO: Add info about libraries, how to include