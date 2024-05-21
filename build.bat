@ECHO OFF
SetLocal EnableDelayedExpansion

IF NOT EXIST .\build\NUL MKDIR .\build
IF NOT EXIST .\build\build_cache\NUL MKDIR .\build\build_cache

REM Get a list of all the .c files and corresponding object files
SET cFilenames=
SET oFilenames=
FOR /R %%f IN (*.c) DO (
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
FOR %%f IN (%cFilenames%) DO (
    SET oFilename=.\build\build_cache\%%~nf.obj
    IF NOT EXIST !oFilename! (
        ECHO Compiling %%f ...
        clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
    ) ELSE (
        REM Get the absolute paths
        FOR %%c IN (%%f) DO (
            SET absSrcFile=%%~fc
            SET absSrcFile=!absSrcFile:\=\\!
        )
        FOR %%o IN (!oFilename!) DO (
            SET absObjFile=%%~fo
            SET absObjFile=!absObjFile:\=\\!
        )
        ECHO checking !absSrcFile!
        FOR /F "tokens=2 delims==" %%c IN ('wmic datafile where "name='!absSrcFile!'" get LastModified /VALUE ^| FIND "="') DO SET srcLastModified=%%c
        FOR /F "tokens=2 delims==" %%o IN ('wmic datafile where "name='!absObjFile!'" get LastModified /VALUE ^| FIND "="') DO SET objLastModified=%%o
        
        IF !srcLastModified! GTR !objLastModified! (
            ECHO Compiling %%f ...
            clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
        )
    )
)

REM Link all the object files
clang %oFilenames% -o ./build/%assembly%.exe %linkerFlags%

ECHO Build complete.
