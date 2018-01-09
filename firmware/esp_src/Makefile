#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
# Note: For the IDF build system, your project is seen as the *main* component, alongside all the other components.

PROJECT_NAME := CH_Fishinator_ESP

#To include outside library eithe ruse EXTRA_COMPONENT_DIRS or symbolic link to components dirctory in project base dir.
#EXTRA_COMPONENT_DIRS = /CH/development/lib_global_ext/example_library  #Absolute path works, if only headers then they must be in an /include sub-dir

#You can include headers via the below commands, PRIV = private and is only used for this component, ADD = adds to all components
#NOTE: That both of these options are expaned to relative paths.
#COMPONENT_SRCDIRS := can include relative path src directories.
#COMPONENT_ADD_INCLUDEDIRS := /components/example_library/include	 # Works if symbolic link is used
#COMPONENT_PRIV_INCLUDEDIRS := /CH/development/lib_global_ext/example_library/include	 # Will not work
COMPONENT_ADD_INCLUDEDIRS := /components/Arduino-PID-Library	 # Works if symbolic link is used


#For adding headers with absolute paths, you must use the following commands with absolute paths.
COMPONENT_EXTRA_INCLUDEDIRS := /CH/development/lib_global_ext/example_library/include	 # Works


include ./components/esp-idf/make/project.mk

