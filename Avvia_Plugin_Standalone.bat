@echo off
chcp 65001 >nul
title OpenClaw VST Bridge - Plugin Standalone Launcher
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║  OpenClaw VST Bridge - Launcher Plugin Standalone       ║
echo ║  Per Edo & Heartbroken                                  ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.
echo Questo avvia il plugin in modalita' standalone (senza DAW).
echo Avvia WebSocket server sulla porta 8080 e OSC su 9000/9001.
echo.
echo [AVVISO] Devi aver compilato il plugin prima!
echo.

REM Controlla se siamo nella cartella corretta
if not exist "build\OpenClawVSTPlugin_artefacts\Release\OpenClawVSTBridgeAI.exe" (
    echo [ERRORE] Plugin standalone non trovato.
    echo Percorso atteso: build\OpenClawVSTPlugin_artefacts\Release\OpenClawVSTBridgeAI.exe
    echo.
    echo Devi compilare il plugin prima con:
    echo   mkdir build
    echo   cd build
    echo   cmake .. -DJUCE_ROOT=C:\Percorso\JUCE
    echo   cmake --build . --config Release
    echo.
    pause
    exit /b 1
)

echo [INFO] Avvio Plugin Standalone...
echo [INFO] WebSocket: ws://localhost:8080
echo [INFO] OSC In:    udp://localhost:9000
echo [INFO] OSC Out:   udp://localhost:9001
echo.
echo ═══════════════════════════════════════════════════════════
echo Assicurati di avviare anche Avvia_WebUI.bat in un altro terminale!
echo ═══════════════════════════════════════════════════════════
echo.

REM Avvia il plugin
cd build\OpenClawVSTPlugin_artefacts\Release
start OpenClawVSTBridgeAI.exe

if errorlevel 1 (
    echo.
    echo [ERRORE] Avvio fallito.
    pause
    exit /b 1
) else (
    echo.
    echo [INFO] Plugin avviato!
    echo [INFO] Puoi chiudere questa finestra, il plugin rimane in esecuzione.
    echo.
    pause
)
