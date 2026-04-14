# Per Edo & Heartbroken - Guida Sviluppo UI VST Plugin

**Versione:** 1.0  
**Data:** 14 Aprile 2026  
**Scopo:** Come sviluppare l'interfaccia web e collegarla ad Ableton/Reaper in tempo reale

---

## Cosa Ti Serve

| Tool | Perché |
|------|--------|
| VS Code / Cursor / Windsurf | Modificare i file React |
| Node.js (installato) | Per avviare il server di sviluppo |
| Ableton + Max for Live (consigliato) OPPURE Reaper | Per vedere i controlli muoversi |
| Browser (Chrome/Edge) | Per vedere l'interfaccia |

---

## Flusso di Lavoro Completo (Windows)

### FASE 1: Avvia il Plugin (Terminale 1)

Questo è il tuo "ponte" tra Ableton e la Web UI.

```powershell
cd C:\percorso\del\progetto\OpenClaw-VST-Bridge\build
.\OpenClawVSTPlugin_artefacts\Release\OpenClawVSTBridgeAI.exe
```

**Cosa fa:**
- Avvia WebSocket Server su porta `8080`
- Ascolta OSC da Ableton/Reaper su porta `9000`
- Invia comandi ad Ableton/Reaper su porta `9001`

**Lascia aperto questo terminale!**

---

### FASE 2: Avvia la Web UI (Terminale 2)

```powershell
cd C:\percorso\del\progetto\OpenClaw-VST-Bridge\webview-ui
npm run dev
```

**Si apre automaticamente il browser** su `http://localhost:3000`

**Hot Reload:** Quando salvi un file, il browser si aggiorna da solo.

---

### FASE 3: Configura Ableton (Una Volta Sol)

#### Opzione A: Ableton + Max for Live (Consigliato)

1. In Ableton, carica il plugin VST **OpenClawVSTBridgeAI**
2. Crea una traccia MIDI
3. Aggiungi un device Max for Live
4. Incolla questo codice Max nel device:

```max
----------begin_max5_patcher----------
1008.3ocuXs0aaaCG+Z4+J7+.... (codice OSC bridge)
----------end_max5_patcher----------
```

*Nota:* Aura fornirà il device Max completo se necessario.

#### Opzione B: Reaper (Più Semplice)

1. Apri Reaper
2. Carica il plugin VST **OpenClawVSTBridgeAI** su una traccia
3. Vai su **Options → Preferences → Control/OSC/Web**
4. Clicca **Add**
5. Imposta:
   - Mode: OSC
   - Device name: OpenClaw
   - Local listen port: `9001`
   - Default send port: `9000`
   - Default send IP: `127.0.0.1`

---

## Quali File Modificare

### File Principale: `webview-ui/src/App.jsx`

**Percorso:** `OpenClaw-VST-Bridge/webview-ui/src/App.jsx`

Questo è il cuore dell'interfaccia. Qui:
- Aggiungi pulsanti
- Cambi colori/layout
- Aggiungi animazioni
- Configuri i comandi DAW

**Struttura del file:**
```javascript
// 1. IMPORT
import { motion, AnimatePresence } from 'framer-motion'
import { openclaw, ConnectionState } from './openclaw-bridge'
import { BotFace } from './components/BotFace'

// 2. STATI (variabili che cambiano)
const [connectionStatus, setConnectionStatus] = useState(ConnectionState.DISCONNECTED)
const [botState, setBotState] = useState('idle')

// 3. EFFETTI (connessione, listener)
useEffect(() => {
  // Qui si connette al plugin
  openclaw.connect()
  
  // Qui ascolta i messaggi da Ableton
  openclaw.on('daw.transport', (payload) => {
    console.log('DAW dice:', payload)
  })
}, [])

// 4. RENDER (quello che vedi a schermo)
return (
  <div className="bg-background...">
    {/* QUI METTI I TUOI COMPONENTI */}
  </div>
)
```

---

## Come Aggiungere un Nuovo Pulsante che Comanda Ableton

### Esempio: Pulsante "Rewind"

**Passo 1:** Trova la sezione `sideModules` in `App.jsx`

```javascript
const sideModules = [
  { id: 'ai', icon: 'memory', label: 'AI ENGINE' },
  { id: 'transport', icon: 'play_arrow', label: 'TRANSPORT' },
  { id: 'comp', icon: 'settings_input_component', label: 'COMP' },
  // AGGIUNGI QUI:
  { id: 'rewind', icon: 'skip_previous', label: 'REWIND' },
]
```

**Passo 2:** Trova la funzione `handleTransport` e aggiungi:

