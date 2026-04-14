# Organizzazione Build VST3 per Piattaforme

**Nota per Aura:** Creare tre cartelle separate per i build VST3 su Windows, Linux e macOS.

---

## Struttura Proposta

```
OpenClaw-VST-Bridge/
├── build/
│   ├── windows/
│   │   └── OpenClawVSTPlugin_artefacts/
│   │       └── Release/
│   │           └── VST3/
│   │               └── OpenClawVSTBridgeAI.vst3
│   ├── linux/
│   │   └── OpenClawVSTPlugin_artefacts/
│   │       └── Release/
│   │           └── VST3/
│   │               └── OpenClawVSTBridgeAI.vst3
│   └── macos/
│       └── OpenClawVSTPlugin_artefacts/
│           └── Release/
│               └── VST3/
│                   └── OpenClawVSTBridgeAI.vst3
│
├── releases/
│   ├── v1.0.0/
│   │   ├── OpenClawVSTBridgeAI-Windows-x64.vst3.zip
│   │   ├── OpenClawVSTBridgeAI-Linux-x64.vst3.tar.gz
│   │   └── OpenClawVSTBridgeAI-macOS-universal.vst3.zip
│   └── ...
│
└── ...
```

---

## Script di Build per CI/CD

### build-all.sh (Linux/macOS)

```bash
#!/bin/bash
# Build per tutte le piattaforme

VERSION=${1:-"1.0.0"}
BUILD_DIR="build"
RELEASE_DIR="releases/${VERSION}"

mkdir -p "${RELEASE_DIR}"

# Linux Build
echo "Building Linux..."
mkdir -p "${BUILD_DIR}/linux"
cd "${BUILD_DIR}/linux"
cmake ../.. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release -j$(nproc)
cd ../..

# Package Linux
tar -czf "${RELEASE_DIR}/OpenClawVSTBridgeAI-Linux-x64.vst3.tar.gz" \
  -C "${BUILD_DIR}/linux/OpenClawVSTPlugin_artefacts/Release/VST3" \
  OpenClawVSTBridgeAI.vst3

echo "Linux build complete!"
```

### build-windows.ps1 (Windows)

```powershell
# Build per Windows
$VERSION = "1.0.0"
$BUILD_DIR = "build\windows"
$RELEASE_DIR = "releases\$VERSION"

New-Item -ItemType Directory -Force -Path $RELEASE_DIR

# Windows Build
Write-Host "Building Windows..."
New-Item -ItemType Directory -Force -Path $BUILD_DIR
Set-Location $BUILD_DIR
cmake ..\.. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
Set-Location ..\..

# Package Windows
Compress-Archive -Path "${BUILD_DIR}\OpenClawVSTPlugin_artefacts\Release\VST3\OpenClawVSTBridgeAI.vst3" `
  -DestinationPath "${RELEASE_DIR}\OpenClawVSTBridgeAI-Windows-x64.vst3.zip"

Write-Host "Windows build complete!"
```

### build-macos.sh (macOS)

```bash
#!/bin/bash
# Build Universal Binary per macOS (Intel + Apple Silicon)

VERSION=${1:-"1.0.0"}
BUILD_DIR="build/macos"
RELEASE_DIR="releases/${VERSION}"

mkdir -p "${RELEASE_DIR}"

# Build Intel (x86_64)
echo "Building macOS Intel..."
mkdir -p "${BUILD_DIR}/intel"
cd "${BUILD_DIR}/intel"
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=x86_64
cmake --build . --config Release
cd ../..

# Build Apple Silicon (arm64)
echo "Building macOS Apple Silicon..."
mkdir -p "${BUILD_DIR}/arm64"
cd "${BUILD_DIR}/arm64"
cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build . --config Release
cd ../..

# Create Universal Binary
echo "Creating Universal Binary..."
lipo -create \
  "${BUILD_DIR}/intel/OpenClawVSTPlugin_artefacts/Release/VST3/OpenClawVSTBridgeAI.vst3/Contents/MacOS/OpenClawVSTBridgeAI" \
  "${BUILD_DIR}/arm64/OpenClawVSTPlugin_artefacts/Release/VST3/OpenClawVSTBridgeAI.vst3/Contents/MacOS/OpenClawVSTBridgeAI" \
  -output "${BUILD_DIR}/OpenClawVSTBridgeAI"

# Package macOS
zip -r "${RELEASE_DIR}/OpenClawVSTBridgeAI-macOS-universal.vst3.zip" \
  "${BUILD_DIR}/OpenClawVSTBridgeAI.vst3"

echo "macOS build complete!"
```

---

## Cartelle di Installazione per Utenti

| Piattaforma | Destinazione VST3 |
|-------------|-------------------|
| Windows | `C:\Program Files\Common Files\VST3\` |
| Linux | `~/.vst3/` o `/usr/lib/vst3/` |
| macOS | `~/Library/Audio/Plug-Ins/VST3/` |

---

## Note Implementazione

1. **Windows:** Richiede Visual Studio 2022 e Windows SDK
2. **Linux:** Richiede GCC/Clang, libwebkit2gtk (se GUI nativa)
3. **macOS:** Richiede Xcode, macOS SDK 11.0+ (per Universal Binary)

---

**TODO:**
- [ ] Creare script CI/CD (GitHub Actions)
- [ ] Configurare build automatici su push
- [ ] Creare release automatiche con changelog
- [ ] Firmare binari Windows (code signing)
- [ ] Notarizzare per macOS

---

*Creato da Carlo per Aura - Aprile 2026*
