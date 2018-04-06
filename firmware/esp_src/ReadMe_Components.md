##IDF Components: A method of includeing external libraries into your project.
In the standard build structure for the ESP-IDF SDK a project (in this template the top level being the ESP_src directory),
is to have a **main** folder with the projects source in a directory tree; and all other external libraries to
be included in the **components** directory.  For this template I have chosen to include both the ESP_IDF sdk and the arduino-esp32
libraries in this fashion. This allows for each project to freeze SDKs or libraries at a particular release/commit; or to modify those
libraries without inducing unwanted changes to other projects.

NOTE: It might be best to update the AAA_Template directory with the newest IDF/Arduino libraries, and propogate through a merge for a specific project.


### Adding libraries as components
1. The primary way of adding external libraries, and sdks to the project is via git submodules
    * NOTE: Most submodules are added from a local repo (e.g. esp-idf in /CH/development/sdks),
     so things like component.mk files can be added.
    * Add submodules `cd components && git submodule add /path_to_or_url`
    * Checkout a particular branch/tag and so you: `git checkout branch/tag` **IN** submodule dir.
        * You must then return **base** directory and `git commit -m "Updated submodule branch xxx' to save the setting.
        * Then you can run `git submodule sync && git submodule update --init --recursive`  to download the directory
    * To fast forward a submodule branch you navigate to its directories `components/xxx` and `git fetch`; `git pull`; 
   and if the submodule has submodules `git submodule update --recursive` in submodule directory
        * **THEN** commit the main project. 
    * Remove(hide/untrack), or add `git submodule deinit /path_to` or `git submodule init /path`
    * Submodule docs: https://git-scm.com/book/en/v2/Git-Tools-Submodules
    
1. Any external library with src files included in the components directory or as a sub directory in main must include 
a **components.mk** file; this file must be added to the top level of the library.

2. You can include the library in the indexer by adding the associated folders to the top level Cmakelists.txt file:
    * include_directories(components/library_to_include): if it symbolically linked.
    * include_directories(/abs/path/to/library_to_include)

2. You can define include paths, source paths, compiler and linker flags and other things.
    * Any paths are **_relative_** to the **components.mk** path where they are included, so generally to your project base directory.
    
3. See the makefile for examples of how to include libraries in your project. But include the following
    * EXTRA_COMPONENT_DIRS : = Abs path to external libraries containing headers and or src NOTE: if only headers than they must be in an /include dir.
    * COMPONENT_ADD_INCLUDEDIRS := Relative include paths for headers to add to all components. `EX: COMPONENT_ADD_INCLUDEDIRS := .` in a simple arduino library

    * COMPONENT_PRIV_INCLUDEDIRS := Visible to only this component.
    * COMPONENT_SRCDIRS :=  Additional src files needed for this component to compile, try not to use due to Brittleness
    
   ##### Refer to the build system documentation here: http://esp-idf.readthedocs.io/en/latest/api-guides/build-system.html
### Errata and notes about libraries:
- **Arduino-Esp32** libraries are using code from the **esp32_snippets** libraries and as such might not play
nice together.  This might need to adressed at a later date.
    - Currently removing one or the other seems to work just fine.


### Configuring esp-idf or arduino-esp32 to a specific release, IE freeze SDK version.
As both sdks are changing rapidly we might need to freeze the version to a specific realease or commit.
1) You can do this buy navigating to the submodule and checking out a specific release
* e.g. `cd /components/esp-idf && git checkout v2.0`
2) When you commit the main project it will update to save the same module, FREEZING the release!

Note this can be done in git kraken with the sub modules tab and selecting a particular commit, ie leave submodule in the detached head state.

##



TODO: Experiment with component specific build variables.
TODO: How to include precompiled libraries and archives.