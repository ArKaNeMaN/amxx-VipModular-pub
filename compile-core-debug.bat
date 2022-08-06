@echo off

echo /============ PREPARE =============
echo /

xcopy .\amxmodx\scripting\include C:\AmxModX\1.9.0\include /s /e /y

if not exist .\amxmodx\plugins mkdir .\amxmodx\plugins
cd .\amxmodx\plugins

echo /
echo /
echo /============ COMPILE =============
echo /

echo / /
echo / / Compile %%~nF:
echo / /
amxx190 DEBUG=1 ..\scripting\VipModular.sma

echo /
echo /
echo /============ END =============
echo /

set /p q=