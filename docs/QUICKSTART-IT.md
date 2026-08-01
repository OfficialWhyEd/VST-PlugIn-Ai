# Guida rapida — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

---

## Cos'è

Un plugin (VST3 / AU / Standalone) che si carica sul master della sessione e fa da co-pilota AI: legge l'audio, parla col DAW via OSC, espone i parametri di tutti i plugin caricati e li rende manovrabili da un'AI. L'interfaccia è React, dentro una WebView.

---

## Dove trovare le cose

| Cosa cerchi | File |
|-------------|------|
| Come si lavora al progetto | `docs/WORKFLOW.md` |
| Stato attuale | `docs/STATUS.md` |
| I 100 step di sviluppo | `docs/ROADMAP.md` |
| Architettura | `docs/ARCHITECTURE.md` |
| Protocollo di comunicazione | `docs/project/protocol-json-v1.md` |
| Documenti di prodotto (IT + EN) | `Research/italiano/`, `Research/inglese/` |
| Loghi e icone | `Research/logo/` |

---

## Compilare

### Prerequisiti

| | Windows | macOS | Linux |
|---|---|---|---|
| Compilatore | Visual Studio Build Tools 2022+ | Xcode Command Line Tools | GCC/Clang |
| CMake | 3.20+ | 3.20+ | 3.20+ |
| JUCE | 8.0.5+ | 8.0.5+ | 8.0.5+ |
| Node | 20+ | 20+ | 20+ |
| Extra | pacchetto NuGet `Microsoft.Web.WebView2` | — | `libwebkit2gtk-4.1`, `gtk+-3.0` |

JUCE si trova tramite la variabile d'ambiente `JUCE_ROOT`, oppure dai percorsi noti elencati in cima al `CMakeLists.txt`.

### Prima la UI, poi il plugin

La UI React va compilata **prima**: il `CMakeLists.txt` copia `webview-ui/dist` dentro il bundle del plugin, e se la dist non esiste il plugin si apre vuoto.

```bash
cd webview-ui
npm install
npm run build
cd ..
```

### Windows

```powershell
$env:JUCE_ROOT = "E:\CARTELLE\JUCE"
cmake -S . -B build-win -G "Visual Studio 18 2026" -A x64
cmake --build build-win --config Release --parallel 4
```

Se CMake non trova WebView2:

```powershell
Register-PackageSource -provider NuGet -name nugetRepository -location https://www.nuget.org/api/v2
Install-Package Microsoft.Web.WebView2 -Scope CurrentUser -RequiredVersion 1.0.1901.177 -Source nugetRepository
```

Oppure scompatta il `.nupkg` dove vuoi e passa `-DJUCE_WEBVIEW2_PACKAGE_LOCATION="percorso/della/cartella"`.

Il VST3 esce in `build-win/WhyCremisiVSTPlugin_artefacts/Release/VST3/WhyCremisi.vst3` — copialo in `C:\Program Files\Common Files\VST3\`.

### macOS

```bash
export JUCE_ROOT=/Users/whyed/Documents/Dev/SDKs/JUCE
cmake -B build
cmake --build build --config Release
cp -r build/WhyCremisiVSTPlugin_artefacts/Release/VST3/WhyCremisi.vst3 ~/Library/Audio/Plug-Ins/VST3/
```

---

## Sviluppare la UI col live reload

Il plugin, all'apertura, cerca un dev server Vite su `localhost:5173` e poi su `4173`. Se lo trova lo usa al posto del bundle interno — quindi puoi modificare la UI e vedere il risultato dentro il DAW senza ricompilare il C++.

```bash
cd webview-ui
npm run dev
```

Poi apri il plugin nel DAW.

---

## Impostare l'OSC in Reaper

Options → Preferences → Control/OSC/web → Add → OSC:

| Campo | Valore |
|-------|--------|
| Device port | 9000 |
| Device IP | 127.0.0.1 |
| Local listen port | 8000 |

Il plugin ascolta su 9000 e invia sulla porta 8000 dell'IP configurato.

---

## Se qualcosa va storto

- **Il plugin si apre ma è vuoto** → manca `webview-ui/dist`: compila la UI e ricompila il plugin
- **`withResourceProvider` non compila** → WebView2 non è attivo, vedi la sezione Windows sopra
- **Il DAW non risponde ai comandi** → controlla porte e IP OSC; il plugin logga in console cosa invia
- **Hai cancellato qualcosa per sbaglio** → `git reflog`, poi `git checkout <hash>`
