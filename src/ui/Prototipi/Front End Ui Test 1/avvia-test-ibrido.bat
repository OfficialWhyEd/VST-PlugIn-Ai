@echo off
setlocal
cd /d "%~dp0"

echo.
echo  [ HEARTBROKEN ] WHYCREMISI - AVVIO DIRETTO
echo  --------------------------------------------
echo  Caricamento UI locale (ES5, nessun server)
echo.

start "" "..\..\..\..\build\OpenClawVSTPlugin_artefacts\Release\Standalone\OpenClaw VST Bridge AI.exe"

echo Avviato.