```javascript
const handleTransport = useCallback((action) => {
  if (openclaw.isConnected()) {
    
    // Mappa azioni generiche a comandi specifici
    const commandMap = {
      'play': { type: 'daw.command', payload: { action: 'play' } },
      'stop': { type: 'daw.command', payload: { action: 'stop' } },
      'record': { type: 'daw.command', payload: { action: 'record' } },
      // AGGIUNGI QUI:
      'rewind': { type: 'daw.command', payload: { action: 'rewind', value: -10 } },
    }
    
    openclaw.send(commandMap[action] || { type: 'daw.command', payload: { action } })
  }
}, [])
```

**Passo 3:** Salva → Vedi il pulsante in browser → Clicca → Ableton esegue!

---

## Come Ricevere Valori da Ableton (Slider che si Muovono)

### Esempio: Mostrare Volume di Ableton nella UI

**Nel file `App.jsx`, aggiungi questo listener:**

```javascript
useEffect(() => {
  // ... altri listener ...
  
  // NUOVO: Ascolta parametri da Ableton
  const unsubParam = openclaw.on('daw.parameter', (payload) => {
    // payload = { address: '/track/1/volume', value: 0.75, name: 'Volume' }
    
    if (payload.address === '/track/1/volume') {
      setTrackVolume(payload.value) // Aggiorna lo slider
    }
  })
  
  return () => {
    // ... altri unsubscribe ...
    unsubParam()
  }
}, [])
```

**Aggiungi lo slider nel render:**

```javascript
// Aggiungi nello stato in alto:
const [trackVolume, setTrackVolume] = useState(0.5) // 0.0 - 1.0

// Aggiungi nel JSX (nella sezione Mastering Rack):
<div className="flex flex-col gap-1">
  <span className="text-[8px] text-[#FFB000] uppercase">Track Volume</span>
  <div className="h-2 bg-[#222222] w-full">
    <motion.div 
      className="h-full bg-[#DC143C]"
      style={{ width: `${trackVolume * 100}%` }}
      animate={{ width: `${trackVolume * 100}%` }}
    />
  </div>
  <span className="text-[9px] text-[#4d4d4d]">{Math.round(trackVolume * 100)}%</span>
</div>
```

**In Ableton:** Muovi il volume di una traccia → Vedi la barra muoversi in tempo reale nella Web UI!

---

## Comandi OSC Supportati (Ableton/Reaper)

### Comandi Generici (Funzionano con Entrambi)

| Azione | Comando | Descrizione |
|--------|---------|-------------|
| Play | `play` | Avvia riproduzione |
| Stop | `stop` | Ferma riproduzione |
| Record | `record` | Attiva/disattiva registrazione |
| Rewind | `rewind` | Torna indietro |
| BPM | `bpm` | Cambia BPM |

### Comandi Specifici Ableton (via Max for Live)

| Indirizzo OSC | Valore | Effetto |
|---------------|--------|---------|
| `/live/play` | 1 | Play |
| `/live/stop` | 1 | Stop |
| `/live/undo` | 1 | Undo |
| `/live/redo` | 1 | Redo |
| `/live/track/mute` | 0/1, track_id | Muta traccia |
| `/live/track/solo` | 0/1, track_id | Solo traccia |
| `/live/track/volume` | 0.0-1.0, track_id | Volume traccia |
| `/live/track/pan` | -1.0-1.0, track_id | Pan traccia |
| `/live/tempo` | 20-999 | Cambia BPM |

### Comandi Specifici Reaper

| Indirizzo OSC | Valore | Effetto |
|---------------|--------|---------|
| `/action/40044` | 1 | Play |
| `/action/40047` | 1 | Stop |
| `/action/40048` | 1 | Record |
| `/action/40322` | 1 | Undo |
| `/track/1/mute` | 0/1 | Muta traccia 1 |
| `/track/1/solo` | 0/1 | Solo traccia 1 |
| `/track/1/volume` | 0.0-1.0 | Volume traccia 1 |
| `/track/1/pan` | -1.0-1.0 | Pan traccia 1 |
| `/tempo` | 60-240 | Cambia BPM |
| `/time` | secondi | Vai a posizione |

---

## Workflow Rapido (Checklist)

### Ogni volta che vuoi modificare l'UI:

1. **Apri Terminale 1:** Avvia plugin standalone (se non già aperto)
2. **Apri Terminale 2:** `npm run dev` (se non già aperto)
3. **Apri Browser:** `localhost:3000` (dovrebbe essere già aperto)
4. **Apri Ableton/Reaper:** Carica il plugin VST
5. **Modifica `App.jsx`** con il tuo editor (VS Code, Cursor, ecc.)
6. **Salva** → Guarda il browser → **Vedi subito i cambiamenti!**
7. **Testa con Ableton:** Muovi controlli nel DAW → Controlla che si aggiornino nella Web UI

