@echo off
set "VCPKG=C:\Users\Whyed\vcpkg\scripts\buildsystems\vcpkg.cmake"
set "JUCE=C:\Users\Whyed\Desktop\CARTELLE\JUCE"
set "VS_BAT=C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"

echo [STREGA] Caricamento ambiente MSVC 2026...
call "%VS_BAT%" x64

echo [STREGA] Pulizia e Configurazione (VS 2026)...
if exist build rd /s /q build
cmake -B build -G "Visual Studio 18 2026" -A x64 -DJUCE_ROOT="%JUCE%" -DCMAKE_TOOLCHAIN_FILE="%VCPKG%"

echo [STREGA] Compilazione (Standalone + VST3)...
cmake --build build --config Release --target OpenClawVSTPlugin_Standalone
cmake --build build --config Release --target OpenClawVSTPlugin_VST3

echo [STREGA] Ottimizzazione Artefatti...
set "EXE_PATH=build\OpenClawVSTPlugin_artefacts\Release\Standalone"
if exist "%EXE_PATH%\OpenClawVSTBridgeAI.exe" (
    move /y "%EXE_PATH%\OpenClawVSTBridgeAI.exe" "%EXE_PATH%\OpenClaw VST Bridge AI.exe"
)

echo [STREGA] Fine.
