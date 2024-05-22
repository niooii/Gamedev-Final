@ECHO OFF
SetLocal EnableDelayedExpansion

REM runs after building
.\build.bat 
if !errorlevel! neq 0 (
    exit /b !errorlevel!
)
.\build\gdf.exe