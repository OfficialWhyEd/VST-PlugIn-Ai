# TODO.md - VST-PlugIn-Ai Development Roadmap

## 🎯 Obiettivo Corrente: Interfaccia Editor Effetti & Architettura Modulare
*Focus: Implementazione della GUI reattiva con 8 knob e sistema di feedback visivo.*

---

## 🏗️ FASE 1: Fondamenta Interfaccia (UI Core)
- [ ] **Definizione Layout Base**: Configurare `PluginEditor.h/cpp` per un'interfaccia responsive (RF2.1).
- [ ] **Implementazione Componenti Knob**: Creare una classe `CustomSlider` derivata da `juce::Slider` per gestire i knob con lo stile del progetto.
- [ ] **Attachment Parametri**: Collegare i knob ai parametri `gain1-8` definiti nel `PluginProcessor` tramite `AudioProcessorValueTreeState::SliderAttachment`.
- [ ] **Visualizzazione Valori**: Aggiungere label dinamiche per mostrare nome, valore e unità di misura (RF2.3, RF2.4).

---

## 🎨 FASE 2: Editor Modulare & Configurabile
- [ ] **Editor Modulare**: Progettare un sistema di gestione comandi flessibile per permettere a Edo di implementare nuovi tool/effetti in modo modulare su futuri plugin.
- [ ] **Interfaccia Configurabile**: Implementare un pannello dove l'utente può aggiungere/rimuovere strumenti o widget (tools) nel pannello principale del plugin.

---

## 🎓 FASE 3: Sistema Tutorial AI (DAW Interaction)
- [ ] **Guida AI Interattiva**: Implementare la logica per cui l'AI può inviare messaggi al DAW per guidare l'utente inesperto (lezioni step-by-step su Ableton/Reaper).
- [ ] **Visual Feedback Tutorial**: L'AI deve poter evidenziare visivamente nella UI o inviare comandi al DAW per mostrare come funzionano i controlli.

---

## 🌐 FASE 4: "OpenClaw DAW" (Remote Control)
- [ ] **Integration Bridge**: Creare un bridge di comunicazione tra Telegram/WhatsApp e il plugin VST.
- [ ] **Remote Control Engine**: Implementare il controllo dei parametri del DAW basato su messaggi ricevuti esternamente.

---

## 🛠️ FASE 5: Validazione & Test
- [ ] **HiDPI Support**: Verificare lo scaling corretto su schermi 4K/HiDPI (RF2.12).
- [ ] **Performance UI**: Profilazione per garantire un rendering fluido a 60fps (RF2.11).

---

## 🍎 FASE 6: Portabilità & Apple Ecosystem
- [ ] **Supporto macOS**: Configurare il sistema di build per generare plugin `.vst3` e `.component` (AU) nativi per macOS (Intel e Apple Silicon).
- [ ] **Code Signing & Notarization**: Implementare il protocollo di firma e notarizzazione richiesto da Apple per far girare i plugin su macOS (RNF5.2).
- [ ] **UI Scaling (Retina)**: Ottimizzare i componenti grafici per display Retina su macOS.

---

## 📱 FASE 7: Mobile Companion App & Remote Connectivity
- [ ] **Mobile Connectivity Bridge**: Implementare un protocollo di comunicazione sicura (es. WebSockets o gRPC) per connettere il plugin VST a dispositivi mobile (Android/iOS).
- [ ] **Cross-Platform Mobile GUI**: Discutere e selezionare il framework per sviluppare l'app mobile (es. Flutter, React Native, Kotlin Multiplatform).
- [ ] **Remote DAW Control**: Sviluppare la logica per inviare comandi dalla companion app mobile al plugin VST.

---

## 📈 FASE 8: Sistema di Tracking & Logging Automatizzato (GitOps)
- [ ] **Git Log Exporter**: Implementare uno script (Python o shell) che estrae automaticamente: `push`/`pull`, `git diff` e firme agente ([JETTY], [AURA], [HEARTBROKEN]).
- [ ] **Sincronizzazione Excel/CSV**: Automatizzare la scrittura di questi dati in un registro storico per tracciare le modifiche.
- [ ] **Dashboard Aggiornamenti**: Integrare il log nel sistema Visual Agents.

---

## 🚀 FASE 9: AI Prompt System & User Community Engagement
- [ ] **Smart Command System**: Implementare un sistema di "slash-commands" (es. `/boost-vocal`, `/tighten-bass`) nella chat del plugin per attivare macro complesse istantaneamente.
- [ ] **Community Polling Engine**: Integrare un sistema di sondaggi in-app per chiedere direttamente ai produttori quali nuovi comandi o tool AI desiderano vedere implementati.
- [ ] **Research-Driven Development**: Automatizzare la raccolta dei feedback dai sondaggi per aggiornare la roadmap di sviluppo in base alle necessità reali degli utenti.

---

## 🎭 FASE 10: AI Avatar & Kinetic Presence (The Living Logo)
- [ ] **3D Kinetic Logo Integration**: Implementare un modello 3D (WebGL/Three.js) per il logo WhyCremisi come avatar.
- [ ] **AI Thinking State**: Animazione "pensante" durante l'elaborazione (es. rotazione/contrazione) per segnalare attività AI.
- [ ] **Interactive Gaze (Mouse Tracking)**: Sistema di tracking dello sguardo: il logo segue il movimento del mouse quando non è in elaborazione.
- [ ] **Chat-Head Presence**: Visualizzazione "avatar" accanto alle risposte dell'AI, simile alle chat di gruppo.
- [ ] **Art Direction**: Definire la direzione artistica del modello 3D in coerenza con `MASTER_SPEC.md`.

---

## ⚓ Note di Sincronizzazione [JETTY]
- *Prima di iniziare ogni task: `git pull` per verificare modifiche di Aura.*
- *A task completato: commit con tag `[JETTY]` e aggiornamento di questo file.*
