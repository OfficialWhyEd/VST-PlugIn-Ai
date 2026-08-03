@echo off
setlocal enabledelayedexpansion
title WhyCremisi - aggiorna per Ableton

REM ---------------------------------------------------------------
REM  Un colpo solo: ricompila la UI, ricompila il plugin, e mette la
REM  versione nuova dove Ableton la trova.
REM
REM  Da lanciare ogni volta che si vuole provare in Live quello che
REM  abbiamo appena sviluppato. Ableton legge il file dal disco solo
REM  quando scansiona i plugin: se Live e' aperto, il file e' bloccato
REM  e la copia fallisce — per questo lo script controlla prima.
REM ---------------------------------------------------------------

cd /d "%~dp0.."

echo.
echo   [1/4] Controllo che Ableton sia chiuso...
tasklist /FI "IMAGENAME eq Ableton Live*" 2>nul | find /I "Ableton" >nul
if %errorlevel% equ 0 (
    echo.
    echo   [FERMO] Ableton Live e' aperto.
    echo   Il file del plugin e' in uso e non si puo' sovrascrivere.
    echo   Chiudi Live e rilancia questo script.
    echo.
    pause
    exit /b 1
)
echo         chiuso, si procede.

echo.
echo   [2/4] Compilo l'interfaccia React...
pushd webview-ui
call npm run build --silent
if %errorlevel% neq 0 (
    echo   [ERRORE] La UI non compila. Guarda i messaggi qui sopra.
    popd & pause & exit /b 1
)
popd
echo         fatta.

echo.
echo   [3/4] Compilo il plugin...
if "%JUCE_ROOT%"=="" set "JUCE_ROOT=E:\CARTELLE\JUCE"
cmake --build build-win --config Release --parallel 4 >"%TEMP%\whycremisi-build.log" 2>&1
if %errorlevel% neq 0 (
    echo   [ERRORE] Il plugin non compila. Dettagli in:
    echo   %TEMP%\whycremisi-build.log
    findstr /C:" error " "%TEMP%\whycremisi-build.log"
    pause & exit /b 1
)
echo         fatto.

echo.
echo   [4/4] Installo dove Ableton lo cerca...
net session >nul 2>&1
if %errorlevel% neq 0 (
    echo         servono i permessi di amministratore, li chiedo...
    powershell -NoProfile -Command "Start-Process -FilePath '%~dp0installa-vst3.cmd' -Verb RunAs -Wait"
) else (
    call "%~dp0installa-vst3.cmd"
)

echo.
echo   Pronto. Apri Ableton: se il plugin era gia' stato scansionato
echo   una volta, Live carica la versione nuova da solo.
echo   La prima volta invece serve: Preferenze - Plug-Ins - Rescan.
echo.
pause
