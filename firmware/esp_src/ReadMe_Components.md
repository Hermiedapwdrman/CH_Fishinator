##Notes for adding external libraries as components to IDF config.

1) Any external library included in the components directory or as a sub directory must include a **components.mk** file

2) You can define  include paths, source paths, compiler and linker flags and other things.
    * Any paths are relative to the **components.mk** path.
    * Refer to the build system documentation here: http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html
    
3) As a rule of thumb you will want to include for the build system to attempt to compile library sources.
    * COMPONENT_ADD_INCLUDEDIRS := ./dir1 /dir1/dir2  etc...
    * COMPONENT_SRCDIRS :=  ./srcdir1 etc...
    

TODO: Experiment with component specific build variables.
TODO: How to include precompiled libraries and archives.