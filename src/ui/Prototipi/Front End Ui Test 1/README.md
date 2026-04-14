# VST-PlugIn-Ai - Documentazione Avatar & UI

Questo documento descrive l'implementazione dell'interfaccia utente (UI) e in particolare del componente Avatar (BotFace), progettato per essere il punto focale visivo ed emotivo del plugin VST.

## 1. Filosofia del Design (Avatar)

L'obiettivo principale del `BotFace` è essere **estremamente minimale, fluido ed emotivo**. 
Abbiamo abbandonato design complessi (HUD olografici, anelli rotanti, glitch pesanti) in favore di un approccio basato sulla pura espressione vettoriale.

### Caratteristiche Chiave:
- **Zero Component Swap**: L'avatar non smonta e rimonta componenti diversi per ogni stato. Utilizza **3 singoli percorsi SVG** (Occhio Sinistro, Occhio Destro, Bocca).
- **Path Morphing Perfetto**: Grazie a `framer-motion`, l'attributo `d` (data) dei percorsi SVG viene animato in modo fluido. Questo significa che il passaggio da uno stato felice a uno triste avviene con una transizione organica e continua delle linee, senza scatti.
- **Fisica a Molla (Spring Physics)**: Le transizioni utilizzano curve di tipo `spring` (stiffness: 80, damping: 12) per dare un senso di peso e biologia ai movimenti dell'avatar.
- **Respiro Costante**: L'intero SVG fluttua leggermente sull'asse Y (`y: [0, -3, 0]`) in un loop continuo per sembrare sempre vivo, anche quando è in stato `idle`.

## 2. Stati Emotivi Implementati (`BotState`)

L'avatar reagisce in tempo reale al contesto del plugin. Gli stati attuali sono:

1. **`idle`**: Stato di riposo. Occhi aperti a mezza luna, sorriso accennato. Include un'animazione di battito di ciglia (blink) ogni 5 secondi.
2. **`sad`**: Stato di tristezza/anomalia. Gli occhi si inclinano verso il basso e l'interno, la bocca diventa una curva all'ingiù. L'intero viso si abbassa sull'asse Y per simulare "pesantezza".
3. **`thinking`**: Elaborazione. Gli occhi si rimpiccioliscono e guardano verso l'alto a destra. La bocca diventa un piccolo punto di concentrazione. Il viso si solleva leggermente.
4. **`typing`**: Risposta in corso. Occhi aperti, la bocca si apre e si chiude in un loop fluido per simulare il parlato.
5. **`listening`**: In attesa di input/analisi audio. Occhi spalancati, bocca piatta che vibra leggermente.
6. **`error`**: Errore critico. Occhi inclinati a formare una "V" arrabbiata, bocca a zig-zag.
7. **`success`**: Operazione riuscita. Occhi ad arco molto ampi, sorriso grande. L'avatar "salta" leggermente verso l'alto.
8. **`loading`**: Caricamento generico. Occhi chiusi (linee orizzontali), bocca piatta.
9. **`advisory`**: Avviso/Suggerimento. Espressione neutra ma attenta.

## 3. Integrazione nel Sistema (App.tsx)

Il `BotFace` è strettamente collegato al motore di chat e telemetria in `App.tsx`.

### Come interagisce:
- Quando l'utente invia un comando, lo stato passa immediatamente a `thinking`.
- Durante la generazione della risposta (simulata tramite `simulateTyping`), lo stato passa a `typing`.
- Il sistema analizza il testo in ingresso per determinare lo stato finale (`endState`):
  - Se l'utente digita parole come **"triste"**, **"sad"** o **"male"**, l'avatar risponde in modo empatico e il suo stato finale diventa `sad`.
  - Se viene rilevato un errore (es. comando sconosciuto o parole come "error", "fail"), lo stato diventa `error`.
  - Per comandi di analisi (es. `/analyze_spectral`), lo stato può diventare `advisory`.

### Estendibilità per Antigravity:
Per chi continuerà lo sviluppo (Antigravity o altri dev):
1. **Nuovi Stati**: Per aggiungere un nuovo stato, basta aggiungerlo al tipo `BotState` in `BotFace.tsx` e definire i 3 percorsi SVG (`l`, `r`, `m`) nell'oggetto `paths`.
2. **Collegamento al Backend VST**: Attualmente le risposte sono simulate in `handleCommand`. Quando il plugin C++ invierà dati reali tramite WebSocket o IPC, basterà mappare i payload in ingresso per aggiornare la variabile di stato `botState` in React.

## 4. Struttura del Codice Rilevante
- `src/components/BotFace.tsx`: Il cuore dell'avatar vettoriale morphing.
- `src/App.tsx`: La logica di gestione dello stato, la console dei comandi e l'integrazione UI.

---
*Documentazione generata durante la sessione di ottimizzazione UI/UX.*
*[ HB + ]*
