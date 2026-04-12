# 📢 Comunicazioni — Da Leggere Prima di Iniziare

**Ultimo aggiornamento:** 2026-04-12 12:24  
**Mittente:** Aura (PM + Backend)  
**Destinatari:** Heartbroken (Frontend React), Edo (Visione/Testing)

---

## 🚨 Aggiornamento Importante — Sessione 12/04

### ✅ Completato Oggi
- **Build VST3 Linux:** Plugin funzionante in `~/.vst3/`
- **Protocollo JSON v1.0:** Definitivo e testato (11/11 test passati)
- **Test Reaper:** Plugin si carica correttamente
- **Architettura finale:** Decisa "Ponte OSC-Web"

### 🔴 Cambiamento Architetturale — WebView Scartata

**Problema scoperto:** WebView (WebKit/GTK) **crasha** nel plugin VST su Linux.

**Evidenzia:**
- Forum JUCE conferma: "WebBrowserComponent very poor linux experience"
- Test reale: Reaper crasha all'apertura del plugin con WebView

**Nuova architettura adottata:** **Ponte OSC-Web**

```
BROWSER (React) ←OSC→ PLUGIN VST (JUCE)
localhost:3000   :9000  Audio + OSC Server
```

**Perché questa soluzione:**
- ✅ Stabile (UI fuori processo, niente crash)
- ✅ Professionale (usata da WebUISynth, juce-webview-tutorial)
- ✅ Heartbroken ha libertà totale (React, Vue, qualsiasi framework)
- ✅ Latenza 1-10ms accettabile per controllo

**Documento completo:** `Documentazione/architettura-ponte.md`

---

## 👥 Ruoli Confermati

| Persona | Ruolo | Cosa Fa |
|---------|-------|---------|
| **Aura** | PM + Backend C++ | Protocolli, OSC, build, documentazione |
| **Heartbroken** | Frontend React | UI web in browser, comunicazione OSC |
| **Edo** | Testing/Requirements | Test Windows, Ableton, direzione strategica |

---

## 🎯 Stato Attuale — 12/04/6

### ✅ Completato (Aura)
- [x] Build VST3 Linux funzionante
- [x] Protocollo JSON v1.0 definito e testato
- [x] WebViewBridge C++ implementato (da usare con ponte)
- [x] JavaScript Bridge per React creato
- [x] Architettura Ponte decisa e documentata
- [x] UI nativa JUCE fallback (stabile)

### ⏳ Da Fare — Heartbroken
| Task | Stato | Note |
|------|-------|------|
| Setup React + server dev | ⏳ | `npm create vite@latest webview-ui -- --template react` |
| UI web base | ⏳ | Componenti React per controllo plugin |
| Client OSC in React | ⏳ | Comunicazione con plugin via OSC (porta 9000) |
| Widget dinamici | ⏳ | Slider, knob, button (come da protocollo) |

**Importante:** La UI non è più _dentro_ il plugin, ma in **browser separato** che parla con il plugin via OSC.

**File guida:**
- `Documentazione/protocol-json-v1.md` — Struttura messaggi
- `webview-ui/src/openclaw-bridge.js` — Bridge JavaScript (da usare)
- `Tools/test_protocol_json.py` — Esempi di messaggi JSON

### ⏳ Da Fare — Edo
| Task | Stato | Note |
|------|-------|------|
| Test plugin Windows | ✅ | Build completata da Edo |
| Test Ableton | ⏳ | Verificare caricamento VST3 |
| Validazione OSC Ableton | ⏳ | Verificare se `/live/*` comandi funzionano |

---

## 📚 Documentazione Aggiornata

**Per Heartbroken (React):**
1. `Documentazione/architettura-ponte.md` — Spiegazione architettura
2. `Documentazione/protocol-json-v1.md` — Protocollo messaggi JSON
3. `webview-ui/src/openclaw-bridge.js` — Bridge JavaScript da integrare
4. `Tools/test_protocol_json.py` — Esempi di messaggi

**Per Edo (Testing):**
1. `STATUS.md` — Stato completo progetto
2. `Documentazione/protocol-json-v1.md` — Protocollo da validare su Ableton

---

## ⏱️ Timeline Aggiornata

| Data | Obiettivo | Chi |
|------|-----------|-----|
| **+24h** | Setup React + server dev | Heartbroken |
| **+48h** | UI web base connessa via OSC | Heartbroken |
| **+72h** | Widget dinamici funzionanti | Heartbroken |
| **+96h** | Test Ableton feedback | Edo |
| **+120h** | Integrazione completa end-to-end | Tutti |

---

## 🔧 Supporto Tecnico

**Heartbroken — Problemi con React o OSC?**
→ Aura fornisce supporto, snippet, debug.

**Edo — Problemi con Ableton OSC?**
→ Verificare mappatura comandi `/live/*` vs Reaper.

---

## 📝 Istruzioni per Aggiornare Questo File

Aggiungere messaggi in cima con timestamp:

```markdown
## [2026-04-13 10:00] Heartbroken — Setup completato
Setup React e server dev completati. Inizio integrazione OSC.

---
```

**Mantenere la struttura:**
- Header sempre in cima
- Messaggi importanti in evidenza
- Stato attuale con checkbox
- Timeline aggiornata
- Documentazione linkata

---

**Prossima azione richiesta:**
- **Heartbroken:** Iniziare setup React (entro +24h)
- **Edo:** Test Ableton quando pronto (build Windows OK)

**Contatti:** Repo GitHub per discussioni tecniche.
