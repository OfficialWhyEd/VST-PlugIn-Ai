@echo off
chcp 65001 >nul
title OpenClaw VST Bridge - Build Windows
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║  OpenClaw VST Bridge - Build Script Windows x64         ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

REM Configurazione
set "JUCE_ROOT=C:\SDKs\JUCE"
set "BUILD_DIR=build\windows"
set "RELEASE_DIR=releases\windows"

REM Verifica JUCE
if not exist "%JUCE_ROOT%\CMakeLists.txt" (
    echo [ERRORE] JUCE non trovato in: %JUCE_ROOT%
    echo Modifica questa variabile all'inizio dello script.
    pause
    exit /b 1
)

REM Verifica CMake
where cmake >nul 2>nul
if errorlevel 1 (
    echo [ERRORE] CMake non trovato nel PATH.
    echo Installa CMake e aggiungilo al PATH.
    pause
    exit /b 1
)

echo [INFO] JUCE trovato in: %JUCE_ROOT%
echo [INFO] Build directory: %BUILD_DIR%
echo.

REM Crea cartelle
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"

REM Entra nella cartella build
cd "%BUILD_DIR%"

REM Configura con CMake
echo [INFO] Configurazione CMake...
cmake ..\.. -DJUCE_ROOT="%JUCE_ROOT%" -DCMAKE_BUILD_TYPE=Release -A x64

if errorlevel 1 (
    echo.
    echo [ERRORE] CMake configuration fallita!
    pause
    exit /b 1
)

REM Compila
echo.
echo [INFO] Compilazione in corso...
cmake --build . --config Release --parallel

if errorlevel 1 (
    echo.
    echo [ERRORE] Build fallita!
    pause
    exit /b 1
)

cd ..\..

REM Copia il risultato nella cartella releases
set "VST3_SOURCE=%BUILD_DIR%\OpenClawVSTPlugin_artefacts\Release\VST3\OpenClawVSTBridgeAI.vst3"
set "VST3_DEST=%RELEASE_DIR%\OpenClawVSTBridgeAI.vst3"

if exist "%VST3_SOURCE%" (
    xcopy /E /I /Y "%VST3_SOURCE%" "%VST3_DEST%" >nul
    echo.
    echo ═══════════════════════════════════════════════════════════
    echo [SUCCESSO] Build completata!
    echo.
    echo File VST3 trovato in:
    echo   %VST3_DEST%
    echo.
    echo Per installare in Ableton, copia la cartella in:
    echo   C:\Program Files\Common Files\VST3\
    echo ═══════════════════════════════════════════════════════════
) else (
    echo.
    echo [ERRORE] File VST3 non trovato dopo la build.
    echo Percorso atteso: %VST3_SOURCE%
)

echo.
pause
