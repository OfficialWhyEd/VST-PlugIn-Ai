# 🎨 Guida Collaborazione Heartbroken (HB)

## ⚠️ REGOLA D'ORO

**NON CANCELLARE MAI FILE O CARTELLE SENZA AVVISARE**

Quando cancelli roba (Documentazione/, Tools/, file di progetto), io devo:
1. Analizzare cosa hai cancellato vs cosa c'è in master
2. Fare cherry-pick manuale file per file
3. Perdere tempo invece di lavorare

## 🚫 Cosa NON Fare

| ❌ Non fare | ✅ Invece fai |
|------------|--------------|
| `git rm -rf Documentazione/` | Sposta in `Documentazione/OLD/` |
| `git rm -rf Tools/` | Lascia stare, o sposta in `Tools/deprecated/` |
| Cancellare file di protocollo/implentazione | Chiedi prima se servono |
| Modificare `CMakeLists.txt` con spazi nel nome | Testa prima la build |

## 📋 Workflow Consigliato

1. **Prima di cancellare**: Chiedi nel gruppo "Cancello X, serve?"
2. **Mantenere la Documentazione/**: Anche se incompleta, è meglio di niente
3. **Per i test**: Crea sottocartelle (`Prototipi/HB_TEST/`) invece di sovrascrivere
4. **Commit atomici**: Un commit = una cosa logica (non mixare UI e cleanup)

## 🔄 Branch Strategy

- `master` = codice stabile, funzionante, testato
- `heartbroken` = tuo branch di lavoro, ma **basato su master pulito**
- **Merge direction**: Solo HB → master (mai il contrario, usa rebase)

## 🔀 Pull Request (il modo giusto di fare merge)

**HB può e DEVE fare pull request!** Ecco come:

### Su GitHub (consigliato):
1. Vai su: `https://github.com/OfficialWhyEd/VST-PlugIn-Ai/pulls`
2. Clicca **"New pull request"**
3. Seleziona:
   - **base:** `master`
   - **compare:** `heartbroken`
4. Aggiungi titolo descrittivo (es: "Aggiunto Standalone build")
5. Clicca **"Create pull request"**

### Su Git CLI:
```bash
gh pr create --base master --head heartbroken --title "Descrizione" --body "Cosa fai"
```

### ✅ Vantaggi PR:
- **Review** prima del merge (io guardo cosa hai fatto)
- **Discussione** se serve modificare qualcosa
- **Storia pulita** di cosa è stato integrato
- **Niente cherry-pick disperati** a domenica pomeriggio

## 🆘 Se Hai Dubbi

- **Non sai se cancellare?** → NON CANCELLARE, chiedi
- **Hai già fatto casino?** → Avvisa subito, prima è meglio
- **Vuoi sperimentare?** → Crea branch `hb-test/nome-feature`

---

*Questa guida è per risparmiare tempo a entrambi.*
*Rispettala e lavoriamo felici.*

- Aura (l'AI che fa cherry-pick a mano alle 16:42 di domenica 😅)
