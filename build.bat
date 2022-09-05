@echo off

call config
call copy-includes

echo Cleanup old compiled plugins...

if exist amxmodx\plugins rd /S /q amxmodx\plugins

set PLUGINS_LIST=amxmodx\configs\plugins-%PACKAGE_PLUINGS_LIST_POSTFIX%.ini
if exist %PLUGINS_LIST% del %PLUGINS_LIST%

if not "%PACKAGE_WITH_COMPILED_PLUGINS%" == "1" goto after-compile

echo Prepare for compiling plugins...

mkdir amxmodx\plugins
cd amxmodx\plugins

set PLUGINS_LIST=..\configs\plugins-%PACKAGE_PLUINGS_LIST_POSTFIX%.ini
if "%PACKAGE_PLUINGS_LIST_USE%" == "1" (
    echo. 2>%PLUGINS_LIST%
)

echo Compile plugins...

for /R ..\scripting\ %%F in (*.sma) do (
    echo.
    echo Compile %%~nF:
    
    if "%PACKAGE_DEBUG%" == "1" (
       %AMXX_COMPILER_EXECUTABLE_PATH% DEBUG=1 %%F
    ) else (
       %AMXX_COMPILER_EXECUTABLE_PATH% %%F
    )

    if errorlevel 1 (
        echo.
        echo Plugin %%~nF compiled with error.
        set /p q=
        exit /b %errorlevel%
    )

    if "%PACKAGE_PLUINGS_LIST_USE%"=="1" (
       echo %%~nF.amxx>>%PLUGINS_LIST%
    )
)

cd ..\..

:after-compile

echo Prepare files...
powershell Copy-Item -Path ".\amxmodx" -Destination ".\.build\%PACKAGE_NAME%" -Recurse

if "%PACKAGE_README_USE%" == "1" (
    powershell Copy-Item -Path ".\README.md" -Destination ".\.build" -Recurse
)

echo Move prepared files to ZIP archive...
if exist .\%PACKAGE_NAME%.zip del .\%PACKAGE_NAME%.zip
cd .\.build
powershell Compress-Archive ./* .\..\%PACKAGE_NAME%.zip
cd ..
rmdir .\.build /s /q

echo Build finished.
