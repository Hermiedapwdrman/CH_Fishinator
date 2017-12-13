###Notes for adding external libraries as components to IDF config.
1. There are two ways to include external libraries (either headers or src) into your project
    * The preferred method of including external projects is to include them as git submodules
    * Otherwise a symbolic link *ln -s* or copy the library to the components directory in the base project directory
    * Utilize either EXTRA_COMPONENT_DIRS to link to the location of the library directory with the components.mk file

1. Any external library with src files included in the components directory or as a sub directory in main must include a **components.mk** file

2. You can include the library in the indexer by adding the associated folders to the top level Cmakelists.txt file:
    * include_directories(components/library_to_include): if it symbolically linked.
    * include_directories(/abs/path/to/library_to_include)

2. You can define include paths, source paths, compiler and linker flags and other things.
    * Any paths are **_relative_** to the **components.mk** path where they are included, so generally to your project base directory.
    
3. See the makefile for examples of how to include libraries in your project. But include the following
    * EXTRA_COMPONENT_DIRS : = Abs path to external libraries containing headers and or src NOTE: if only headers than they must be in an /include dir.
    * COMPONENT_ADD_INCLUDEDIRS := Relative include paths for headers to add to all components.
    * COMPONENT_PRIV_INCLUDEDIRS := Visible to only this component.
    * COMPONENT_SRCDIRS :=  Additional src files needed for this component to compile, try not to use due to Brittleness
    
   ##### Refer to the build system documentation here: http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html

### Configuring esp-idf or arduino-esp32 to a specific release, IE freeze SDK version.
As both sdks are changing rapidly we might need to freeze the version to a specific realease or commit.
1) You can do this buy navigating to the submodule and checking out a specific release
* e.g. `cd /components/esp-idf && git checkout v2.0`
2) When you commit the main project it will update to save the same module.

Note this can be done in git kracken with the sub modules tab and selecting a particular commit



TODO: Experiment with component specific build variables.
TODO: How to include precompiled libraries and archives.