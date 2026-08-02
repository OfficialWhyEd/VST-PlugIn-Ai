# TODO — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

Questo file elenca le cose aperte **fuori** dalla roadmap dei 100 step (`Documentazione/ROADMAP.md`), che resta il piano di sviluppo vero e proprio.

---

## Alta priorità

- [ ] **Verificare il plugin dentro un DAW su Windows** — il VST3 ora compila, ma va provato in Reaper e in Ableton: caricamento, apertura UI, comandi OSC.
- [ ] **Ricompilare su Mac dopo il merge** — il ramo unificato è stato verificato solo su Windows. Su macOS vanno riprovati il blocco `if(APPLE)` del `CMakeLists.txt` e il formato AU.
- [ ] **Allineare la versione di JUCE** — su Mac è stato usato JUCE 8.0.12, su Windows c'è 8.0.4. Le API della WebView differiscono fra le due.
- [ ] **Testare il feedback OSC dal DAW al plugin** — la direzione plugin → DAW funziona, l'inversa non è mai stata verificata davvero.
- [ ] **Collegare la catena DSP alla UI e all'AI** — EQ, compressore e limiter esistono e funzionano, ma non è raggiungibile nulla: `DSPEngine::setBypass()` non viene chiamato da nessuna parte e nel bridge OSC non c'è un solo comando che imposti soglia, ratio o bande. Il `CompressorBox` della UI manda comandi che non arrivano al DSP interno. Finché non sono cablati, i moduli restano bypassati.

## Media priorità

- [ ] **Preferenze configurabili** — IP e porte OSC/WebSocket si impostano ancora modificando il codice.
- [ ] **Mappare tutti i parametri OSC di Reaper** — volume, pan, mute, solo, tempo, posizione.
- [ ] **Supporto OSC per Ableton** — `AbletonDawHandler` esiste ma è poco più di uno scheletro.
- [ ] **CI su GitHub Actions** — build automatica per Windows e macOS a ogni push (step 80 della roadmap). Avrebbe intercettato subito il fatto che `master` non compilava.

## Bassa priorità

- [ ] `getCurrentPosition` è deprecato in JUCE 8 → usare `getPosition` in `PluginProcessor`.
- [ ] Pulire la logica di riconnessione del WebSocket.
- [ ] Vector Scope con dati audio reali (l'analyzer ormai li fornisce).
- [ ] Decidere che fare dei branch storici `heartbroken`, `heartbroken-claude`, `aura` una volta riunificati.

---

## Fatto

- [x] **Build Windows sbloccata** (01/08/2026) — guard su `SIGPIPE`, WebView2 attivato nel `CMakeLists.txt`, percorso della UI reso cross-platform, copia della dist React anche su Windows.
- [x] **Regole del progetto riscritte** (01/08/2026) — rimosso tutto l'impianto a tre persone; vedi `Documentazione/WORKFLOW.md`.
- [x] **Rami riunificati** (01/08/2026) — `master` e `heartbroken-claude` riuniti; nel merge sono emersi il `getName()` spezzato che impediva a `master` di compilare, l'escaping JavaScript mai eseguito e il flag `/utf-8` mancante per MSVC.
- [x] **Documentazione separata dal sito** (01/08/2026) — `docs/` è solo la landing page GitHub Pages, la documentazione sta in `Documentazione/`.
- [x] **Riferimenti al repository corretti** (01/08/2026) — il repo è stato rinominato in `OfficialWhyEd/WhyCremisi`: aggiornati i comandi di clone e rimosso il remote `carlo`, che puntava a un repository non più esistente.
- [x] **Il plugin non colora più il suono da solo** (01/08/2026) — misurando l'uscita con `pedalboard` è saltato fuori che con i default di fabbrica comprimeva 4:1 sopra −24 dB e limitava a −6 dB, attenuando di 12,7 dB un segnale a −9 dBFS RMS. Su un plugin che si carica sul master e che nasce per analizzare, è un comportamento sbagliato. Ora i moduli partono bypassati.
