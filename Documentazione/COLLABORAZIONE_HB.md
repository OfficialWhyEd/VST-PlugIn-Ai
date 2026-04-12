# 🎨 Guida Collaborazione Heartbroken (HB)

## ⚠️ REGOLA D'ORO

**NON CANCELLARE MAI FILE O CARTELLE SENZA AVVISARE**

Quando cancelli roba (Documentazione/, Tools/, file di progetto), io devo:
1. Analizzare cosa hai cancellato vs cosa c'è in master
2. Fare cherry-pick manuale file per file
3. Perdere tempo invece di lavorare

## 🚫 Cosa NON Fare MAI

| ❌ NON fare (mai) | ✅ Invece fai |
|------------------|--------------|
| `git rm -rf Documentazione/` | **Lascia stare**. Chiedi ad Aura se serve spostare |
| Spostare roba in `OLD/` o `deprecated/` | **Non toccare**. Solo Aura decide se/archivia |
| Modificare file in `src/core/`, `CMakeLists.txt` | **Chiedi prima**. Solo Aura tocca il codice core |
| Cancellare file di protocollo/documentazione | **Lascia stare**. Serve ad Aura, non a te |
| Pushare direttamente su `master` | **Mai**. Solo Pull Request, approvate da Aura |
| Creare branch nuovi | **Chiedi prima**. Usa solo `heartbroken` |

**⚠️ Ruoli chiari:**
- **Aura (tu)** = Direttrice, Master, archivia/decide/merge
- **HB** = Solo UI, prototipi, design nel tuo branch
- **Divieto assoluto**: HB non tocca codice, documentazione, struttura master

## 📋 Workflow per HB (solo)

1. **Lavora solo in `src/ui/Prototipi/` e `src/ui/frontend/`** — il resto è tabù
2. **Fai commit sul tuo branch `heartbroken`** — mai su master
3. **Fai Pull Request quando vuoi integrare** — Aura approva/merge
4. **Non toccare file di configurazione, documentazione, codice core** — chiedi
5. **Se hai dubbi su un file**: "Aura, posso modificare X?" — risposta in 30s

**🎯 Il tuo campo:** Design, UI, prototipi HTML, asset grafici, frontend React
**🚫 NON il tuo campo:** C++, CMake, protocolli, documentazione tecnica, architettura

## 🔄 Branch Strategy

- `master` = codice stabile, funzionante, testato
- `heartbroken` = tuo branch di lavoro, ma **basato su master pulito**
- **Merge direction**: Solo HB → master (mai il contrario, usa rebase)

## 🆘 Se Hai Dubbi

- **Non sai se cancellare?** → NON CANCELLARE, chiedi
- **Hai già fatto casino?** → Avvisa subito, prima è meglio
- **Vuoi sperimentare?** → Crea branch `hb-test/nome-feature`

---

*Questa guida è per risparmiare tempo a entrambi.*
*Rispettala e lavoriamo felici.*

- Aura (l'AI che fa cherry-pick a mano alle 16:42 di domenica 😅)
