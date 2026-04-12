# Architettura Ponte - OpenClaw VST Bridge AI

**Data:** 2026-04-12  
**Stato:** Proposta architettura

---

## Problema Identificato

WebView (WebKit/GTK) nel plugin VST **crasha** su Linux.

**Evidenze:**
- Forum JUCE: "WebBrowserComponent very poor linux experience"
- GitHub JUCE: Issue #1557 GUI crashes on Ubuntu 24.04
- Test reale: Reaper crasha all'apertura del plugin con WebView

**Soluzione temporanea:** UI nativa JUCE (fallback) - funzionante.

---

## Architettura Proposta: "Ponte OSC-Web"

Inspirata da progetti reali (tomduncalf/WebUISynth, JanWilczek/juce-webview-tutorial).

```
┌─────────────────┐     OSC/WebSocket      ┌─────────────────┐
│   BROWSER       │  ◄───────────────────►  │  PLUGIN VST     │
│  (React UI)     │    localhost:9000       │  (JUCE nativo)  │
│  Heartbroken    │                         │  Audio Engine   │
│  localhost:3000 │                         │  OSC Server     │
└─────────────────┘                         └─────────────────┘
         │                                           │
         │         Comunicazione:                    │
         │         - Transport DAW                   │
         │         - Track info                      │
         │         - Widget values                   │
         │         - AI responses                    │
         └───────────────────────────────────────────┘
                            AI/Ollama (opzionale)
```

---

## Vantaggi

| Aspetto | WebView integrata | Ponte OSC-Web |
|---------|-------------------|---------------|
| Stabilità | ❌ Crash su Linux | ✅ Fuori processo |
| Libertà UI | Limitata (WebKit) | Totale (any browser) |
| Debug | Difficile | Facile (DevTools) |
| Latenza | Bassa | 1-10ms (accettabile per controllo) |
| Professionale | Sì | Sì (usato da progetti reali) |

---

## Implementazione

### Componenti

1. **Plugin VST** (JUCE)
   - Audio engine
   - OSC server (riceve comandi da UI)
   - OSC client (invia dati DAW a UI)
   - UI nativa fallback (emergenza)

2. **UI Web** (React)
   - Server dev localhost:3000
   - Client OSC (comunica con plugin)
   - Widget dinamici
   - Chat AI integrata

### Protocollo

**Plugin → UI (via OSC):**
```json
{"type": "daw.transport", "payload": {"isPlaying": true, "bpm": 120}}
{"type": "daw.track", "payload": {"trackId": 1, "volumeDb": -3.0}}
{"type": "ai.response", "payload": {"content": "Suggerimento..."}}
```

**UI → Plugin (via OSC):**
```json
{"type": "daw.command", "payload": {"command": "play"}}
{"type": "widget.valueChange", "payload": {"widgetId": "eq-high", "value": 3.5}}
{"type": "ai.prompt", "payload": {"prompt": "Analizza traccia 1"}}
```

---

## Task per Implementazione

| # | Task | Chi | Priorità |
|---|------|-----|----------|
| 1 | Implementare OSC bidirezionale completo | Aura | Alta |
| 2 | Creare server web per UI React | Heartbroken | Alta |
| 3 | Integrare client OSC in React | Heartbroken | Media |
| 4 | Test integrazione end-to-end | Carlo | Media |
| 5 | Documentare setup sviluppo | Aura | Bassa |

---

## Riferimenti

- **tomduncalf/WebUISynth** (45⭐): github.com/tomduncalf/WebUISynth
- **JanWilczek/juce-webview-tutorial** (50⭐): github.com/JanWilczek/juce-webview-tutorial
- **JUCE OSC Tutorial**: docs.juce.com/master/tutorial_osc_sender_receiver.html
- **Forum JUCE WebView**: forum.juce.com/t/webbrowsercomponent-very-poor-linux-experience/66681

---

## Decisione

✅ **Adottata architettura Ponte OSC-Web**

**Motivo:** Professionale, stabile, usata in progetti reali. Heartbroken ha libertà totale sulla UI.

---

*Documento creato per tracciare decisione architetturale.*
