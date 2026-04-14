@echo off
setlocal
set "UI_DIR=C:\Users\Whyed\Desktop\VST-PlugIn-Ai\build\OpenClawVSTPlugin_artefacts\Release\Standalone\webview-ui\dist"
set "VST_EXE=C:\Users\Whyed\Desktop\VST-PlugIn-Ai\build\OpenClawVSTPlugin_artefacts\Release\Standalone\OpenClaw VST Bridge AI.exe"

echo [WhyCremisi] Avvio server UI locale (porta 8765)...
start "WHYCREMISI_SERVER" cmd /c "cd /d \"%UI_DIR%\" && python -m http.server 8765"

echo [WhyCremisi] Attesa 2 secondi per il server...
timeout /t 2 /nobreak > nul

echo [WhyCremisi] Apertura VST...
start "" "%VST_EXE%"
