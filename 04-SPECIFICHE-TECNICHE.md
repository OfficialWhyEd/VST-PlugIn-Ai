# 04-SPECIFICHE TECNICHE - Architettura e Protocolli

**Documento:** Scelte tecnologiche, architettura, dettagli protocolli  
**Target:** Team di sviluppo (tecnico)  
**Stato:** Draft - richiede review Edo/Carlo

---

## 🎯 Decisioni Architetturali Critiche

### 1. Linguaggio di Programmazione: C++ (JUCE)

**Opzioni Valutate:**

| Linguaggio | Pro | Contro | Verdetto |
|------------|-----|--------|----------|
| **C++ + JUCE** | Standard industria, 1000+ plugin, performance max | Steep learning curve, memory unsafe | ✅ **SCELTO** |
| **Rust + baseplug** | Memory safe, modern, no crashes | Ecosistema VST immaturo, poca documentazione | ❌ Rischioso per MVP |
| **C# + NAudio** | Semplice, .NET tooling | Solo Windows/macOS, performance audio non ottimale | ❌ Non cross-platform audio |
| **JavaScript (Node.js)** | Veloce sviluppo, familiarità | Latenza audio inaccettabile, non è plugin VST | ❌ Non fattibile |

**Motivazione C++:**
- JUCE è lo standard de-facto (Ableton, Arturia, FabFilter lo usano)
- Performance audio garantita (< 1ms processing latency)
- Supporto completo VST3/AU/AAX
- Ecosistema maturo (forum, training, codebase esistenti)
- Può caricare WebView per UI (React + Tauri)

**Setup JUCE:**
```cpp
// PluginProcessor.h
class OpenClawAudioProcessor : public juce::AudioProcessor
{
public:
    OpenClawAudioProcessor();
    ~OpenClawAudioProcessor() override;
    
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    
private:
    // OSC handler
    std::unique_ptr<OscHandler> oscHandler;
    
    // AI engine
    std::unique_ptr<AiEngine> aiEngine;
    
    // Parameters
    juce::AudioProcessorValueTreeState parameters;
};
```

---

### 2. UI Framework: React + Tauri (via WebView)

**Stack UI:**
```
┌─────────────────────────────────────┐
│  Plugin VST3 (C++/JUCE)             │
│  └─ WebView (Chromium)              │
│     └─ React App                    │
│        ├─ State: Zustand            │
│        ├─ UI: Tailwind CSS          │
│        └─ Animations: Framer Motion │
└─────────────────────────────────────┘
```

**Perché non JUCE nativo:**
- JUCE UI è C++ → sviluppo lento, complesso
- React → sviluppo rapido, hot reload, componenti riutilizzabili
- Team può lavorare su UI senza conoscere C++

**Build System:**
```bash
# Build C++ core
juce::build --target vst3 --config Release

# Build React UI
cd gui && npm run build

# Bundle together
./scripts/bundle.sh
```

**Comunicazione C++ ↔ React:**
```cpp
// C++ side
void sendToWebView(const juce::var& data)
{
    if (auto* editor = dynamic_cast<OpenClawEditor*>(getActiveEditor()))
    {
        editor->sendMessageToWebView(data);
    }
}

// JS side (React)
window.addEventListener('message', (event) => {
    const data = JSON.parse(event.data);
    // Update React state
});
```

---

### 3. AI Engine: Ollama Local + Fallback Cloud

**Architettura:**
```
User input
    ↓
[AI Router]
    ↓
┌──────────────┬──────────────┬──────────────┐
│ Ollama Local │  Gemini API  │  OpenAI API  │
│ (default)    │  (fallback)  │  (backup)    │
└──────────────┴──────────────┴──────────────┘
    ↓
[Memory Layer]
    ↓
Response to user
```

**Modelli Consigliati:**
- **Primario:** `llama3.1:8b` (4-bit) - 5GB RAM, 200ms response
- **Fallback:** `gemini-1.5-flash` - cloud, sub-second
- **Backup:** `gpt-4o-mini` - cloud, fallback

**System Prompt:**
```
You are an AI audio assistant. The user is using Ableton Live.
Current parameters:
- Parameter 1: {name}, value: {value}, range: {min}-{max}
- Parameter 2: ...

Rules:
1. Be concise (max 2 sentences)
2. Never suggest dangerous values (gain > +6dB, ratio > 10:1)
3. Ask for confirmation before applying changes
4. Explain your reasoning

User question: {user_input}
```

**Memory Context:**
```json
{
  "session_id": "uuid",
  "conversation": [...],
  "parameters_history": [
    {"param": "gain", "from": -12, "to": -6, "by": "user", "at": 1234567890}
  ],
  "user_patterns": [
    {"pattern": "gain_boost_before_eq", "frequency": 0.7}
  ]
}
```

