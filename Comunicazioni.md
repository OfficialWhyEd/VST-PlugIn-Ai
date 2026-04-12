# 📢 Comunicazioni — Da Leggere Prima di Iniziare

**Ultimo aggiornamento:** 2026-04-12 08:45  
**Mittente:** Aura (Project Manager)  
**Destinatari:** Heartbroken (React UI), Edo (Indicazioni/Prompt)

---

## 🚨 Messaggio Importante

Il **protocollo JSON v0.1** è definitivo e pronto per l'implementazione.  
La **documentazione completa** è nella cartella `Documentazione/`.

**Non serve aspettare approvazioni.** Procedere con le implementazioni assegnate.

---

## 👥 Ruoli Chiari

| Persona | Ruolo | Cosa Fa |
|---------|-------|---------|
| **Aura** | PM + Backend AI | Documentazione, protocolli, mock server, supporto tecnico |
| **Heartbroken** | Frontend React | UI toolbox modulare, widget, connessione WebView |
| **Edo** | Visione/Requirements | Indicazioni strategiche, test su Ableton, direzione progetto |

**Nota:** Edo fornisce la visione e i requisiti. Heartbroken e Aura eseguono lo sviluppo tecnico.

---

## 🎯 Stato Attuale (12/04/2026)

### ✅ Completato da Aura
- [x] Protocollo JSON v0.1 (`PROTOCOLLO_JSON_v0.1.md`)
- [x] Guida implementazione con codice (`IMPLEMENTATION_GUIDE.md`)
- [x] Manuali PDF (Reaper, OSC Spec)
- [x] Note tecniche con fonti verificate

### ⏳ Da Fare — Heartbroken
| Task | Stato | Note |
|------|-------|------|
| Setup React | ❌ | `npx create-vite@latest webview-ui` |
| Componente Toolbox | ❌ | Vedi `IMPLEMENTATION_GUIDE.md` sezione React |
| Widget Slider/Knob/Button | ❌ | Codice esempio già fornito |
| Connessione WebView | ❌ | Usare `window.receiveFromPlugin` mockato per test |

### ⏳ Da Fare — Edo
| Task | Stato | Note |
|------|-------|------|
| Test Ableton | ⏳ | Plugin VST3 Windows compilato |
| Validazione protocollo | ❌ | Verificare se `/live/*` comandi funzionano su Ableton |
| Feedback su documentazione | ❌ | Eventuali modifiche necessarie al protocollo |

---

## 📚 Documentazione da Consultare

**Per Heartbroken:**
1. `IMPLEMENTATION_GUIDE.md` → Sezione "Per Heartbroken (React)"
2. `PROTOCOLLO_JSON_v0.1.md` → Struttura messaggi JSON
3. Mock data forniti nella guida per testare senza C++

**Per Edo:**
1. `PROTOCOLLO_JSON_v0.1.md` → Verificare se mappabile su AbletonOSC
2. `NOTE_TECNICHE_PROTOCOLI.md` → Link ufficiali AbletonOSC

---

## ⏱️ Timeline

| Data | Obiettivo |
|------|-----------|
| **+24h** | Heartbroken: Setup React, mock funzionante |
| **+48h** | Heartbroken: Toolbox base con proposte widget |
| **+72h** | Heartbroken: Widget funzionanti (Slider, Knob, Button) |
| **+96h** | Heartbroken: PR verso `master` con UI completa |
| **+96h** | Edo: Feedback su test Ableton |

---

## 🔧 Supporto

**Problemi tecnici su React?** → Aura può fornire snippet, debug, mock server.  
**Problemi su protocollo OSC/Ableton?** → Edo verifica comandi `/live/*`.

**Non aspettare risposte prima di procedere.** Il protocollo è definito, il codice è indicato, i mock sono pronti.

---

## 📝 Come Aggiornare Questo File

Aggiungere messaggi in cima con timestamp:

```markdown
## [2026-04-13 10:00] Heartbroken — Setup completato
Setup React fatto, mock ricezione JSON funziona. Inizio componente Toolbox.

---
```

---

**Prossima comunicazione prevista:** Heartbroken conferma setup React (+24h).
