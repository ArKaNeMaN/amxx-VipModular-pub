@echo off

call config
call copy-includes

if not exist amxmodx\plugins mkdir amxmodx\plugins
cd amxmodx\plugins

echo.
%AMXX_COMPILER_EXECUTABLE_PATH% DEBUG=1 ..\scripting\VipModular.sma

if errorlevel 1 (
    echo.
    echo Core plugin compiled with error.
    set /p q=
    exit /b %errorlevel%
)
