@echo off

echo Copy includes to compiler...
powershell Copy-Item -Path ".\amxmodx\scripting\include\*" -Destination "C:\AmxModX\1.9.0\include" -Recurse -Force

echo Prepare for compiling plugins...
if exist .\amxmodx\plugins rd /S /q .\amxmodx\plugins

mkdir .\amxmodx\plugins
cd .\amxmodx\plugins

set PLUGINS_LIST=..\configs\plugins-vipm.ini
echo. 2>%PLUGINS_LIST%

echo Compile plugins...

for /R ..\scripting\ %%F in (*.sma) do (
    echo.
    echo Compile %%~nF:
    amxx190 %%F

    if errorlevel 1 (
        echo.
        echo Plugin %%~nF compiled with error.
        set /p q=
        exit /b %errorlevel%
    )
    echo %%~nF.amxx>>%PLUGINS_LIST%
)


cd ..\..

echo Prepare files...
powershell Copy-Item -Path ".\amxmodx" -Destination ".\.build\VipModular" -Recurse
powershell Copy-Item -Path ".\README.md" -Destination ".\.build" -Recurse

echo Move prepared files to ZIP archive...
if exist .\VipModular.zip del .\VipModular.zip
cd .\.build
powershell Compress-Archive ./* .\..\VipModular.zip
cd ..
rmdir .\.build /s /q

echo Build finished.
