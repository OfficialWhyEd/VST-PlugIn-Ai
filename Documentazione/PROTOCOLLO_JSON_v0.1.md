# Protocollo JSON Plugin ↔ AI

**Versione:** 0.1 — Draft basato su OSC 1.0 Spec  
**Data:** 2026-04-12  
**Fonte:** OSC 1.0 Specification, DAW_VOCABULARY.md precedente

---

## 📐 Specifica Tecnica Base

### Trasporto
- **Protocollo:** UDP (recomandato) o TCP
- **Porta default:** Plugin riceve su 8000, invia a DAW su 9000
- **Formato:** JSON inserito in payload OSC o HTTP POST diretto

### Formato Messaggio JSON Base

```json
{
  "message_type": "event|command|query|response",
  "timestamp": 1715505600,
  "session_id": "uuid-generato",
  "payload": {}
}
```

---

## 📤 Messaggi Plugin → AI

### 1. Evento Rilevato dal DAW

```json
{
  "message_type": "event",
  "timestamp": 1715505600.123,
  "session_id": "plugin-session-001",
  "source": {
    "daw": "reaper|ableton|generic",
    "protocol": "osc|midi|internal"
  },
  "event": {
    "type": "parameter_change|transport|midi_cc|pitch_bend|note",
    "category": "volume|pan|plugin_param|transport|modulation|expression",
    "target": {
      "track_id": 1,
      "track_name": "Synth Lead",
      "param_name": "Volume",
      "param_path": "/track/1/volume"
    },
    "value": {
      "raw": 0.75,
      "normalized": 0.75,
      "display": "-12.5 dB",
      "unit": "dB"
    },
    "range": {
      "min": 0.0,
      "max": 1.0,
      "type": "float"
    }
  },
  "suggested_widget": {
    "type": "slider|knob|xy_pad|button|fader",
    "label": "Track 1 Volume",
    "auto_proposed": true
  }
}
```

### 2. Richiesta AI Action

```json
{
  "message_type": "command",
  "timestamp": 1715505600.456,
  "session_id": "plugin-session-001",
  "command": {
    "type": "ai_process",
    "request": "analyze_pattern",
    "context": {
      "track_id": 1,
      "clip_name": "Verse 1",
      "previous_values": [0.5, 0.6, 0.75],
      "pattern": "crescendo"
    },
    "ai_provider": "ollama|openai|anthropic",
    "model": "llama3|gemini|claude"
  }
}
```

### 3. Query Stato DAW

```json
{
  "message_type": "query",
  "timestamp": 1715505600.789,
  "session_id": "plugin-session-001",
  "query": {
    "type": "get_parameter",
    "target": {
      "track_id": 1,
      "param_name": "Volume"
    }
  }
}
```

---

## 📥 Messaggi AI → Plugin

### 1. Risposta con Widget Proposal

```json
{
  "message_type": "response",
  "timestamp": 1715505601.000,
  "session_id": "plugin-session-001",
  "response": {
    "type": "widget_proposal",
    "status": "success|error",
    "data": {
      "proposed_widgets": [
        {
          "widget_id": "widget-001",
          "type": "slider",
          "label": "Track 1 Volume",
          "description": "Volume del canale 1 con range -inf a +6dB",
          "parameters": {
            "min": 0.0,
            "max": 1.0,
            "default": 0.75,
            "step": 0.01,
            "orientation": "vertical|horizontal"
          },
          "mapping": {
            "osc_address": "/track/1/volume",
            "midi_cc": 7,
            "feedback": true
          },
          "style": {
            "color": "#4CAF50",
            "size": "medium",
            "position": "auto"
          }
        },
        {
          "widget_id": "widget-002",
          "type": "knob",
          "label": "Track 1 Pan",
          "parameters": {
            "min": -1.0,
            "max": 1.0,
            "default": 0.0,
            "step": 0.01
          },
          "mapping": {
            "osc_address": "/track/1/pan",
            "midi_cc": 10
          }
        }
      ],
      "confidence": 0.92,
      "reasoning": "Valori frequentemente modificati, range standard"
    }
  }
}
```

