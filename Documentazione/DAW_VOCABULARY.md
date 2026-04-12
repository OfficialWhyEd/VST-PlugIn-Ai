# DAW Protocol Vocabulary

**Creato:** 2026-04-12  
**Fonti:** Reaper OSC SDK, AbletonOSC, MIDI CC Standard  
**Scopo:** Riferimento per costruire protocollo JSON plugin modulare

---

## 📊 Panoramica Protocolli

| DAW | Protocollo | Caratteristiche |
|-----|------------|-----------------|
| **Reaper** | OSC nativo | Pattern file `.ReaperOSC` personalizzabili |
| **Ableton Live** | OSC via Max for Live | Richiede AbletonOSC (libreria Python) |
| **Standard** | MIDI CC | Universale, 7-bit (0-127) |
| **VST3** | Parametri plugin | Standardizzato, host → plugin |

---

## 🔵 REAPER OSC

### File Pattern Config
- **Posizione:** `~/.config/REAPER/Default.ReaperOSC` (Linux)
- **Formato:** Pattern personalizzabili, es: `TRACK_VOLUME` → `/track/@/volume`
- **Wildcard:** `@` = track number

### Comandi Comuni Reaper

| Categoria | Comando | Pattern OSC | Tipo Dato |
|-----------|---------|-------------|-----------|
| **Transport** | Play | `/play` | toggle |
| | Stop | `/stop` | trigger |
| | Record | `/record` | toggle |
| | Pause | `/pause` | trigger |
| | Metronome | `/metronome` | toggle |
| | Tempo | `/tempo` | float (BPM) |
| | Time Position | `/time` | float (sec) |
| | Beat Position | `/beat` | float |
| **Track** | Volume | `/track/@/volume` | float (0.0-1.0) |
| | Pan | `/track/@/pan` | float (-1.0 to 1.0) |
| | Mute | `/track/@/mute` | toggle |
| | Solo | `/track/@/solo` | toggle |
| | Arm | `/track/@/arm` | toggle |
| | Name | `/track/@/name` | string |
| | Peak Meters | `/track/@/vu` | float (L/R) |
| **FX** | Bypass | `/fx/@/bypass` | toggle |
| | Preset | `/fx/@/preset` | string/int |
| | Parameter | `/fx/@/param/@` | float (0.0-1.0) |

### Bidirezionale
Reaper può **inviare** e **ricevere** OSC. Il plugin deve:
1. Aprire porta UDP (es: 8000)
2. Inviare a Reaper (porta configurata, default 9000)
3. Ascoltare messaggi in entrata

---

## 🔴 Ableton Live OSC (via AbletonOSC)

**Requisito:** Max for Live + device AbletonOSC  
**Repo:** https://github.com/ideoforms/AbletonOSC

### Comandi Principali

| Categoria | Comando | Indirizzo OSC | Args |
|-----------|---------|---------------|------|
| **Transport** | Play | `/live/play` | - |
| | Stop | `/live/stop` | - |
| | Continue | `/live/continue` | - |
| | Record | `/live/record` | - |
| | Tap Tempo | `/live/tap_tempo` | - |
| | Tempo | `/live/set/tempo` | float |
| **Song** | Current Time | `/live/song/get/current_time` | → float |
| | Is Playing | `/live/song/get/is_playing` | → bool |
| | Panic (Stop All) | `/live/song/stop_all_clips` | - |
| **Track** | Volume Get | `/live/track/get/volume` | track_id → float |
| | Volume Set | `/live/track/set/volume` | track_id, float |
| | Pan Get | `/live/track/get/panning` | track_id → float |
| | Pan Set | `/live/track/set/panning` | track_id, float |
| | Mute | `/live/track/set/mute` | track_id, bool |
| | Solo | `/live/track/set/solo` | track_id, bool |
| | Name | `/live/track/get/name` | track_id → string |
| | Meter | `/live/track/start_listen/meter` | track_id |
| **Clip** | Fire | `/live/clip/fire` | track_id, clip_id |
| | Stop | `/live/clip/stop` | track_id, clip_id |
| | Get Color | `/live/clip/get/color` | track_id, clip_id → int |
| | Get Name | `/live/clip/get/name` | track_id, clip_id → string |
| | Get Playing | `/live/clip/get/is_playing` | track_id, clip_id → bool |
| **Device** | Parameter Get | `/live/device/get/parameter/value` | track_id, device_id, param_id → float |
| | Parameter Set | `/live/device/set/parameter/value` | track_id, device_id, param_id, float |

