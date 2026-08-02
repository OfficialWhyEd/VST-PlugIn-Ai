# TODO — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

Questo file elenca le cose aperte **fuori** dalla roadmap dei 100 step (`docs/ROADMAP.md`), che resta il piano di sviluppo vero e proprio.

---

## Alta priorità

- [ ] **Riunificare `master` e `heartbroken-claude`** — i due rami si sono separati il 09/05/2026: `heartbroken-claude` ha tutto il codice di maggio, `master` ha solo README, branding e landing page di giugno. Serve un merge che tenga entrambe le cose.
- [ ] **Allineare la versione di JUCE** — su Mac è stato usato JUCE 8.0.12, su Windows c'è 8.0.4. Le API della WebView differiscono fra le due.
- [ ] **Verificare il plugin dentro un DAW su Windows** — il VST3 ora compila, ma va provato in Reaper e in Ableton: caricamento, apertura UI, comandi OSC.
- [ ] **Testare il feedback OSC dal DAW al plugin** — la direzione plugin → DAW funziona, l'inversa non è mai stata verificata davvero.

## Media priorità

- [ ] **Preferenze configurabili** — IP e porte OSC/WebSocket si impostano ancora modificando il codice.
- [ ] **Mappare tutti i parametri OSC di Reaper** — volume, pan, mute, solo, tempo, posizione.
- [ ] **Supporto OSC per Ableton** — `AbletonDawHandler` esiste ma è poco più di uno scheletro.
- [ ] **CI su GitHub Actions** — build automatica per Windows e macOS a ogni push (step 80 della roadmap).
- [ ] **Aggiornare il README** — la sezione build parla solo di macOS e dichiara JUCE 7 mentre il progetto usa JUCE 8.

## Bassa priorità

- [ ] `getCurrentPosition` è deprecato in JUCE 8 → usare `getPosition` in `PluginProcessor`.
- [ ] Pulire la logica di riconnessione del WebSocket.
- [ ] Vector Scope con dati audio reali (l'analyzer ormai li fornisce).
- [ ] Decidere che fare dei branch storici `heartbroken`, `heartbroken-claude`, `aura` una volta riunificati.

---

## Fatto

- [x] **Build Windows sbloccata** (01/08/2026) — guard su `SIGPIPE`, WebView2 attivato nel `CMakeLists.txt`, percorso della UI reso cross-platform, copia della dist React anche su Windows.
- [x] **Regole del progetto riscritte** (01/08/2026) — rimosso tutto l'impianto a tre persone; vedi `docs/WORKFLOW.md`.
