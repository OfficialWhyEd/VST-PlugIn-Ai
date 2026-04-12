# OpenClaw VST Bridge AI - Stato Progetto

**Ultimo aggiornamento:** 2026-04-12 11:31

---

## ✅ Completato Oggi (Aura)

| Componente | Stato | Note |
|------------|-------|------|
| Protocollo JSON v1.0 | ✅ | Documento completo in `Documentazione/protocol-json-v1.md` |
| WebViewBridge C++ | ✅ | Implementazione completa con shortcuts |
| PluginEditor WebView | ✅ | Integrazione WebView + fallback UI |
| JavaScript Bridge | ✅ | `openclaw-bridge.js` per React |
| **Test Protocollo JSON** | ✅ | Python test suite - 11/11 passati |
| Fix CMakeLists.txt | ✅ | Ordine linking corretto (gui_basics prima di gui_extra) |

---

## 🧪 Test Protocollo JSON v1.0

**File:** `Tools/test_protocol_json.py`

**Comando:** `python3 Tools/test_protocol_json.py`

**Risultati:** ✅ 11/11 test passati

Messaggi testati:
- ✅ DAW Transport (play/stop)
- ✅ DAW Track
- ✅ DAW Meter
- ✅ OSC Message
- ✅ AI Response
- ✅ Widget Create
- ✅ Plugin Init
- ✅ DAW Command (play, setVolume)
- ✅ AI Prompt

Error handling verificato:
- ❌ Messaggio senza tipo (rilevato)
- ❌ Tipo non valido (rilevato)
- ❌ Payload mancante (rilevato)
- ❌ daw.track senza trackId (rilevato)

---

## 📋 Protocollo JSON Definito

**File:** `Documentazione/protocol-json-v1.md`

### Messaggi C++ → JS
- `daw.transport` - Stato trasporto DAW
- `daw.track` - Info traccia
- `daw.meter` - Metering audio
- `daw.clip` - Info clip
- `osc.message` - Messaggi OSC
- `ai.response` / `ai.stream` - Risposte AI
- `ui.widget.create/update/remove` - Widget dinamici
- `plugin.error` - Errori

### Messaggi JS → C++
- `plugin.init` - Inizializzazione UI
- `daw.command` - Comandi DAW
- `daw.request` - Richieste info
- `ai.prompt` - Prompt AI
- `widget.valueChange` - Cambio widget
- `osc.send` - Invio OSC
- `config.get/set` - Configurazione

### Meccanismo
- C++ → JS: `goToURL("javascript:...")`
- JS → C++: `pageAboutToLoad("app://message/...")`

---

## ⚠️ Build C++ - Note

**Problema:** Build JUCE richiede ~4GB RAM

**Soluzione:** 
- Rimosso Standalone (solo VST3)
- Ordine linking corretto: `juce_gui_basics` prima di `juce_gui_extra`

**Per buildare:**
```bash
cd /home/carlo/progetti/OpenClaw-VST-Bridge
rm -rf build
cmake -B build -DJUCE_ROOT=/home/carlo/SDKs/JUCE
cmake --build build --config Release -j1  # usa solo 1 core
```

**Richiesto:** Chiudere applicazioni pesanti prima di buildare

---

## ✅ Completato (Linux)

| Componente | Stato | Note |
|------------|-------|------|
| Build VST3 | ⏳ | CMakeLists fixato, build richiede più memoria |
| OscHandler | ✅ | Bidirezionale (send/receive) |
| AiEngine | ⚠️ | Struttura base HTTP, POST cURL non implementato |
| WebViewBridge | ✅ | Header + implementation completi v1.0 |
| PluginEditor WebView | ✅ | Integrazione completa con fallback |
| CMakeLists.txt | ✅ | Fixato ordine moduli JUCE |
| Git | ✅ | Pushato su master |

---

## 🔄 In Corso (Windows - Edo)

**Build completata:** ✅
- Repo clonato, JUCE e vcpkg configurati
- Commit `be777b8`: Fix include JUCE

**Task attuali:**
| Task | Stato | Note |
|------|-------|------|
| Build Windows | ✅ Completata | VST3 e Standalone compilati |
| Test Ableton | ⏳ Da fare | Verificare caricamento plugin |
| Rilevare eventi DAW | ❓ Bloccato | Edo deve confermare se sa come fare |

---

## 🔴 Bloccante: Task Team Ridefiniti

| Ruolo | Task | Problema | Azione |
|-------|------|----------|--------|
| **Edo (C++)** | Rilevare eventi DAW → inviare a JS | Non so se sa come fare | Documentare esempio OSC |
| **Heartbroken (React)** | UI dinamica con widget | Non sa dell'idea "toolbox" | Allineare su design |

**Nota:** Toolbox modulare = plugin rileva eventi DAW e propone widget dinamici. Es: "Rilevato pitch su ch1. Aggiungere slider?"

---

## ❌ Mancante

1. **Build Linux VST3** - Richiede più memoria (~4GB)
2. **Test VST3 in Reaper** - Carlo (quando build funziona)
3. **Test Ableton Windows** - Edo
4. **✅ Completato: Comunicazione WebView** - Protocollo C++↔JS v1.0 testato
5. **UI React** - Da creare (Heartbroken) - usa `openclaw-bridge.js`
6. **AiEngine POST** - Implementare cURL reale per Ollama

---

## 📁 File Aggiunti/Modificati Oggi

```
Documentazione/protocol-json-v1.md      # Nuovo: protocollo completo
Tools/test_protocol_json.py             # Nuovo: test suite Python
src/ui/WebViewBridge.h                  # Modificato: v1.0 API
src/ui/WebViewBridge.cpp                # Modificato: implementazione completa
src/core/PluginEditor.h                 # Modificato: WebView integration
src/core/PluginEditor.cpp               # Modificato: WebView + fallback
src/core/PluginProcessor.h              # Modificato: forward declarations
src/core/PluginProcessor.cpp            # Modificato: include fix
cmake/CMakeLists.txt                    # Modificato: fix ordine linking
third_party/nlohmann/json.hpp           # Nuovo: libreria JSON
webview-ui/src/openclaw-bridge.js       # Nuovo: bridge JavaScript React
```

---

## 🎯 Prossimi Passi

| # | Task | Chi | Priorità |
|---|------|-----|----------|
| 1 | Build VST3 Linux (con più RAM) | Carlo | Alta |
| 2 | Testare VST3 in Reaper | Carlo | Alta |
| 3 | Creare UI React base (usando bridge) | Heartbroken | Alta |
| 4 | Test Ableton Windows | Edo | Alta |
| 5 | Implementare POST cURL in AiEngine | Aura | Media |
| 6 | Testare OSC bidirezionale in DAW | Carlo/Edo | Media |

---

## Commits Recenti

- `3100ba1` - AURA: Add test suite protocollo JSON v1.0
- `ce7b93b` - AURA: Fix ordine linking JUCE, build solo VST3
- `45d00c4` - AURA: Protocollo JSON C++↔JavaScript v1.0 completo
- `be777b8` - AURA: Fix include JUCE per Windows
- `4b8ff9a` - AURA: Rimuovi evaluateJavaScript privato, usa goToURL

---

## Note Importanti

- **Percorso JUCE Edo:** `C:\Users\Whyed\Desktop\CARTELLE\JUCE`
- **Percorso vcpkg Edo:** `C:\Users\Whyed\vcpkg`
- **Repo GitHub:** https://github.com/OfficialWhyEd/VST-PlugIn-Ai
- **Branch attivo:** master

---

*File aggiornato per tracciare stato tra sessioni.*