### Pattern AbletonOSC
- `/live/{object}/{action}/{property}`
- `{object}`: song, track, clip, device, etc.
- `{action}`: get, set, fire, stop, etc.
- Risposte: stesso indirizzo con dati aggiuntivi

---

## 🟡 MIDI CC Standard

### CCs Più Comuni (0-127, 7-bit)

| CC | Nome | Funzione | Range |
|----|------|----------|-------|
| 0 | Bank Select | Cambio banco | 0-127 |
| 1 | Modulation Wheel | Vibrato, modulazione | 0-127 |
| 2 | Breath Controller | Soffio, CC generico | 0-127 |
| 4 | Foot Controller | Pedale espressione | 0-127 |
| 7 | Volume | Volume canale | 0-127 |
| 8 | Balance | Bilanciamento stereo | 0-127 |
| 10 | Pan | Panning L/R | 0-127 |
| 11 | Expression | Pedale espressione | 0-127 |
| 64 | Sustain Pedal | On/Off sustain | 0-63=off, 64-127=on |
| 74 | Filter Cutoff | Cutoff filtro | 0-127 |
| 91 | Reverb Send | Send riverbero | 0-127 |
| 93 | Chorus Send | Send chorus | 0-127 |

### Pitch Bend (non CC)
- **Range:** 14-bit (-8192 to +8191)
- **Default:** 0 = no bend
- **Range tipico:** ±2 semitoni

### Channel Pressure (Aftertouch)
- **Range:** 0-127
- **Per canale:** un valore per tutte le note
- **Polyphonic:** un valore per ogni nota (rare)

---

## 🟢 Comandi Comuni Cross-DAW

Per il plugin modulare, questi comandi sono universali:

| Azione | Reaper | Ableton | MIDI CC |
|--------|--------|---------|---------|
| Play | `/play` | `/live/play` | System Real-Time |
| Stop | `/stop` | `/live/stop` | System Real-Time |
| Record | `/record` | `/live/record` | System Real-Time |
| Volume Track | `/track/1/volume` | `/live/track/set/volume` | CC 7 |
| Pan Track | `/track/1/pan` | `/live/track/set/panning` | CC 10 |
| Modulation | - | - | CC 1 |
| Expression | - | - | CC 11 |

---

## 📝 Implicazioni per Plugin Modulare

### Cosa il plugin può rilevare:
1. **Cambiamenti parametro** → Volume, Pan, plugin params
2. **Eventi transport** → Play, stop, record, tempo
3. **MIDI in tempo reale** → Pitch bend, mod wheel, CCs

### Cosa deve supportare il protocollo JSON:
- `event_type`: "transport", "parameter", "midi_cc", "pitch_bend"
- `source`: "reaper_osc", "ableton_osc", "midi_cc"
- `target`: track_id, clip_id, param_id
- `value`: float/int/string
- `suggested_widget`: "slider", "knob", "button", "xy_pad"

### Per la toolbox modulare:
Quando arriva un evento sconosciuto, il plugin deve:
1. Identificare tipo e range
2. Proporre widget appropriato
3. Se utente accetta → aggiungere a UI dinamica

---

## 🔗 Link Utili

- **Reaper OSC SDK:** https://www.reaper.fm/sdk/osc/osc.php
- **AbletonOSC:** https://github.com/ideoforms/AbletonOSC
- **MIDI CC List:** https://www.presetpatch.com/midi-cc-list.aspx
- **OSC Spec:** http://opensoundcontrol.org/spec-1_0.html

---

*File creato per riferimento sviluppo VST Plugin AI.*
