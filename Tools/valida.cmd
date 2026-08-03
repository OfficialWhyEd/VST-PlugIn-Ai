@echo off
setlocal
title WhyCremisi - validazione

REM ---------------------------------------------------------------
REM  Il controllo che si fa prima di considerare buona una build.
REM
REM  pluginval carica il plugin come farebbe un DAW e lo sottopone a
REM  cio' che un host fa davvero: apertura a freddo e a caldo, editor
REM  aperto mentre l'audio scorre, salvataggio e ricarica dello stato,
REM  automazione fitta sui parametri, cambi di configurazione dei bus.
REM  Include anche il validatore ufficiale Steinberg per il VST3.
REM
REM  E' lo stesso strumento che usano i produttori di plugin
REM  commerciali: trova i crash che si manifestano solo dentro un host,
REM  che sono quelli che non si vedono provando a mano.
REM
REM  Livelli di severita': 1 minimo, 5 quello raccomandato prima di un
REM  rilascio, 10 il massimo (piu' lento, ripete i test con parametri
REM  casuali). Si passa come primo argomento, altrimenti 5.
REM ---------------------------------------------------------------

set "PV=E:\CARTELLE\pluginval\pluginval.exe"
set "LIVELLO=%~1"
if "%LIVELLO%"=="" set "LIVELLO=5"

REM Si valida la copia compilata, non quella installata: cosi' si
REM scoprono i problemi prima di metterla dove Ableton la legge.
set "VST=%~dp0..\build-win\WhyCremisiVSTPlugin_artefacts\Release\VST3\WhyCremisi.vst3"

if not exist "%PV%" (
    echo   [ERRORE] pluginval non c'e' in:
    echo   %PV%
    echo.
    echo   Si scarica da github.com/Tracktion/pluginval/releases
    pause & exit /b 1
)

if not exist "%VST%" (
    echo   [ERRORE] Il plugin compilato non c'e'. Compila prima.
    pause & exit /b 1
)

echo.
echo   Validazione a severita' %LIVELLO%. Puo' richiedere qualche minuto.
echo.

"%PV%" --strictness-level %LIVELLO% --validate-in-process --timeout-ms 300000 --validate "%VST%"

if %errorlevel% neq 0 (
    echo.
    echo   ESITO: qualcosa non ha superato il controllo.
    echo   Le righe con FAIL qui sopra dicono cosa.
    echo.
    pause & exit /b 1
)

echo.
echo   ESITO: superato. La build regge quello che un host le fa.
echo.
pause
