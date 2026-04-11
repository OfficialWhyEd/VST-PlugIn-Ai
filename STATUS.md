# OpenClaw VST Bridge AI - Stato Progetto

**Ultimo aggiornamento:** 2026-04-12 00:41

---

## ✅ Completato (Linux)

| Componente | Stato | Note |
|------------|-------|------|
| Build VST3 | ✅ | Compilato con successo |
| Build Standalone | ✅ | Compilato con successo |
| OscHandler | ✅ | Bidirezionale (send/receive) |
| AiEngine | ⚠️ | Struttura base HTTP, POST cURL non implementato |
| WebViewBridge | ✅ | Header + implementation creati |
| CMakeLists.txt | ✅ | Configurato (cURL, JUCE, Linux) |
| Git | ✅ | Pushato su master |

---

## 🔄 In Corso (Windows - Edo)

**Setup completato:**
- Repo clonato in `C:\Users\Whyed\Desktop\VST-PlugIn-Ai`
- JUCE installato in `C:\Users\Whyed\Desktop\CARTELLE\JUCE`
- vcpkg in `C:\Users\Whyed\vcpkg` con curl:x64-windows e nlohmann-json:x64-windows

**Ultimo fix:**
- Corretto include JUCE in `WebViewBridge.h` (da `<JuceHeader.h>` a `<juce_gui_extra/juce_gui_extra.h>`)
- Commit: `be777b8`

**Prossimo passo per Edo:**
```powershell
cd C:\Users\Whyed\Desktop\VST-PlugIn-Ai
git pull origin master
rmdir /s /q build
cmake -B build -DJUCE_ROOT=C:/Users/Whyed/Desktop/CARTELLE/JUCE -DCMAKE_TOOLCHAIN_FILE=C:/Users/Whyed/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

---

## ❌ Mancante

1. **Build Windows** - Edo sta compilando
2. **Test VST3 in DAW** - Carlo (Reaper) e Edo (Ableton)
3. **Comunicazione WebView** - Protocollo C++ ↔ JavaScript
4. **UI React** - Da creare (Heartbroken)
5. **AiEngine POST** - Implementare cURL reale per Ollama

---

## Prossimi Passi

| # | Task | Chi | Priorità |
|---|------|-----|----------|
| 1 | Completare build Windows | Edo | Alta |
| 2 | Testare VST3 in Reaper | Carlo | Alta |
| 3 | Definire protocollo JSON C++↔JS | Aura | Alta |
| 4 | Implementare POST cURL in AiEngine | Aura | Media |
| 5 | Creare progetto React base | Heartbroken | Media |
| 6 | Testare OSC bidirezionale | Carlo/Edo | Media |

---

## Commits Recenti

- `be777b8` - AURA: Fix include JUCE per Windows
- `4b8ff9a` - AURA: Rimuovi evaluateJavaScript privato, usa goToURL
- `6321e7e` - AURA: WebViewBridge completo con nlohmann-json
- `ced6e52` - AURA: Fix include path per src/ui/

---

## Note Importanti

- **Percorso JUCE Edo:** `C:\Users\Whyed\Desktop\CARTELLE\JUCE`
- **Percorso vcpkg Edo:** `C:\Users\Whyed\vcpkg`
- **Repo GitHub:** https://github.com/OfficialWhyEd/VST-PlugIn-Ai
- **Branch attivo:** master

---

*File creato per tracciare lo stato tra sessioni.*