### 2. Azione da Eseguire sul DAW

```json
{
  "message_type": "command",
  "timestamp": 1715505602.000,
  "session_id": "plugin-session-001",
  "action": {
    "type": "set_parameter|fire_clip|trigger_transport",
    "target": {
      "track_id": 1,
      "param_name": "Volume"
    },
    "value": 0.8,
    "interpolation": {
      "type": "linear|ease_in|ease_out",
      "duration_ms": 100
    }
  }
}
```

### 3. Modifica Toolbox Utente

```json
{
  "message_type": "command",
  "timestamp": 1715505603.000,
  "session_id": "plugin-session-001",
  "toolbox_action": {
    "type": "add_widget|remove_widget|modify_widget",
    "widget_id": "widget-003",
    "widget_config": {
      "type": "xy_pad",
      "label": "XY Control",
      "x_param": "pan",
      "y_param": "volume",
      "track_id": 1
    },
    "user_prompt": "Aggiungere controllo XY per pan/volume?",
    "auto_add": false
  }
}
```

---

## 🧰 Toolbox Modulare — Schema

### Flusso Aggiunta Widget

```
1. Evento OSC rilevato → Plugin
2. Plugin invia evento → AI (con suggested_widget)
3. AI analizza → Risponde con widget proposal
4. Plugin mostra prompt utente: "Aggiungere [widget]?"
5. Utente accetta → Widget aggiunto a UI dinamica
6. Widget registrato → Collegato a evento OSC/MIDI
```

### Tipi Widget Supportati

| Tipo | Parametri | Dati Ingresso |
|------|-----------|--------------|
| `slider` | min, max, step, orientation | float |
| `knob` | min, max, step, rotation | float |
| `xy_pad` | x_param, y_param, x_range, y_range | float[2] |
| `button` | toggle/momentary, color | boolean |
| `fader` | min, max, step | float |
| `meter` | peak/rms, channel | float |
| `scope` | time_window, channels | buffer audio |

---

## 🔌 Mapping OSC ↔ JSON

### Da OSC a JSON Event

```
OSC: /track/1/volume 0.75
↓
JSON:
{
  "event": {
    "type": "parameter_change",
    "target": {
      "track_id": 1,
      "param_name": "volume"
    },
    "value": {
      "raw": 0.75,
      "normalized": 0.75
    }
  }
}
```

### Da JSON a OSC Command

```
JSON:
{
  "action": {
    "type": "set_parameter",
    "target": { "track_id": 1, "param_name": "volume" },
    "value": 0.8
  }
}
↓
OSC: /track/1/volume 0.8
```

---

## 🐛 Errori e Stati

```json
{
  "message_type": "response",
  "status": "error",
  "error": {
    "code": "OSC_CONNECTION_LOST|AI_TIMEOUT|INVALID_PARAMETER|DAW_NOT_RESPONDING",
    "message": "Connessione OSC con Reaper persa",
    "recoverable": true|false
  }
}
```

---

## 📋 Da Verificare / Completare

- [ ] Test con Reaper reale (pattern OSC specifici)
- [ ] Test con AbletonOSC (comandi /live/*)
- [ ] Implementazione WebSocket C++ ↔ JavaScript
- [ ] Serializzazione JSON in JUCE
- [ ] Gestione errori e retry

---

## 🔗 Riferimenti

- OSC 1.0 Spec: Documentazione/Manuali/OSC_1.0_Specification.pdf
- Reaper OSC SDK: https://www.reaper.fm/sdk/osc/osc.php
- AbletonOSC: https://github.com/ideoforms/AbletonOSC

---

*Draft creato basato su OSC 1.0 Specification e analisi DAW. Da validare con test reali.*
