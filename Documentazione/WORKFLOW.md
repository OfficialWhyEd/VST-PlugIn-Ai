# Workflow — WhyCremisi

**Ultimo aggiornamento:** 1 agosto 2026

---

## Chi lavora al progetto

**Edo (WhyEd)**, da solo, affiancato da Claude.

Il progetto è nato come lavoro a tre (Edo, Carlo con l'agente "Aura", l'agente "Heartbroken") e le vecchie regole erano fatte per evitare che tre persone si pestassero i piedi: branch personali, prefissi obbligatori nei commit, aree di codice vietate, review incrociata prima di ogni merge.

**Carlo non lavora più al progetto dal 1° agosto 2026.** Quelle regole non servono più e sono state rimosse. Niente permessi da chiedere, niente aree proibite, niente prefissi `AURA:` / `HEARTBROKEN:`.

---

## Branch

| Branch | A cosa serve |
|--------|--------------|
| `master` | Il ramo stabile. Ciò che compila e funziona. |
| `feat/…`, `fix/…`, `docs/…` | Branch di lavoro temporanei, uno per cosa. Si cancellano dopo il merge. |

I branch storici `heartbroken`, `heartbroken-claude` e `aura` appartengono alla vecchia organizzazione. `aura` e `heartbroken` sono interamente contenuti in `heartbroken-claude` e non contengono nulla di unico.

### Se il lavoro è breve
Committa direttamente su `master`. Sei solo: non c'è nessuno da bloccare.

### Se il lavoro è lungo o rischioso
```bash
git switch -c feat/nome-chiaro
# ... lavora, committa ...
git switch master
git merge --no-ff feat/nome-chiaro
git branch -d feat/nome-chiaro
git push
```

---

## Messaggi di commit

Formato convenzionale, in inglese o italiano purché coerente:

```
feat: aggiunto vectorscope con dati reali
fix: crash alla chiusura del WebSocket server
docs: aggiornato STATUS con la sessione di agosto
chore: riorganizzata la cartella Research
```

Niente prefissi con il nome dell'agente: il campo autore di git basta già a dire chi ha scritto cosa.

---

## Prima di committare

1. `git status` — sai cosa stai per includere
2. Il progetto compila (`cmake --build build --config Release`)
3. Se hai toccato la UI: `npm run build` in `webview-ui/` passa

Non c'è altro. Se hai rotto qualcosa, `git revert` esiste.

---

## Dove sta cosa

| Area | Cartella |
|------|----------|
| Audio processor, parametri, sessioni | `src/core/` |
| Bridge OSC + WebSocket | `src/bridge/`, `src/osc/` |
| Motore AI e provider | `src/ai/` |
| Personalità e memoria dell'agente | `src/agent/` |
| DSP (analisi, comp, limiter, EQ) | `src/dsp/` |
| Handler per Reaper e Ableton | `src/daw/` |
| Interfaccia React | `webview-ui/` |
| Documenti di prodotto e ricerca | `Research/`, `Documentazione/product/` |
| Loghi e icone | `Research/logo/` |

---

## Note operative

- **Non riscrivere la storia già pushata** (`push --force` su `master`). Tutto il resto si aggiusta.
- **`.secrets` è nel `.gitignore`**: le chiavi API non vanno nel repo, mai. Il plugin le tiene in memoria e le salva in `AppData/Roaming/WhyCremisi` (Windows) o `~/Library/Application Support/WhyCremisi` (macOS).
- **Quando finisci una sessione di lavoro**, aggiorna `Documentazione/STATUS.md` e spunta gli step in `Documentazione/ROADMAP.md`. È l'unica documentazione che serve davvero mantenere viva.
