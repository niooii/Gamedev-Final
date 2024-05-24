# Gamedev-Final

Literally my gamedev final.

# How to build
1. Install vulkansdk
2. Install clang (llvm) and add to path
3. .\build.exe (_build.bat builds build.exe in case anything goes wrong, build.exe should be able to build itself)

### To build build.exe:
Add the -DCOMPILE_BUILDER define to the BUILDER_DEFINES field in build_options.gdf. Change executable name accordingly, then run .\build.exe. Remove the define to build the actual game.

# Todos:
- Have a resources folder outside of the build directory, then copy all resources on build. I keep deleting the resources folder by accident. I will scream if it happens again.