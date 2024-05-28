@ECHO OFF
SetLocal EnableDelayedExpansion

.\build.exe -F"build_builder_opts.gdf"

IF %ERRORLEVEL% EQU 0 (
    DEL .\build.exe

    COPY .\build\build.exe .\build.exe
)