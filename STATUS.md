# OpenClaw VST Bridge AI - Stato Progetto

**Ultimo aggiornamento:** 2026-04-12 11:20

---

## ✅ Completato Oggi (Aura)

| Componente | Stato | Note |
|------------|-------|------|
| Protocollo JSON v1.0 | ✅ | Documento completo in `Documentazione/protocol-json-v1.md` |
| WebViewBridge C++ | ✅ | Implementazione completa con shortcuts |
| PluginEditor WebView | ✅ | Integrazione WebView + fallback UI |
| JavaScript Bridge | ✅ | `openclaw-bridge.js` per React |

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

## ✅ Completato (Linux)

| Componente | Stato | Note |
|------------|-------|------|
| Build VST3 | ✅ | Compilato con successo |
| Build Standalone | ✅ | Compilato con successo |
| OscHandler | ✅ | Bidirezionale (send/receive) |
| AiEngine | ⚠️ | Struttura base HTTP, POST cURL non implementato |
| WebViewBridge | ✅ | Header + implementation completi v1.0 |
| PluginEditor WebView | ✅ | Integrazione completa con fallback |
| CMakeLists.txt | ✅ | Configurato (cURL, JUCE, Linux) |
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

**Problema:** La TODO list promessa da Edo non è stata pushata. Heartbroken e Edo lavorano senza task assegnati.

**Soluzione temporanea:** Vedi `PENDING_TODO.md` con task aggiornati.

---

## 🔴 Bloccante: Task Team Ridefiniti

| Ruolo | Task | Problema | Azione |
|-------|------|----------|--------|
| **Edo (C++)** | Rilevare eventi DAW → inviare a JS | Non so se sa come fare | Documentare esempio OSC |
| **Heartbroken (React)** | UI dinamica con widget | Non sa dell'idea "toolbox" | Allineare su design |

**Nota:** Toolbox modulare = plugin rileva eventi DAW e propone widget dinamici. Es: "Rilevato pitch su ch1. Aggiungere slider?"

---

## ❌ Mancante

1. **Build Windows** - Edo sta compilando
2. **Test VST3 in DAW** - Carlo (Reaper) e Edo (Ableton)
3. **✅ Completato: Comunicazione WebView** - Protocollo C++↔JS v1.0
4. **UI React** - Da creare (Heartbroken) - usa `openclaw-bridge.js`
5. **AiEngine POST** - Implementare cURL reale per Ollama

---

## 📁 File Aggiunti/Modificati Oggi

```
Documentazione/protocol-json-v1.md      # Nuovo: protocollo completo
src/ui/WebViewBridge.h                 # Modificato: v1.0 API
src/ui/WebViewBridge.cpp                 # Modificato: implementazione completa
src/core/PluginEditor.h                  # Modificato: WebView integration
src/core/PluginEditor.cpp                # Modificato: WebView + fallback
webview-ui/src/openclaw-bridge.js      # Nuovo: bridge JavaScript React
```

---

## Prossimi Passi

| # | Task | Chi | Priorità |
|---|------|-----|----------|
| 1 | Testare build C++ con nuovo bridge | Carlo | Alta |
| 2 | Testare VST3 in Reaper | Carlo | Alta |
| 3 | Creare UI React base (usando bridge) | Heartbroken | Alta |
| 4 | Implementare POST cURL in AiEngine | Aura | Media |
| 5 | Testare OSC bidirezionale in DAW | Carlo/Edo | Media |

---

## Commits Recenti

- `be777b8` - AURA: Fix include JUCE per Windows
- `4b8ff9a` - AURA: Rimuovi evaluateJavaScript privato, usa goToURL
- `6321e7e` - AURA: WebViewBridge completo con nlohmann-json
- `ced6e52` - AURA: Fix include path per src/ui/

---

## Note Importanti

- **Percorso JUCE Edo:** `C:\Users\Whyed\Desktop\CARTELLE\JUCE`
- **Percorso vcpkg Edo:** `C:\Users\Whyed\vcpkg`
- **Repo GitHub:** https://github.com/OfficialWhyEd/VST-PlugIn-Ai
- **Branch attivo:** master

---

*File aggiornato per tracciare stato tra sessioni.*