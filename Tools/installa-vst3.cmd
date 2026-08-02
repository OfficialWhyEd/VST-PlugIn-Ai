@echo off
setlocal enabledelayedexpansion
title Installa WhyCremisi VST3

REM ---------------------------------------------------------------
REM  Copia il VST3 appena compilato nella cartella dei plugin di
REM  sistema, cosi' Ableton e le altre DAW lo trovano alla scansione.
REM  Serve l'elevazione: se manca, lo script si richiama da solo.
REM ---------------------------------------------------------------

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo Servono i permessi di amministratore, chiedo l'elevazione...
    powershell -NoProfile -Command "Start-Process -FilePath '%~f0' -Verb RunAs"
    exit /b
)

set "SRC=%~dp0..\build-win\WhyCremisiVSTPlugin_artefacts\Release\VST3\WhyCremisi.vst3"
set "DST=%CommonProgramFiles%\VST3\WhyCremisi.vst3"

echo.
echo   Origine:      %SRC%
echo   Destinazione: %DST%
echo.

if not exist "%SRC%" (
    echo [ERRORE] Il plugin compilato non c'e'.
    echo Compila prima con:  cmake --build build-win --config Release
    echo.
    pause
    exit /b 1
)

if exist "%DST%" (
    echo Rimuovo la versione precedente...
    rmdir /s /q "%DST%"
)

echo Copio...
xcopy "%SRC%" "%DST%\" /E /I /Y /Q >nul
if %errorlevel% neq 0 (
    echo [ERRORE] Copia non riuscita.
    pause
    exit /b 1
)

REM La UI React deve viaggiare insieme al plugin, altrimenti la
REM finestra si apre vuota.
if exist "%DST%\Contents\Resources\webview-ui\index.html" (
    echo   UI React inclusa.
) else (
    echo   [ATTENZIONE] Manca la UI React nel bundle.
    echo   Compilala con:  cd webview-ui ^&^& npm run build
    echo   poi ricompila il plugin e rilancia questo script.
)

echo.
echo   Fatto. In Ableton: Preferenze - Plug-Ins - Rescan.
echo   Il plugin appare come "WhyCremisi" fra gli effetti audio VST3.
echo.
pause
