# Wallpaper Engine Media Extensions

### Building

The following prerequisites need to be installed/available:

* CMake https://cmake.org/ (3.4+)
* Visual Studio 2017
* SFML 2 (static lib)
* mpg123 (static lib)

Compile prerequisites
* Compile OpenAL from https://github.com/kcat/openal-soft with LIBTYPE = STATIC
* Add to AL includes #define AL_LIBTYPE_STATIC
* Compile SFML2 statically, replace OpenAL lib with static lib above
* Compile mpg123 as static library

Clone this project and open it in CMake. Choose the Visual Studio 2017 generator for x86 or x64. Specify the following variables for the project inside CMake:

* OP_SFML2_LIB: Path to SFML 2 static libraries.
* OP_SFML2_INC: Path to SFML 2 include files.
* OP_MPG123_LIB: Path to mpg123 static libraries.
* OP_MPG123_INC: Path to mpg123 include files.
* OP_BINARY_OUTPUT_DIRECTORY: Output directory for binaries that Wallpaper Engine will load, should be [Your Steam path]/steamapps/common/wallpaper_engine/bin


