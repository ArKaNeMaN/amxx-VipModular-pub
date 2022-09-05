@echo off

echo Copy includes to compiler...
set PACKAGE_INCLUDES_DIR=amxmodx\scripting\include
if exist "%PACKAGE_INCLUDES_DIR%" (
    powershell Copy-Item -Path "%PACKAGE_INCLUDES_DIR%\*" -Destination "%AMXX_COMPILER_DIR%\include" -Recurse -Force
)
