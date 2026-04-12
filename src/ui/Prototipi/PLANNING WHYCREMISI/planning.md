# PLANNING: WHYCREMISI - Progetto VST AI
**Stato:** Prototipazione UI (Fase 1)
**Data:** 12 Aprile 2026

---

## 🎯 Obiettivo Strategico
Trasformare il prototipo `whycremisi_kinetic_interactive_advisory` in un plugin VST3 professionale, modulare e interattivo, integrando il backend C++ con una UI React "Ultra-Premium".

---

## 🧪 Log Avanzamento Progetto (12 Aprile 2026)

### Stato: Fase 1 - Analisi e Architettura UI
*   **Target Prototipo**: `whycremisi_kinetic_interactive_advisory`
*   **Analisi Tecnica**: 
    *   Estratta la logica estetica (Tailwind, `crt-overlay`, `led-amber-active`, animazioni `breathe`).
    *   Identificata la struttura modulare: Side Module, AI Command Console, Mastering Rack, Vector Scope.
    *   Confermato l'uso di `WebViewBridge` per comunicazione JSON bidirezionale.
*   **Architettura Modulare Proposta**:
    1. `WhyCremisiLayout.tsx`: Grid-System principale.
    2. `SidebarModule.tsx`: Gestione stati attivi.
    3. `AIConsole.tsx`: Logica chat/terminale con typing effect.
    4. `MasteringRack.tsx`: Slider Gain + Controlli AI (con bridge JSON).
    5. `VectorScope.tsx`: Canvas per visualizzazione segnale.
*   **Conclusione Analisi**: Il prototipo `kinetic_interactive_advisory` è la "Bibbia" visiva di WhyCremisi.
*   **Prossimi Step Fase 1**:
    1.  [x] Estrarre variabili colore/font dal prototipo (`#DC143C`, `#FFB000`, `Space Grotesk`).
    2.  [ ] Tradurre layout `code.html` in componenti React modulari (WhyCremisiLayout, AIConsole, GainModule, TerminalLog, VectorScope).
    3.  [ ] Mappare le chiamate `window.postMessage` nel bridge C++ (`WebViewBridge.cpp/h`) per connettere la console AI e lo slider Gain.
    4.  [ ] Implementare logica di visualizzazione (Scope Canvas) basata su dati C++.
