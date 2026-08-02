# Stato del progetto — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

---

## In breve

Il plugin esiste, è grosso e funziona su macOS. Su Windows non compilava più: in questa sessione è stato rimesso in piedi. La UI React è completa e si compila ovunque. Il grosso del lavoro rimasto è verifica sul campo dentro i DAW, test automatici e CI.

---

## Sessione 1 agosto 2026 — ritorno su Windows

Il progetto era nato su Windows, è proseguito su Mac da maggio a giugno, e a fine luglio è tornato su Windows. In mezzo il team si è ridotto a una persona.

### Stato dei branch

I rami `master` e `heartbroken-claude` si sono separati il 09/05/2026 al commit `811bae3`:

- `heartbroken-claude` — 50 commit di codice: riorganizzazione del progetto, sistema logo completo, AI function calling, gestione del contesto, stabilità, fix crash, UI widget. **È il ramo con il codice vero.**
- `master` — 30 commit del 6-7 giugno: README, branding, landing page GitHub Pages. Nessun codice nuovo.

`heartbroken` e `aura` sono interamente contenuti in `heartbroken-claude`.

**I due rami vanno ancora riunificati.**

### Build Windows sbloccata

Quattro problemi, tutti risolti:

| Problema | File | Soluzione |
|---|---|---|
| `SIGPIPE` non esiste su Windows | `src/core/PluginProcessor.cpp` | guard `#if ! JUCE_WINDOWS` |
| `withResourceProvider` non compilava | `CMakeLists.txt` | `NEEDS_WEBVIEW2 TRUE` + `JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1` |
| UI React cercata solo nel bundle macOS | `src/core/PluginEditor.cpp` | nuova `findUIDirectory()` che prova bundle macOS, VST3 Windows e cartella accanto all'eseguibile |
| dist React copiata solo `if(APPLE)` | `CMakeLists.txt` | ramo Windows che la copia nel VST3 e accanto allo standalone |

In più: il log di debug era hardcoded su `/tmp/whycremisi-debug.log`, percorso inesistente su Windows. Ora passa da `whycremisi::debugLogFile()` (`src/debug/DebugLog.h`) che usa la cartella temporanea di sistema.

### Rami riunificati, e cosa è saltato fuori

`master` e `heartbroken-claude` sono stati riuniti. Il merge ha fatto emergere due difetti che nessuno aveva visto perché **`master` non era mai stato compilato dopo i commit di branding di giugno**:

- **Il plugin non compilava affatto su `master`.** Il commit `9ee5bf5` ("watermark across all project files") aveva sostituito il nome del plugin *dentro il codice*, non solo nei commenti: `getName()` in `PluginProcessor.h` restituiva una stringa letterale spezzata su due righe. Ora restituisce `JucePlugin_Name`.

- **L'escaping JavaScript non funzionava.** In `WebViewBridge.cpp` un commento terminava con un backslash, che in C++ continua il commento sulla riga successiva: la `replace` che fa l'escaping delle virgolette doppie finiva dentro il commento e non veniva mai eseguita. Ogni virgoletta doppia nei messaggi verso la WebView passava non escapata.

- **MSVC e i caratteri accentati.** 25 sorgenti su 57 contengono simboli non-ASCII (`©`, `™`, `—`) nei commenti e nel watermark. Clang assume UTF-8, MSVC no: senza `/utf-8` interpreta quei byte con la codepage locale e va in errore di sintassi. Il flag è stato aggiunto al `CMakeLists.txt`.

### Il plugin è stato provato con audio vero, per la prima volta

Caricando il VST3 in `pedalboard` (Python) e misurando l'uscita è emerso che **il plugin alterava il suono appena inserito**: con i default di fabbrica il compressore lavorava 4:1 sopra −24 dB e il limiter tagliava a −6 dB, entrambi `enabled = true` e non bypassati. Un tono a −9 dBFS RMS usciva a −21,7 dBFS, con il picco intatto — la firma di una compressione ad attacco lento.

Il punto non è la qualità del DSP, che funziona: è che nessuno l'aveva chiesto. WhyCremisi si carica sul master e nasce per leggere e suggerire, quindi deve essere trasparente finché non gli si chiede di intervenire. E soprattutto non c'era modo di spegnerlo: `DSPEngine::setBypass()` non è chiamato da nessuna parte e nel bridge OSC non esiste un comando che regoli soglia o ratio. Il `CompressorBox` della UI manda messaggi che non raggiungono il DSP interno.