---

## Problemi Comuni & Soluzioni

### Problema: "DISCONNECTED" rimane rosso

**Causa:** Il plugin standalone non è aperto, o il WebSocket non si connette.

**Soluzione:**
1. Controlla che il plugin sia aperto (Terminale 1)
2. Ricarica la pagina browser (F5)
3. Controlla console browser (F12 → Console) per errori

### Problema: Modifiche non si vedono

**Causa:** `npm run dev` si è bloccato.

**Soluzione:**
```powershell
# Premi Ctrl+C nel Terminale 2, poi:
npm run dev
```

### Problema: Ableton non risponde ai comandi

**Causa:** OSC non configurato correttamente.

**Soluzione:**
1. Verifica che il plugin VST sia caricato in Ableton
2. Controlla le porte OSC (9000 in, 9001 out)
3. Controlla firewall Windows (permetti Node.js e il plugin)

---

## File Utili da Consultare

| File | Perché Leggerlo |
|------|-----------------|
| `webview-ui/src/App.jsx` | Interfaccia principale |
| `webview-ui/src/openclaw-bridge.js` | Come funziona il ponte |
| `webview-ui/src/components/BotFace.jsx` | Animazione faccia bot |
| `src/bridge/OscBridge.cpp` | Logica C++ (solo se devi modificare comandi OSC) |
| `docs/protocol-json-v1.md` | Protocollo di comunicazione |

---

## Colori del Tema CREMISI (Per Riferimento)

| Nome | Codice | Uso |
|------|--------|-----|
| Cremisi | `#DC143C` | Accenti, pulsanti importanti, errori |
| Amber | `#FFB000` | Testo attivo, bordi, cursori |
| Sfondo scuro | `#0d0d0d` | Background principale |
| Pannello | `#131313` | Header, sidebar |
| Card | `#1a1a1a` | Box messaggi, contenitori |
| Bordo | `#222222` | Bordi sottili |
| Testo spento | `#4d4d4d` | Testo secondario |
| Testo primario | `#e5e2e1` | Testo principale |

---

## Comandi Git (Per Pushare Modifiche)

Quando hai finito di modificare e vuoi salvare su GitHub:

```powershell
cd C:\percorso\del\progetto\OpenClaw-VST-Bridge

# Vedi cosa hai modificato
git status

# Aggiungi i file modificati
git add webview-ui/src/App.jsx

# Committa con messaggio descrittivo
git commit -m "feat: Aggiunto controllo X e animazione Y"

# Pusha su GitHub
git push origin master
```

---

## Contatti & Aiuto

- **Problemi tecnici:** Chiedi ad Aura (OpenClaw)
- **Design UI:** Discuti con Heartbroken
- **Comandi DAW specifici:** Consulta la documentazione Ableton/Reaper OSC

---

## ⚠️ TODO - Lavori Futuri

### Modifica Plugin C++ per Supporto Ableton Nativo

**Problema:** I comandi OSC attuali funzionano con Reaper ma **NON con Ableton** (richiede Max for Live).

**Soluzione richiesta:** Modificare il codice C++ del plugin per inviare comandi specifici Ableton Live Object Model (LOM).

**File da modificare:**
- `src/bridge/OscBridge.cpp` - Aggiungere mapping comandi Ableton
- `src/core/PluginProcessor.cpp` - Gestione comandi specifici per DAW

**Comandi Ableton da implementare:**

| Azione | Comando Reaper (attuale) | Comando Ableton (da aggiungere) |
|--------|--------------------------|----------------------------------|
| Play | `/action/40044` | `/live/play` o tramite LOM |
| Stop | `/action/40047` | `/live/stop` |
| Record | `/action/40048` | `/live/record` |
| Track Volume | `/track/1/volume` | `/live/track/volume 0 0.75` |
| Track Pan | `/track/1/pan` | `/live/track/pan 0 0.0` |
| Track Mute | `/track/1/mute` | `/live/track/mute 0 1` |
| Track Solo | `/track/1/solo` | `/live/track/solo 0 1` |
| Tempo | `/tempo` | `/live/tempo 120` |
| Undo | `/action/40322` | `/live/undo` |
| Redo | - | `/live/redo` |

**Note implementazione:**
- Aggiungere rilevamento automatico DAW (Reaper vs Ableton)
- Oppure parametro di configurazione esplicito
- Documentare nel protocollo JSON v1.1

**Priorità:** Alta - Necessario per funzionamento completo con Ableton senza Max for Live.

**Assegnato a:** Edo (C++) / Heartbroken (coordinamento)

---

**Buon lavoro! 🎛️🎚️🎵**
