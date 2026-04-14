# WHYCREMISI MASTER SPECIFICATION (Design System)
**Versione:** 1.0 (Base Prototipo Kinetic Interactive Advisory)
**Stato:** MASTER

---

## 🎨 Identità Visiva (Art Direction)
- **Palette Colori:**
    - Primario: `#DC143C` (Rosso Cremisi)
    - Secondario: `#FFB000` (Ambra/Glow)
    - Sfondo: `#0d0d0d` / `#131313`
    - Testo/UI: `#e5e2e1`
- **Tipografia:**
    - Family: 'Space Grotesk'
- **Effetti Grafici Identitari:**
    - **CRT Overlay:** Griglia fine e scanlines (trasparenza 0.02-0.05).
    - **LED Feedback:** Effetti di glow (box-shadow) su stati attivi (`led-red-active`, `led-amber-active`).
    - **Animations:** Glitch sui bottoni al passaggio del mouse, effetto "Breathe" (pulsazione) sui bordi delle finestre Advisory.

## 🏗️ Struttura Componenti (Modularità)
1. **Layout**: Header (Navigazione), Side Module (AI Engine), Main Console (Chat/Mastering), Mastering Rack (Knobs/Gain), Vector Scope (Grafici live).
2. **Componenti UI**:
    - **Knobs**: Stile circolare con segmenti, indicatore visivo di valore, reset su double-click.
    - **Chat Terminal**: Interfaccia monospaced con cursore lampeggiante.
    - **Advisory Card**: Box informativo centrale con effetto "breathe".
    - **Vector Scope**: Rappresentazione grafica realtime (Canvas/WebGL).

## 📡 Integrazione Bridge (C++ <-> Web)
- **Comunicazione**: JSON bidirezionale via `window.postMessage`.
- **Throttling**: Gli aggiornamenti della UI non devono superare i 30-60fps per evitare sovraccarico CPU.
- **Feedback Live**: Ogni variazione di parametro audio nel motore C++ deve triggerare un evento nel Bridge che aggiorna lo stato dei componenti React.

## 🛠️ Regole di Modifica
- Ogni modifica grafica DEVE mantenere il contrasto cromatico tra `#DC143C` e `#FFB000`.
- Non aggiungere effetti pesanti senza prima testare il peso sulla RAM.
- Ogni nuovo tool deve essere un componente React a sé stante, iniettabile nel `Main Prototipo`.

---
*Questa specifica rappresenta il riferimento obbligatorio per qualsiasi sviluppo UI futuro.*