---

## 🔌 Protocollo OSC (Open Sound Control)

### Overview
OSC è protocollo UDP-based per controllo audio. Ableton Live supporta OSC via Max4Live (non nativo).

### Setup Ableton Live
1. Install Max4Live (incluso in Live Suite)
2. Caricare device "OSC Send" e "OSC Receive"
3. Configurare porta: 9000 (default)

### OSC Address Pattern Standard

#### Track Controls
```
/track/1/volume f 0.75      # Track 1 volume (0.0 to 1.0)
/track/1/pan i 64            # Track 1 pan (0-127)
/track/1/mute b 1            # Track 1 mute (0/1)
/track/1/solo b 0            # Track 1 solo
/track/1/arm b 1             # Track 1 record arm
```

#### Device Parameters
```
/track/1/device/1/param/1 f 0.5   # Device 1, param 1
/track/1/device/1/bypass b 0      # Bypass device
```

#### Transport
```
/transport/play t              # Play
/transport/stop t              # Stop
/transport/record t            # Record
/transport/position f 120.5    # Position in seconds
/tempo f 128.0                 # BPM
```

#### Custom Messages (per OpenClaw)
```
/openclaw/plugin/name s "FabFilter Pro-Q 3"
/openclaw/plugin/parameters i 24
/openclaw/plugin/param/1/name s "Frequency"
/openclaw/plugin/param/1/value f 1000.0
/openclaw/plugin/param/1/min f 20.0
/openclaw/plugin/param/1/max f 20000.0
```

### Implementazione C++

```cpp
class OscHandler
{
public:
    OscHandler(int port = 9000)
    {
        socket.bindTo(port);
        socket.addListener(this);
    }
    
    void sendMessage(const juce::String& address, const juce::var& value)
    {
        osc::OutboundPacketStream p(buffer, 1024);
        p << osc::BeginMessage(address.toRawUTF8());
        
        if (value.isFloat())
            p << value.toString().getFloatValue();
        else if (value.isInt())
            p << (int)value;
        else if (value.isString())
            p << value.toString().toRawUTF8();
        
        p << osc::EndMessage;
        socket.send(p.Data(), p.Size());
    }
    
    void oscMessageReceived(const osc::ReceivedMessage& message) override
    {
        juce::String address = message.AddressPattern();
        
        // Parse parameters
        for (auto it = message.ArgumentsBegin(); it != message.ArgumentsEnd(); ++it)
        {
            if (it->IsFloat())
                handleParamChange(address, it->AsFloat());
            else if (it->IsInt32())
                handleParamChange(address, (float)it->AsInt32());
            else if (it->IsString())
                handleParamName(address, it->AsString());
        }
    }
    
private:
    UdpTransmitSocket socket;
    char buffer[1024];
};
```

---

## 🎵 Protocollo MIDI (Fallback)

### Why MIDI?
- Supporto universale in tutti i DAW
- Fallback se OSC non disponibile
- Compatibilità con hardware controllers

### Limitazioni
- Solo 128 CC (Control Change) messages
- Risoluzione 7-bit (0-127) vs OSC float
- No string messages
- No custom addressing

### MIDI CC Standard

| CC # | Name | Use Case |
|------|------|----------|
| 1 | Modulation Wheel | General purpose |
| 7 | Volume | Track volume |
| 10 | Pan | Stereo position |
| 11 | Expression | Dynamics |
| 64 | Sustain Pedal | On/off |
| 74 | Brightness | Filter cutoff |

### Mappatura OpenClaw MIDI

```cpp
// MIDI CC → OSC Address translation
const std::map<int, juce::String> midiToOsc = {
    {1, "/track/1/volume"},
    {2, "/track/1/pan"},
    {3, "/track/1/device/1/param/1"},
    // ... mapping configurabile
};

void handleMidiCC(int ccNumber, int value)
{
    float normalizedValue = value / 127.0f;
    juce::String oscAddress = midiToOsc[ccNumber];
    
    // Forward to OSC handler
    oscHandler.sendMessage(oscAddress, normalizedValue);
    
    // Update UI
    sendToWebView({
        {"type", "param_change"},
        {"address", oscAddress},
        {"value", normalizedValue},
        {"source", "midi"}
    });
}
```

---

## 🎛️ VST3 Parameters (Primario)

### Vantaggio VST3
- **Native integration** - DAW vede i parametri automaticamente
- **No setup OSC/MIDI** - funziona out-of-the-box
- **Automation** - DAW può automatizzare i parametri
- **Preset management** - DAW gestisce preset

