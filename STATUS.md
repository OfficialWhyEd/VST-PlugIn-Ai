# OpenClaw VST Bridge AI - Stato Progetto

**Ultimo aggiornamento:** 2026-04-12 12:21

---

## ✅ Completato Oggi (Sessione 12/04)

| Componente | Stato | Note |
|------------|-------|------|
| **Build Linux VST3** | ✅ | Plugin compilato e installato |
| **Test Reaper** | ✅ | Plugin funziona con fallback UI |
| **Debug WebView** | ✅ | Confermato: WebKit/GTK crasha in VST |
| **Architettura Ponte** | ✅ | Decisa: OSC ↔ UI web esterna |
| Protocollo JSON v1.0 | ✅ | Documentato e testato |
| WebViewBridge C++ | ✅ | Implementato (da attivare con ponte) |
| PluginEditor | ✅ | Fallback UI funzionante |
| Test Protocollo JSON | ✅ | Python test suite 11/11 passati |

---

## 🎉 BUILD COMPLETATA

**File:** `~/.vst3/OpenClawVSTBridgeAI.vst3` (~30MB)

**Installato:** ✅ Copiato in `~/.vst3/`

**Test Reaper:** ✅ Funziona con UI nativa JUCE

---

## 🔴 Problema WebView Risolto

**Problema:** WebView (WebKit/GTK) crasha nel plugin VST su Linux

**Evidenzia:**
- Forum JUCE: "WebBrowserComponent very poor linux experience"
- Test reale: Reaper crasha all'apertura

**Soluzione Adottata:** Architettura "Ponte OSC-Web"

---

## 🏗️ Architettura Ponte OSC-Web

**Documento:** `Documentazione/architettura-ponte.md`

```
BROWSER (React) ←──OSC──→ PLUGIN VST (JUCE)
localhost:3000     :9000    Audio + OSC Server
   │                           │
   └──── AI/Ollama (opt) ─────┘
```

**Vantaggi:**
- ✅ Stabile (UI fuori processo)
- ✅ Libertà totale per Heartbroken (React, Vue, etc.)
- ✅ Professionale (usato da WebUISynth, juce-webview-tutorial)
- ✅ Latenza 1-10ms accettabile per controllo

**Componenti:**
1. **Plugin VST:** JUCE audio + OSC server + fallback UI
2. **UI Web:** React in browser che parla OSC con plugin

---

## 📋 Prossimi Task

| # | Task | Chi | Priorità |
|---|------|-----|----------|
| 1 | Implementare OSC bidirezionale completo | Aura | Alta |
| 2 | Creare server web per UI React | Heartbroken | Alta |
| 3 | Integrare client OSC in React | Heartbroken | Media |
| 4 | Test integrazione end-to-end | Carlo | Media |
| 5 | Documentare setup sviluppo | Aura | Bassa |

---

## 📦 File Aggiunti Oggi

```
Documentazione/protocol-json-v1.md         # Protocollo completo
Documentazione/architettura-ponte.md       # Architettura decisione
Tools/test_protocol_json.py               # Test suite Python
third_party/nlohmann/json.hpp             # Libreria JSON
webview-ui/src/openclaw-bridge.js         # Bridge JS per React
src/core/PluginEditor.cpp                  # Modificato: fallback UI
src/core/PluginEditor.h                    # Modificato: WebView
src/core/PluginProcessor.cpp               # Modificato: includes
src/core/PluginProcessor.h                 # Modificato: forward decl
src/ui/WebViewBridge.cpp                   # Implementazione completa
src/ui/WebViewBridge.h                     # API v1.0
```

---

## 🎯 Piano di Lavoro

### Fase 1: Foundation (Completata ✅)
- ✅ Build VST3 funzionante
- ✅ Protocollo JSON definito
- ✅ UI fallback stabile

### Fase 2: Ponte OSC (Prossima)
- ⏳ Implementare OSC server completo nel plugin
- ⏳ UI web base che comunica via OSC
- ⏳ Test integrazione

### Fase 3: AI Integration
- ⏳ Implementare POST cURL in AiEngine
- ⏳ Connettere AI al flusso OSC

### Fase 4: Polish
- ⏳ UI React completa (Heartbroken)
- ⏳ Test multi-DAW (Reaper, Ableton)
- ⏳ Release

---

## Commits Sessione

- `fcf4a4a` - AURA: Forza fallback UI - WebView GTK crasha in VST
- `5199ce5` - AURA: BUILD COMPLETATA! Plugin VST3 creato
- `482cb93` - AURA: Update STATUS con test protocollo JSON
- `3100ba1` - AURA: Add test suite protocollo JSON v1.0
- `036cb16` - AURA: Fix nome prodotto senza spazi
- `ce7b93b` - AURA: Fix ordine linking JUCE
- `45d00c4` - AURA: Protocollo JSON C++↔JavaScript v1.0

---

## Note per Prossima Sessione

**Ripartire da:**
1. Implementazione OSC bidirezionale nel plugin
2. Creare server web base per UI React
3. Testare comunicazione plugin ↔ browser

**Plugin funzionante in:** `~/.vst3/OpenClawVSTBridgeAI.vst3`

---

*Sessione completata con successo. Build funzionante, architettura definita.*
