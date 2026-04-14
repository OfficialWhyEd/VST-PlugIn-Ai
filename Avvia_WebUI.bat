@echo off
chcp 65001 >nul
title OpenClaw VST Bridge - WebUI Launcher
echo.
echo ╔═══════════════════════════════════════════════════════════╗
echo ║  OpenClaw VST Bridge - Launcher WebUI                   ║
echo ║  Per Edo & Heartbroken                                  ║
echo ╚═══════════════════════════════════════════════════════════╝
echo.

REM Controlla se siamo nella cartella corretta
if not exist "webview-ui\package.json" (
    echo [ERRORE] Non trovo webview-ui\package.json
    echo Assicurati di eseguire questo file dalla cartella principale del progetto.
    echo.
    pause
    exit /b 1
)

echo [INFO] Avvio WebUI...
echo [INFO] Percorso: %CD%\webview-ui
echo.

REM Vai nella cartella webview-ui e avvia npm
cd webview-ui

REM Controlla se node_modules esiste
if not exist "node_modules" (
    echo [AVVISO] node_modules non trovato. Installazione dipendenze...
    call npm install
    if errorlevel 1 (
        echo [ERRORE] Installazione fallita. Assicurati di avere Node.js installato.
        pause
        exit /b 1
    )
)

echo [INFO] Avvio server di sviluppo...
echo [INFO] Il browser si aprirà automaticamente su http://localhost:3000
echo.
echo ═══════════════════════════════════════════════════════════
echo Per fermare: chiudi questa finestra o premi Ctrl+C
echo ═══════════════════════════════════════════════════════════
echo.

REM Avvia npm run dev
call npm run dev

REM Se npm run dev termina, mostra messaggio
if errorlevel 1 (
    echo.
    echo [ERRORE] Il server si è fermato con un errore.
    pause
) else (
    echo.
    echo [INFO] Server fermato.
    pause
)
