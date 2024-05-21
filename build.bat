@ECHO OFF
SetLocal EnableDelayedExpansion

IF NOT EXIST .\build\NUL MKDIR .\build
IF NOT EXIST .\build\build_cache\NUL MKDIR .\build\build_cache

REM Get a list of all the .c files and corresponding object files
SET cFilenames=
SET oFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
    SET oFilename=.\build\build_cache\%%~nf.obj
    SET oFilenames=!oFilenames! !oFilename!
)


SET assembly=gdf
SET compilerFlags=-g -Wvarargs -Wall
REM -Wall -Werror
SET includeFlags=-Isrc
SET linkerFlags=-luser32 -lvulkan-1 -L%VULKAN_SDK%/Lib
SET defines=-D_DEBUG -DGDFIMPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%..."

REM Compile each .c file if the corresponding .obj file is missing or outdated
FOR %%f in (%cFilenames%) do (
    SET oFilename=.\build\build_cache\%%~nf.obj
    IF NOT EXIST !oFilename! (
        ECHO Compiling %%f ...
        clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
    ) ELSE (
        REM Check if the .c file is newer than the .obj file
        FOR %%c IN (%%f) DO FOR %%o IN (!oFilename!) DO (
            IF %%~tc GTR %%~to (
                ECHO Compiling %%f ...
                clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
            )
        )
    )
)

REM Link all the object files
clang %oFilenames% -o ./build/%assembly%.exe %linkerFlags%

ECHO Build complete.
