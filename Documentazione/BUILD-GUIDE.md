# Build guide — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

Guida completa per Windows, macOS e Linux. Per la versione breve vedi [`QUICKSTART-IT.md`](QUICKSTART-IT.md).

---

## Prerequisiti comuni

- **CMake** ≥ 3.20
- **JUCE 8** — [juce.com/get-juce](https://juce.com/get-juce)
- **Node** ≥ 20 (per la UI React)
- **Git**

cURL non serve: le chiamate HTTP verso i provider AI usano il client nativo di JUCE. Nemmeno OpenSSL serve: lo SHA1 dell'handshake WebSocket è implementato dentro il progetto, così la build resta identica su tutte le piattaforme.

### Dove mettere JUCE

Il `CMakeLists.txt` cerca JUCE in quest'ordine:

1. variabile d'ambiente `JUCE_ROOT`
2. una lista di percorsi noti scritti in cima al file

Se lavori su una macchina nuova, imposta la variabile d'ambiente oppure aggiungi il tuo percorso alla lista.

```bash
export JUCE_ROOT=/percorso/di/JUCE          # macOS / Linux
```
```powershell
$env:JUCE_ROOT = "E:\CARTELLE\JUCE"          # Windows, sessione corrente
setx JUCE_ROOT "E:\CARTELLE\JUCE"            # Windows, permanente
```

---

## Passo 1 — la UI React

**Va fatta sempre per prima.** Il `CMakeLists.txt` copia `webview-ui/dist` dentro il bundle del plugin al momento del build; se la dist non c'è, il plugin si apre su una finestra vuota.

```bash
cd webview-ui
npm install
npm run build
cd ..
```

---

## Passo 2 — il plugin

### Windows

Oltre ai prerequisiti comuni servono:

- **Visual Studio Build Tools 2022 o 2026**, workload "Desktop development with C++"
- il pacchetto NuGet **Microsoft.Web.WebView2** — JUCE lo linka staticamente per la WebView. Senza, l'intera API `Options::withResourceProvider` viene esclusa dalla compilazione e il plugin non compila.

```powershell
Register-PackageSource -provider NuGet -name nugetRepository -location https://www.nuget.org/api/v2
Install-Package Microsoft.Web.WebView2 -Scope CurrentUser -RequiredVersion 1.0.1901.177 -Source nugetRepository
```

In alternativa scarica il `.nupkg` da nuget.org, scompattalo dove vuoi (è uno zip) e passa la cartella *contenitore* a CMake:

```powershell
cmake -S . -B build-win -G "Visual Studio 18 2026" -A x64 -DJUCE_WEBVIEW2_PACKAGE_LOCATION="E:/CARTELLE/NuGet"
```

Build:

```powershell
$env:JUCE_ROOT = "E:\CARTELLE\JUCE"
cmake -S . -B build-win -G "Visual Studio 18 2026" -A x64
cmake --build build-win --config Release --parallel 4
```

Risultato in `build-win/WhyCremisiVSTPlugin_artefacts/Release/`:
- `VST3/WhyCremisi.vst3` (la UI React è dentro, in `Contents/Resources/webview-ui`)
- `Standalone/WhyCremisi.exe` (la UI sta nella cartella `webview-ui` accanto all'exe)

Nota: su Windows JUCE non genera il formato AU — è esclusivo di macOS, e viene ignorato senza errori.

### macOS

```bash
export JUCE_ROOT=/percorso/di/JUCE
cmake -B build
cmake --build build --config Release
```

Risultato in `build/WhyCremisiVSTPlugin_artefacts/Release/`: `VST3/`, `AU/`, `Standalone/`.

### Linux

Dipendenze di sistema:

```bash
sudo apt install libasound2-dev libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libgl-dev libfreetype6-dev libxcomposite-dev \
    mesa-common-dev libgl1-mesa-dev libgtk-3-dev libwebkit2gtk-4.1-dev
```

```bash
export JUCE_ROOT=/percorso/di/JUCE
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build      # copia il VST3 in ~/.vst3 e le icone in ~/.local/share/icons
```

---

## Dove installare il VST3

| Piattaforma | Cartella |
|-------------|----------|
| Windows | `C:\Program Files\Common Files\VST3\` |
| macOS | `~/Library/Audio/Plug-Ins/VST3/` |
| Linux | `~/.vst3/` |

---

## Test unitari C++

Sono disattivati di default:

```bash
cmake -B build -DBUILD_UNIT_TESTS=ON
cmake --build build
```

Ci sono anche script Python di test in `tests/` e `test_tools/`: server OSC e WebSocket finti per provare il bridge senza aprire un DAW.

---

## Problemi noti

**"JUCE not found"** — `JUCE_ROOT` non impostata o percorso inesistente.

**"Could NOT find WebView2"** (Windows) — manca il pacchetto NuGet, vedi sopra.

**Il plugin si apre vuoto** — manca `webview-ui/dist`. Compila la UI e ricompila il plugin. In fase di sviluppo puoi anche lasciare `npm run dev` in esecuzione: il plugin cerca un dev server Vite su `localhost:5173` e `4173` e lo preferisce al bundle interno.

**Build sporca** — cancella la cartella `build*` e riconfigura da zero. Su Windows la prima configurazione richiede qualche minuto perché JUCE compila `juceaide`.

---

## Da fare

- [ ] GitHub Actions: build automatica su Windows e macOS a ogni push
- [ ] Release automatiche con changelog
- [ ] Firma del binario Windows
- [ ] Notarizzazione macOS
