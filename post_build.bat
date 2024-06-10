@echo off
SetLocal EnableDelayedExpansion

if not exist "%cd%\build\resources\shaders\" mkdir "%cd%\build\resources\shaders"

echo Compiling shaders...

for %%F in (resources/shaders/*.vert resources/shaders/*.frag) do (
    echo Compiling %%F...
    setlocal enabledelayedexpansion

    set "STAGE="
    if /I "%%~xF"==".vert" set "STAGE=vert"
    if /I "%%~xF"==".frag" set "STAGE=frag"

    if defined STAGE (
        %VULKAN_SDK%\bin\glslc.exe -fshader-stage=!STAGE! .\resources\shaders\%%F -o ./build/resources/shaders/%%F.spv
        if !ERRORLEVEL! NEQ 0 (
            echo Failed to compile shaders. Error: !ERRORLEVEL! && exit /b !ERRORLEVEL!
        )
    ) else (
        echo "Skipping %%F: Unknown shader stage"
    )

    endlocal
)

echo Copying resources...
xcopy "resources" "build\resources" /h /i /k /e /r /y
if !ERRORLEVEL! NEQ 0 (
    echo Error copying resources: !ERRORLEVEL!
    exit /b !ERRORLEVEL!
)