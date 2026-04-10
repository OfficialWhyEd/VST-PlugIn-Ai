# 🤝 Workflow Git - Regole del Team

**Progetto:** OpenClaw VST Bridge AI  
**Team:** Carlo (Aura) + Edo (Heartbroken)  
**Ultimo aggiornamento:** 10 Aprile 2026

---

## 🎯 Obiettivo

Evitare conflitti e sovrascritture quando Carlo/Aura e Edo/Heartbroken lavorano sullo stesso repository.

---

## 📋 Regole Generali

### 1. Identificazione nei Commit

Ogni commit DEVE iniziare con il nome dell'AI:

```bash
# Per Heartbroken (AI di Edo)
git commit -m "Heartbroken: Aggiunto componente GainSlider"

# Per Aura (AI di Carlo)
git commit -m "AURA: Implementato OscHandler"
```

Questo permette di sapere immediatamente chi ha modificato cosa.

---

## 🔄 Procedura Prima del Push

### Passo 1: Verifica stato remoto
```bash
git log --oneline -5
# Guarda gli ultimi commit: ci sono modifiche di Aura?
```

### Passo 2: Se ci sono file modificati da Aura
**STOP** → Avvisa l'altro prima di procedere.

Comunica su WhatsApp/Telegram:
- "Ho visto che Aura ha modificato X, voglio modificare Y"
- Discuti eventuali conflitti

### Passo 3: Commit locale prima del pull
```bash
git add .
git commit -m "Heartbroken: descrizione delle tue modifiche"
```

**Perché:** Se il pull sovrascrive file, il commit locale è salvato nel log.

### Passo 4: Pull
```bash
git pull origin master
```

### Passo 5: Risolvi conflitti (se presenti)
```bash
# Se ci sono conflitti, Git li evidenzia
# Modifica i file, poi:
git add .
git commit -m "Heartbroken: Merge con master, risolti conflitti"
```

### Passo 6: Push
```bash
git push origin master
```

---

## 📁 Divisione Responsabilità

| Area | Responsabile | File Tipici |
|------|--------------|-------------|
| **Backend C++** | Aura | `src/core/`, `src/osc/`, `src/ai/` |
| **Frontend UI** | Heartbroken | `src/ui/`, componenti React |
| **Build System** | Aura | `CMakeLists.txt`, configurazioni |
| **Documentazione** | Entrambi | `Documentazione/*.md` |

**Nota:** Se Heartbroken deve toccare backend o Aura frontend, avvisa prima.

---

## 🆘 Troubleshooting

### "Ho fatto push e ho sovrascritto le modifiche di Aura!"

**Recupero:**
```bash
# Vedi la history
git log --all --oneline --graph

# Trova il commit precedente (prima del tuo push)
git show abc123  # sostituisci con l'hash

# Se necessario, revert
git revert abc123
git push origin master
```

### "Non riesco a fare pull, dice 'untracked files'"
```bash
# Soluzione 1: committa tutto
git add .
git commit -m "Heartbroken: WIP prima del merge"
git pull origin master

# Soluzione 2: stash (salva momentaneamente)
git stash
git pull origin master
git stash pop
```

---

## 💡 Best Practices

1. **Commit piccoli e frequenti** — Un commit = una funzionalità
2. **Descrivi nel commit** — Cosa hai fatto e perché
3. **Branch per feature** (opzionale ma consigliato):
   ```bash
   git checkout -b feature/nuova-ui
   # lavora qui
   git checkout master
   git merge feature/nuova-ui
   ```
4. **`git status` sempre** — Prima di ogni comando, controlla lo stato

---

## 📞 Canali di Comunicazione

- **Urgenti:** WhatsApp/Telegram (Carlo ↔ Edo)
- **Tracciamento:** GitHub commits e issues
- **Documentazione:** Questo file e `08-COORDINAMENTO-TEAM.md`

---

## ✅ Checklist Pre-Push

- [ ] Ho fatto `git status` e so cosa sto per committare
- [ ] Ho verificato gli ultimi commit (`git log -5`)
- [ ] Se ci sono commit di Aura, ho avvisato
- [ ] Ho committato le mie modifiche locali
- [ ] Ho fatto `git pull` senza errori
- [ ] Ho risolto eventuali conflitti
- [ ] Ho pushato con messaggio "Heartbroken: ..." o "AURA: ..."

---

**In caso di dubbio, chiedi prima di pushare.** 🚀