### Implementazione

```cpp
class OpenClawAudioProcessor : public juce::AudioProcessor
{
public:
    OpenClawAudioProcessor()
    {
        // Define parameters
        parameters.createAndAddParameter(
            std::make_unique<juce::AudioParameterFloat>(
                "gain",           // parameter ID
                "Gain",           // name
                -60.0f, 12.0f, 0.0f, // range
                "dB"              // unit
            )
        );
        
        parameters.createAndAddParameter(
            std::make_unique<juce::AudioParameterFloat>(
                "frequency",
                "Frequency",
                20.0f, 20000.0f, 1000.0f,
                "Hz"
            )
        );
        
        parameters.createAndAddParameter(
            std::make_unique<juce::AudioParameterBool>(
                "bypass",
                "Bypass",
                false
            )
        );
    }
    
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        auto gainParam = parameters.getRawParameterValue("gain");
        float gain = Decibels::decibelsToGain(gainParam->load());
        
        // Apply gain
        buffer.applyGain(gain);
    }
};
```

### Comunicazione VST3 → OSC

Quando DAW modifica parametro VST3, invia OSC per sync:

```cpp
void parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::String oscAddress = "/openclaw/plugin/" + parameterID;
    oscHandler.sendMessage(oscAddress, newValue);
    
    // Update React UI
    sendToWebView({
        {"type", "param_change"},
        {"address", oscAddress},
        {"value", newValue},
        {"source", "vst3"}
    });
}
```

---

## 🏗️ Struttura Progetto (C++)

```
src/
├── core/
│   ├── PluginProcessor.cpp/h       # Main VST3 plugin
│   ├── PluginEditor.cpp/h          # GUI wrapper
│   └── OpenClawEngine.cpp/h        # Main engine
├── osc/
│   ├── OscHandler.cpp/h            # OSC send/receive
│   ├── OscMessage.cpp/h            # Message parsing
│   └── OscAddress.cpp/h            # Address pattern matching
├── midi/
│   ├── MidiHandler.cpp/h           # MIDI CC handling
│   └── MidiMapping.cpp/h           # CC → OSC mapping
├── ai/
│   ├── AiEngine.cpp/h              # AI orchestration
│   ├── OllamaClient.cpp/h          # Ollama API
│   ├── GeminiClient.cpp/h          # Gemini API
│   └── MemoryStore.cpp/h           # Conversation memory
├── ui/
│   └── WebViewBridge.cpp/h         # C++ ↔ React bridge
└── utils/
    ├── Logger.cpp/h
    └── Config.cpp/h
```

---

## 📦 Build System

### CMake (Primary)
```cmake
cmake_minimum_required(VERSION 3.20)
project(OpenClaw-VST-Bridge-AI)

# JUCE
add_subdirectory(JUCE)

# Plugin
juce_add_plugin(OpenClawVSTBridgeAI
    COMPANY_NAME "OpenClaw"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    PLUGIN_MANUFACTURER_CODE OpCl
    PLUGIN_CODE OcAI
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "OpenClaw VST Bridge AI"
)

target_sources(OpenClawVSTBridgeAI
    PRIVATE
        src/core/PluginProcessor.cpp
        src/core/PluginEditor.cpp
        src/osc/OscHandler.cpp
        src/ai/AiEngine.cpp
)

target_link_libraries(OpenClawVSTBridgeAI
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        oscpack  # OSC library
        nlohmann_json  # JSON for AI comms
)
```

### Build Script
```bash
#!/bin/bash
# build.sh

# Build C++ core
cmake -B build
cmake --build build --config Release

# Build React UI
cd gui && npm run build && cd ..

# Bundle
./scripts/bundle_vst.sh
```

---

## 📊 Performance Targets

| Metric | Target | Test |
|--------|--------|------|
| CPU overhead (idle) | < 1% | Task Manager |
| CPU overhead (active) | < 5% | PluginDoctor |
| Latency OSC | < 5ms | Loopback test |
| Latency AI response | < 200ms | Local benchmark |
| Memory usage | < 100MB | Process monitor |
| Load time | < 500ms | Stopwatch |

---

## 🔒 Security Considerations

### AI Safety
- Rate limiting: max 1 request/sec per param
- Value clamping: -60dB to +12dB per gain
- Confirmation required: AI non applica > ±3dB senza conferma
- Timeout: 10 secondi max per AI response

### OSC Security
- Bind solo a localhost (127.0.0.1)
- No OSC messages da network esterno
- Validate tutti i messaggi OSC (no buffer overflow)

---

*Questo documento richiede review tecnica da Edo prima di implementazione.*
