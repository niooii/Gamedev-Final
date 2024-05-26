@ECHO OFF
SetLocal EnableDelayedExpansion

.\build.exe

IF %ERRORLEVEL% EQU 0 (
    DEL .\build.exe

    COPY .\build\build.exe .\build.exe
)