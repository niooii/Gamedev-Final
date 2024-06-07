@ECHO OFF
SetLocal EnableDelayedExpansion

IF NOT EXIST .\build\NUL MKDIR .\build

REM Get a list of all the .c files and corresponding object files
SET cFilenames=
FOR /R %%f IN (*.c) DO (
    SET cFilenames=!cFilenames! %%f
    SET oFilename=.\build\build_cache\%%~nf.obj
    SET oFilenames=!oFilenames! !oFilename!
)

SET assembly=build
SET compilerFlags=-g -Wvarargs -Wall -Ofast
REM -Wall -Werrorrr
SET includeFlags=-Isrc -I%VULKAN_SDK%/Include
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -D_CRT_SECURE_NO_WARNINGS -DCOMPILE_BUILDER -DGDF_DEBUG

REM Link all the object files
ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ./build/%assembly%.exe %defines% %includeFlags% %linkerFlags%

ECHO Build complete.

DEL .\build.exe

COPY .\build\build.exe .\build.exe