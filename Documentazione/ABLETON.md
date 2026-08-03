# WhyCremisi in Ableton Live

**Ultimo aggiornamento:** 2 agosto 2026

Guida per far girare il plugin dentro Live su Windows. Tre passaggi, in
quest'ordine.

---

## 1. Installare il plugin

Il VST3 va nella cartella dei plugin di sistema, che richiede permessi da
amministratore. Lo script se li fa dare da solo:

```
Tools\installa-vst3.cmd
```

Compila prima, se non l'hai già fatto — la UI React va costruita per prima,
altrimenti il plugin si apre su una finestra vuota:

```powershell
cd webview-ui
npm run build
cd ..
$env:JUCE_ROOT = "E:\CARTELLE\JUCE"
cmake --build build-win --config Release --parallel 4
```

Poi in Live: **Preferenze → Plug-Ins → Rescan**. Il plugin compare come
**WhyCremisi** fra gli effetti audio VST3.

> Live è più severo di altre DAW nella scansione: se il plugin non appare,
> guarda in Preferenze → Plug-Ins se la cartella VST3 di sistema è attiva.

---

## 2. Installare AbletonOSC

Live da solo **non espone nessuna porta OSC**. Per farlo serve
[AbletonOSC](https://github.com/ideoforms/AbletonOSC), un Remote Script di
terze parti con licenza MIT.

```
Tools\installa-abletonosc.cmd
```

Non serve l'amministratore: si installa nella User Library. Lo script
scarica ed esegue codice di terzi dentro il DAW, quindi lancialo solo se
ti sta bene.

Poi in Live:

1. **Preferenze → Link/Tempo/MIDI**
2. In **Control Surface** scegli **AbletonOSC**
3. Riavvia Live

Se compare nell'elenco, Live ascolta sulla **11000** e risponde sulla
**11001**.

---

## 3. Impostare le porte nel plugin

Le porte predefinite del plugin sono quelle di Reaper. Per Live vanno
cambiate — è il motivo più comune per cui sembra che "non faccia niente":
i comandi partono ma nessuno li ascolta.

Dal plugin, manda la configurazione:

```json
{ "type": "config.set", "payload": { "key": "daw.preset", "value": "ableton" } }
```

Il preset imposta invio sulla **11000**. La porta di ascolto (**11001**)
si applica al riavvio del listener.

| DAW | Il plugin invia su | Il plugin ascolta su |
|-----|--------------------|----------------------|
| Ableton Live (AbletonOSC) | 11000 | 11001 |
| Reaper | 8000 | 9000 |

---

## Il log

Il plugin scrive sempre, anche nella build Release, in:

```
%APPDATA%\WhyCremisi\whycremisi.log
```

Si può aprire con Blocco note mentre Live sta suonando. Ogni riga porta
l'ora al millisecondo e l'area che l'ha scritta; a ogni caricamento c'è
un'intestazione con frequenza di campionamento, dimensione dei blocchi e
nome dell'host, così si distingue una sessione dall'altra e si capisce a
quale istanza appartiene ciascuna riga. Il file ruota a 5 MB: la versione
precedente diventa `whycremisi.log.1`.

Cosa registra: avvio del bridge e porte occupate, connessioni
dell'interfaccia, comandi ricevuti, messaggi OSC in entrata e in uscita,
modifiche alla catena DSP, errori.

Se qualcosa non va, quel file è la prima cosa da guardare — e da allegare
quando si chiede aiuto.

## Come capire se sta funzionando

- Nel log del plugin compaiono righe `[OSC] SENT: /live/song/...`
- Premendo play dal plugin, Live parte
- Il pannello tracce si popola dopo la scoperta automatica

Se i comandi partono ma Live non reagisce, quasi sempre è una di queste:

| Sintomo | Causa probabile |
|---|---|
| Nessuna reazione, nessun errore | AbletonOSC non attivo in Control Surface, o porte sbagliate |
| Funziona il trasporto ma non il mixer | Versione di AbletonOSC diversa da quella attesa |
| Il plugin non compare in Live | Cartella VST3 non scansionata, oppure manca il Rescan |

---

## Nota sul protocollo

Il plugin parla il dialetto di **AbletonOSC**, dove l'indice della traccia
è un *argomento* del messaggio:

```
/live/track/set/volume   [2, 0.85]
```

Il vecchio **LiveOSC** metteva l'indice nell'indirizzo
(`/live/track/2/set/volume`). Sono incompatibili: con gli indirizzi
sbagliati Live non risponde e non segnala nulla. Il codice conserva la
lettura in formato LiveOSC per chi usasse ancora quel Remote Script, ma
tutto ciò che il plugin invia è nel formato AbletonOSC.

Gli indici delle tracce partono da **zero**.
