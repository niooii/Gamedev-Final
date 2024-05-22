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
        if !errorlevel! neq 0 (
            exit /b !errorlevel!
        )
        REM Store the checksum of the .c file
        FOR /F "skip=1 tokens=*" %%a IN ('certutil -hashfile %%f MD5 ^| findstr /r /v "CertUtil: -hashfile"') DO ECHO %%a > !checksumFile!
    ) ELSE (
        REM Get the absolute paths
        @REM SET absSrcFile=%%~ff
        @REM SET absSrcFile=!absSrcFile:\=\\!
        @REM SET absObjFile=%%~fo
        @REM SET absObjFile=!absObjFile:\=\\!

        @REM ECHO checking !absSrcFile!
        REM Get the current checksum of the .c file
        FOR /F "skip=1 tokens=*" %%a IN ('certutil -hashfile %%f MD5 ^| findstr /r /v "CertUtil: -hashfile"') DO SET currentChecksum=%%a
        REM Read the stored checksum
        SET /P storedChecksum=<!checksumFile!
        SET storedChecksum=!storedChecksum: =!
        @REM ECHO "CHECKSUM: !currentChecksum!"
        @REM ECHO "STOREDCHECKSUM: !storedChecksum!"

        REM Compare the timestamps and checksums to determine whether recompilation is needed
        IF NOT "!currentChecksum!" == "!storedChecksum!" (
            SET recompile=1
        ) ELSE (
            SET recompile=0
            @REM ECHO Skipping file with no changes...
        )
        IF !recompile! == 1 (
            ECHO Compiling %%f ...
            clang -c %%f %compilerFlags% %defines% %includeFlags% -o !oFilename!
            if !errorlevel! neq 0 (
                exit /b !errorlevel!
            )
            REM Update the checksum of the .c file
            ECHO !currentChecksum! > !checksumFile!
        )
    )
)

REM Link all the object files
clang %oFilenames% -o ./build/%assembly%.exe %linkerFlags%

ECHO Build complete.
ECHO got to en defines