Ora EQ, compressore e limiter partono bypassati; l'analyzer resta attivo perché serve al metering e non tocca l'audio. Cablare i moduli alla UI e all'AI è in `TODO.md`.

Verifica automatica: con bypass del DAW il delta è 0,00 dB, nessun NaN in uscita, nessun crash su blocchi consecutivi, 16 parametri esposti correttamente.

### La UI React è stata vista girare su Windows, per la prima volta

Avviando lo standalone la finestra si apriva ma mostrava *"L'esplorazione della pagina Web è stata annullata"*: il messaggio di **Internet Explorer**. Su Windows il backend di default di JUCE è ancora IE, e attivare WebView2 nel `CMakeLists.txt` lo rende disponibile ma non lo seleziona — va chiesto esplicitamente con `Backend::webview2`. Il codice passava `defaultBackend`, corretto su macOS (WebKit), fatale su Windows.

Corretto quello, la UI si carica: 6 processi WebView2 attivi e la schermata di setup "Neural Link" con la scelta del provider AI e del modello.

Aggiunta anche la user data folder di WebView2 sotto `%APPDATA%\WhyCremisi\WebView2`: di default WebView2 la crea accanto all'eseguibile, e una volta installato il VST3 in `Program Files` fallirebbe in silenzio.

### Documentazione separata dal sito

Il merge aveva fatto finire nella stessa cartella `docs/` sia la landing page GitHub Pages sia la documentazione tecnica. Ora `docs/` contiene solo il sito, la documentazione sta in `Documentazione/`.

### Regole del progetto riscritte

`WORKFLOW.md`, `QUICKSTART-IT.md` e `TODO.md` erano costruiti sul team a tre (Edo + Carlo/Aura + Heartbroken): branch personali, prefissi obbligatori nei commit, aree di codice vietate, review incrociata. Carlo non lavora più al progetto: quell'impianto è stato rimosso e sostituito con un flusso a una persona.

---

## Cosa funziona

- **Bridge OSC** plugin → DAW: play, stop, record verificati su Reaper
- **WebSocket server** RFC 6455 scritto a mano, handshake e riconnessione con backoff
- **UI React** completa: BotFace, SessionPanel, 13 box modulari, sistema widget, tema chiaro/scuro, ricerca, export/import sessione
- **Motore AI** multi-provider (Groq, Gemini, Anthropic, OpenAI, OpenRouter, Ollama) con function calling e gestione del contesto
- **DSP**: analyzer FFT, LUFS, correlazione stereo, compressore, limiter, EQ
- **Memoria dell'agente**: flight recorder degli eventi di sessione, personalità, workspace
- **MIDI learn** e mappatura parametri
- **Build**: VST3 + AU + Standalone su macOS; VST3 + Standalone su Windows

## Cosa non è mai stato verificato davvero

- Il plugin caricato in **Ableton** (su Reaper sì)
- Il **feedback OSC dal DAW verso il plugin** (la direzione inversa)
- Il comportamento su **Windows dentro un DAW reale** — compila, ma non è ancora stato provato in sessione
- Il **supporto Ableton**: `AbletonDawHandler` è poco più di uno scheletro

---

## Roadmap

`Documentazione/ROADMAP.md` contiene 100 step in 7 fasi.

**Completati:** 1-49, 52, 54, 55, 57, 58, 60, 63, 64, 65, 75, 76, 77
**Aperti:** 50, 51, 53, 56, 59, 61, 62, 66-74, 78-100

Il blocco 78-100 è quello che pesa: test React, test end-to-end, CI, changelog, documentazione API, e tutta la fase 7 (feature avanzate).

---

## Ambiente di sviluppo

| | Windows | macOS |
|---|---|---|
| Copia di lavoro | `E:\Dev\WhyCremisi` | — |
| JUCE | 8.0.4 in `E:\CARTELLE\JUCE` | 8.0.12 |
| Compilatore | MSVC 14.50 (Build Tools 2026) | Xcode CLT |
| CMake | 4.3.1 | — |
| Node | 24.15 | — |
| WebView2 | pacchetto NuGet in `E:\CARTELLE\NuGet` | non serve |

Le due versioni di JUCE andrebbero allineate: le API della WebView cambiano fra 8.0.4 e 8.0.12.
