@ECHO OFF
SetLocal EnableDelayedExpansion

IF NOT EXIST .\build\NUL MKDIR .\build
IF NOT EXIST .\build\build_cache\NUL MKDIR .\build\build_cache
IF NOT EXIST .\build\build_cache\checksums\NUL MKDIR .\build\build_cache\checksums

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
    SET checksumFile=.\build\build_cache\checksums\%%~nf.md5
    IF NOT EXIST !oFilename! (
        ECHO Compiling %%f ...
        clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
        REM Store the checksum of the .c file
        certutil -hashfile %%f MD5 > !checksumFile!
    ) ELSE (
        REM Get the absolute paths
        SET absSrcFile=%%~ff
        SET absSrcFile=!absSrcFile:\=\\!
        SET absObjFile=%%~fo
        SET absObjFile=!absObjFile:\=\\!

        ECHO checking !absSrcFile!
        REM Get the current checksum of the .c file
        FOR /F "tokens=1,2" %%a IN ('certutil -hashfile %%f MD5 ^| FIND "MD5"') DO SET currentChecksum=%%b
        ECHO "CHECKSUM: !currentChecksum!"
        REM Read the stored checksum
        SET /P storedChecksum=<!checksumFile!

        REM Compare the timestamps and checksums to determine whether recompilation is needed
        IF !srcLastModified! GTR !objLastModified! (
            SET recompile=1
        ) ELSE IF NOT "!currentChecksum!" == "!storedChecksum!" (
            SET recompile=1
        ) ELSE (
            SET recompile=0
        )

        IF !recompile! == 1 (
            ECHO Compiling %%f ...
            clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
            REM Update the checksum of the .c file
            certutil -hashfile %%f MD5 > !checksumFile!
        )
    )
)

REM Link all the object files
clang %oFilenames% -o ./build/%assembly%.exe %linkerFlags%

ECHO Build complete.
