##Compile target name
set(TRAGET "USBtoSerial")

#Add sub directories, this tell them they are project source, not library/external source
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config)

#add_subdirectory(./subdir)

## Define path to top level makefile your project, IE top level src directory
set(BASE_PATH    "${${PROJECT_NAME}_SOURCE_DIR}")
#set(SRC_PATH     "${BASE_PATH}/src/KeyboardMouse")
#set(SRC_PATH     "${BASE_PATH}/src/VirtualSerial")
set(SRC_PATH     "${BASE_PATH}/src/USBtoSerial")
#and any library paths you want to include
set(LIB_DIR_PATH "${BASE_PATH}/lib")

#We glob all the src files together
set(SRC_FILES)
set(SRC_INC_PATH)
# Files to be compiled
file(GLOB SRC_FILES "${SRC_PATH}/*.cpp"
        "${SRC_PATH}/*.cc"
        "${SRC_PATH}/*.c"
        "${SRC_PATH}/*.cxx"
        "${SRC_PATH}/*.S"
        "${SRC_PATH}/*.s"
        "${SRC_PATH}/*.sx"
        "${SRC_PATH}/*.asm")
file(GLOB SOURCES "${SRC_PATH}/*")
#foreach(subdir ${SOURCES})
#    file(GLOB src_files "${subdir}/*.cpp"
#            "${subdir}/*.cc"
#            "${subdir}/*.c"
#            "${subdir}/*.cxx"
#            "${subdir}/*.S"
#            "${subdir}/*.s"
#            "${subdir}/*.sx"
#            "${subdir}/*.asm")
#    if(IS_DIRECTORY ${subdir})
#        list(APPEND SRC_INC_PATH  "${subdir}")
#    endif()
#    list(APPEND SRC_FILES "${src_files}")
#endforeach()
#Print Source files
message(STATUS "Current Source: ${SRC_FILES}")


set(LIB_SRC_FILES)
set(LIB_INC_PATH)
file(GLOB LIBRARIES "${LIB_DIR_PATH}/*")
foreach(subdir ${LIBRARIES})
    file(GLOB lib_files "${subdir}/*.cpp"
            "${subdir}/*.cc"
            "${subdir}/*.c"
            "${subdir}/*.cxx"
            "${subdir}/*.S"
            "${subdir}/*.s"
            "${subdir}/*.sx"
            "${subdir}/*.asm")
    if(IS_DIRECTORY ${subdir})
        list(APPEND LIB_INC_PATH  "${subdir}")
    endif()
    list(APPEND LIB_SRC_FILES "${lib_files}")
endforeach()
message(STATUS "Library Sources: ${LIB_SRC_FILES}")



##And the target will be generate automatically..
add_custom_target(${TRAGET}_build COMMAND ${MAKECMD} all -C ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(${TRAGET}_clean COMMAND ${MAKECMD} clean -C ${CMAKE_CURRENT_SOURCE_DIR})
add_custom_target(${TRAGET}_upload COMMAND ${MAKECMD} dfu -C ${CMAKE_CURRENT_SOURCE_DIR} DEPENDS ${TRAGET}_build)
#Add sub directories, this tell them they are project source, not library/external source
include(${PROJECT_SOURCE_DIR}/scripts/avr_toolchain.cmake)
include(${PROJECT_SOURCE_DIR}/scripts/avr_sdk_includes.cmake)
include(${PROJECT_SOURCE_DIR}/scripts/lufa_sdk_includes.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config)


#add_subdirectory(./subdir)

