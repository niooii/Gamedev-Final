@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

IF NOT EXIST .\build\NUL MKDIR .\build

REM echo "Files:" %cFilenames%

SET assembly=gdf
SET compilerFlags=-g -Wvarargs -Wall
REM -Wall -Werror
SET includeFlags=-Isrc
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DGDFIMPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ./build/%assembly%.exe %defines% %includeFlags% %linkerFlags%