# Come si collauda WhyCremisi

**Ultimo aggiornamento:** 3 agosto 2026

Il procedimento con cui si verifica un plugin commerciale non è "aprirlo e
sentire se suona". È una scala: ogni gradino trova una classe di difetti
che il gradino sotto non vede, e si sale prima di considerare buona una
build.

---

## 1. Test unitari — la logica

```powershell
cmake -B build-test -DBUILD_UNIT_TESTS=ON -DJUCE_WEBVIEW2_PACKAGE_LOCATION="E:/CARTELLE/NuGet"
cmake --build build-test --config Release --target WhyCremisiTests
build-test\WhyCremisiTests_artefacts\Release\WhyCremisiTests.exe
```

Verifica il comportamento del codice a prescindere dall'host: parametri,
gain, costruzione e distruzione degli oggetti. Girano in pochi secondi,
quindi si lanciano spesso.

**Stato:** 2057 controlli, nessuno fallito.

Trovati così: il crash quando `processBlock` arriva senza `prepareToPlay`,
e il gain che impiegava cento secondi invece di cento millisecondi.

---

## 2. Misura del segnale — il suono

```powershell
E:\AcapellaLab\venv_mix\Scripts\python.exe <script>
```

`pedalboard` carica il VST3 vero e ci fa passare segnali di cui si conosce
la risposta attesa: tono mono, tono fuori fase, stereo largo, silenzio. Poi
si misura l'uscita e si confronta con quello che dovrebbe essere.

Serve per le domande che nessun test logico può porre: il plugin è
trasparente quando dovrebbe? Il compressore comprime quanto dice? La
correlazione di fase risponde come deve?

Trovato così: il DSP che comprimeva il master senza che nessuno lo avesse
chiesto.

---

## 3. pluginval — l'host simulato

```
Tools\valida.cmd 5
```

È il gradino che risponde alla domanda "si comporterà bene dentro un DAW".
`pluginval` carica il plugin come farebbe un host e gli fa quello che un
host fa davvero:

| Prova | Cosa smaschera |
|---|---|
| Apertura a freddo e a caldo | Stato sporco fra un caricamento e l'altro |
| Editor aperto mentre l'audio scorre | La classe di crash più comune nei plugin |
| Salvataggio e ricarica dello stato | Sessioni che si riaprono diverse da come sono state chiuse |
| Automazione fitta sui parametri | Rotture sotto scrittura rapida dei valori |
| Cambio di configurazione dei bus | Comportamento fuori dallo stereo |
| Validatore Steinberg incluso | Conformità formale al VST3 |

Severità: 1 minima, **5 quella raccomandata prima di un rilascio**, 10 il
massimo (ripete le prove con parametri casuali, più lento).

**Stato:** 19 blocchi superati a severità 5, nessun fallimento.

È lo stesso strumento usato dai produttori di plugin commerciali. Trova i
guasti che si manifestano solo dentro un host — cioè quelli che provando a
mano non si vedono, perché a mano non si aprono e chiudono cento volte
l'editor mentre l'audio scorre.

---

## 4. Il DAW vero

Nessuna simulazione sostituisce questo passaggio, ma ci arriva dopo: i
primi tre gradini servono proprio a non sprecare una sessione vera su
difetti che si potevano trovare in venti secondi.

Qui si verifica ciò che solo un host reale mette alla prova: la scansione
dei plugin, il comportamento dentro il grafo audio della DAW, l'OSC vero,
la latenza dichiarata, la convivenza con altri plugin, il salvataggio del
progetto.

Vedi `ABLETON.md` per l'installazione e il log.

---

## L'ordine conta

Salire i gradini in ordine è ciò che rende il collaudo economico: ogni
livello costa più tempo del precedente, e trovare un difetto in basso vale
molto di più che trovarlo in alto. Un crash scoperto da un test unitario
costa secondi; lo stesso crash scoperto durante una sessione di lavoro
costa la sessione.

## Cosa manca ancora

- **Integrazione continua**: nessuna. È il motivo per cui `master` è rimasto
  non compilabile per due mesi senza che nessuno lo sapesse. I tre gradini
  automatici sopra andrebbero eseguiti a ogni push.
- **Matrice di host**: oggi si prova solo su Ableton. Un plugin commerciale
  si verifica su Live, Logic, Pro Tools, Cubase, Reaper e FL, perché ognuno
  ha abitudini diverse.
- **Regressione audio**: confrontare l'uscita con quella di una build
  precedente su materiale fisso, per accorgersi quando una modifica cambia
  il suono senza volerlo.
