# Gamedev-Final

Literally my gamedev final.

# How to build
1. Install vulkansdk
2. Install clang (llvm) and add to path
3. .\build.exe

### To build build.exe:
Add the -DCOMPILE_BUILDER define to the BUILDER_DEFINES field in build_options.gdf. Change executable name to "build", then run .\_build.bat to build and auto-replace .\build.exe. Remove the define to build the actual game, executable name should be "gdf" in build options. 

# Todos:
- Have a resources folder outside of the build directory, then copy all resources on build. I keep deleting the resources folder by accident. I will scream if it happens again.
- Implement heap shrinking the heap keeps growing forever rn wtf