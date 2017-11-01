#Standard path to call top level make.  Can be changed if needed.
set(MAKE_DIR ${PROJECT_SOURCE_DIR})



add_custom_target(
        Build_all
        COMMAND make all
        WORKING_DIRECTORY ${MAKE_DIR}
)

add_custom_target(
        Clean_all
        COMMAND make clean
        WORKING_DIRECTORY ${MAKE_DIR}
)


add_custom_target(
        Flash_all
        COMMAND make flash
        WORKING_DIRECTORY ${MAKE_DIR}
)
add_custom_target(
        Build_app
        COMMAND make app
        WORKING_DIRECTORY ${MAKE_DIR}
)

add_custom_target(
        Flash_app
        COMMAND make app-flash
        WORKING_DIRECTORY ${MAKE_DIR}
)

add_custom_target(
        Monitor
        COMMAND make monitor
        WORKING_DIRECTORY ${MAKE_DIR}
)

add_custom_target(
        PrintPartition
        COMMAND make partition_table
        WORKING_DIRECTORY ${MAKE_DIR}
)

add_custom_target(
        Erase_all
        COMMAND make erase_flash
        WORKING_DIRECTORY ${MAKE_DIR}
)

#add_custom_target(
#        MenuConfig
#        COMMAND make menuconfig
#        WORKING_DIRECTORY ${MAKE_DIR}
#)



#add_executable(${PROJECT_NAME} ${SRC_LIST})