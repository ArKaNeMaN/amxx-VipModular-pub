@echo off

echo /============ PREPARE =============
echo /

xcopy .\amxmodx\scripting\include C:\AmxModX\1.9.0\include /s /e /y

if exist .\amxmodx\plugins rd /S /q .\amxmodx\plugins
mkdir .\amxmodx\plugins
cd .\amxmodx\plugins

set PLUGINS_LIST=..\configs\plugins-vipm.ini
echo. 2>%PLUGINS_LIST%

echo /
echo /
echo /============ COMPILE =============
echo /

for /R ..\scripting\ %%F in (*.sma) do (
    echo / /
    echo / / Compile %%~nF:
    echo / /
    amxx190 %%F
    echo %%~nF.amxx>>%PLUGINS_LIST%
)

echo /
echo /
echo /============ BUILD =============
echo /

cd ..\..
mkdir .\.build\VipModular\amxmodx\scripting\

xcopy .\amxmodx\scripting\include\ .\.build\VipModular\amxmodx\scripting\include\ /s /e /y
xcopy .\amxmodx\data\ .\.build\VipModular\amxmodx\data\ /s /e /y
xcopy .\amxmodx\configs\ .\.build\VipModular\amxmodx\configs\ /s /e /y
xcopy .\amxmodx\plugins\ .\.build\VipModular\amxmodx\plugins\ /s /e /y
copy .\README.md .\.build\

if exist .\VipModular.zip del .\VipModular.zip
cd .\.build
zip -r .\..\VipModular.zip .
cd ..
rmdir .\.build /s /q

echo /
echo /
echo /============ END =============
echo /

set /p q=