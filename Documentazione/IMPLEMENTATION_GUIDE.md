# IMPLEMENTATION_GUIDE.md

**Per:** Edo (C++) e Heartbroken (React)  
**Scadenza:** 96 ore da ora  
**Obiettivo:** Plugin VST funzionante con OSC e UI dinamica

---

## Per Edo (C++)

### Task 1: Ricezione OSC (24h)

**File da modificare:** `src/osc/OscHandler.cpp`

**Cosa implementare:**
```cpp
// Aggiungi in OscHandler.h
class OscHandler : public juce::OSCReceiver,
                   public juce::OSCReceiver::Listener<juce::OSCReceiver::MessageLoopCallback>
{
public:
    void oscMessageReceived(const juce::OSCMessage& message) override;
    void oscBundleReceived(const juce::OSCBundle& bundle) override;
    
    // Callback per JavaScript
    std::function<void(const juce::String& jsonMessage)> onOscMessage;
};

// Implementazione in OscHandler.cpp
void OscHandler::oscMessageReceived(const juce::OSCMessage& message)
{
    juce::String address = message.getAddressPattern().toString();
    
    // Parse /track/*/volume, /track/*/pan, etc.
    if (address.startsWith("/track/"))
    {
        auto json = oscMessageToJson(message);
        if (onOscMessage)
            onOscMessage(json);
    }
}

juce::String OscHandler::oscMessageToJson(const juce::OSCMessage& message)
{
    juce::DynamicObject::Ptr obj = new juce::DynamicObject();
    obj->setProperty("message_type", "event");
    obj->setProperty("timestamp", juce::Time::getCurrentTime().toMilliseconds() / 1000.0);
    obj->setProperty("session_id", "plugin-session-001");
    
    auto source = new juce::DynamicObject();
    source->setProperty("daw", "reaper");
    source->setProperty("protocol", "osc");
    obj->setProperty("source", source.get());
    
    // Estrai track_id e param_name da /track/1/volume
    juce::String address = message.getAddressPattern().toString();
    // ... parsing ...
    
    return juce::JSON::toString(var(obj));
}
```

**Pattern OSC da supportare:**
- `/track/*/volume` (float 0.0-1.0)
- `/track/*/pan` (float -1.0 a 1.0)
- `/track/*/mute` (int 0/1)
- `/play`, `/stop`, `/record`

---

### Task 2: WebViewBridge JSON (48h)

**File da modificare:** `src/ui/WebViewBridge.cpp`

**Funzioni da implementare:**
```cpp
void WebViewBridge::sendJsonToJavaScript(const juce::String& jsonMessage)
{
    // Escape JSON per JavaScript
    juce::String escaped = jsonMessage.replace("\\", "\\\\")
                                      .replace("'", "\\'")
                                      .replace("\"", "\\\"");
    
    juce::String js = "window.receiveFromPlugin('" + escaped + "');";
    webView->evaluateJavascript(js);
}

void WebViewBridge::receiveJsonFromJavaScript(const juce::String& jsonMessage)
{
    auto json = juce::JSON::parse(jsonMessage);
    
    // Gestisci comandi AI
    auto msgType = json["message_type"].toString();
    if (msgType == "command")
    {
        handleAiCommand(json);
    }
}

void WebViewBridge::handleAiCommand(const juce::var& json)
{
    // Esegui azioni sul DAW via OSC
    auto action = json["action"];
    juce::String type = action["type"].toString();
    
    if (type == "set_parameter")
    {
        int trackId = action["target"]["track_id"];
        juce::String param = action["target"]["param_name"].toString();
        float value = action["value"];
        
        // Invia via OSC
        oscHandler->sendParameterChange(trackId, param, value);
    }
}
```

---

### Task 3: Collegare OscHandler ↔ WebViewBridge (72h)

**In PluginProcessor.cpp:**
```cpp
void PluginProcessor::setupCommunication()
{
    // Collega OSC a WebView
    oscHandler.onOscMessage = [this](const juce::String& json) {
        webViewBridge->sendJsonToJavaScript(json);
    };
}
```

---

## Per Heartbroken (React)

### Task 1: Setup Progetto (24h)

```bash
cd /path/to/VST-PlugIn-Ai
npx create-vite@latest webview-ui --template react
cd webview-ui
npm install
npm run dev
```

---

### Task 2: Componente Toolbox (48h)

**File:** `src/components/Toolbox.jsx`

```jsx
import { useState, useEffect } from 'react';
import { Slider, Knob, Button } from './widgets';

function Toolbox() {
  const [widgets, setWidgets] = useState([]);
  const [proposals, setProposals] = useState([]);

  useEffect(() => {
    // Connessione al plugin C++
    window.receiveFromPlugin = (jsonString) => {
      const message = JSON.parse(jsonString);
      
      if (message.message_type === 'event') {
        // Proponi widget all'utente
        const proposal = generateWidgetProposal(message);
        setProposals(prev => [...prev, proposal]);
      }
    };
  }, []);

  const acceptProposal = (proposal) => {
    setWidgets(prev => [...prev, proposal.widget_config]);
    setProposals(prev => prev.filter(p => p.id !== proposal.id));
    
    // Notifica C++
    sendToPlugin({
      message_type: 'command',
      toolbox_action: {
        type: 'add_widget',
        widget_id: proposal.id,
        accepted: true
      }
    });
  };

  return (
    <div className="toolbox">
      {/* Proposte in attesa */}
      {proposals.map(p => (
        <div key={p.id} className="proposal">
          <p>Aggiungere {p.widget_config.label}?</p>
          <button onClick={() => acceptProposal(p)}>Sì</button>
          <button onClick={() => rejectProposal(p)}>No</button>
        </div>
      ))}
      
      {/* Widget attivi */}
      {widgets.map(w => renderWidget(w))}
    </div>
  );
}

function renderWidget(widget) {
  switch (widget.type) {
    case 'slider':
      return <Slider key={widget.id} {...widget} />;
    case 'knob':
      return <Knob key={widget.id} {...widget} />;
    case 'button':
      return <Button key={widget.id} {...widget} />;
    default:
      return null;
  }
}
```

---

### Task 3: Widget Base (72h)

**Slider.jsx:**
```jsx
function Slider({ widget_id, label, min, max, default: defaultValue, mapping }) {
  const [value, setValue] = useState(defaultValue);

  const handleChange = (e) => {
    const newValue = parseFloat(e.target.value);
    setValue(newValue);
    
    // Invia al plugin
    sendToPlugin({
      message_type: 'command',
      action: {
        type: 'set_parameter',
        target: mapping,
        value: newValue
      }
    });
  };

  return (
    <div className="widget slider">
      <label>{label}</label>
      <input
        type="range"
        min={min}
        max={max}
        step="0.01"
        value={value}
        onChange={handleChange}
      />
      <span>{value.toFixed(2)}</span>
    </div>
  );
}
```

**Knob.jsx** e **Button.jsx** simili (usare libreria come `react-knob` o SVG).

---

## Checklist Completamento

### Edo (C++)
- [ ] OscHandler riceve `/track/*/volume`
- [ ] OscHandler riceve `/track/*/pan`
- [ ] WebViewBridge invia JSON a JavaScript
- [ ] WebViewBridge riceve JSON da JavaScript
- [ ] Comandi `set_parameter` funzionano
- [ ] Test in Ableton: plugin carica senza crash

### Heartbroken (React)
- [ ] `npm run dev` funziona
- [ ] Ricezione messaggi dal plugin
- [ ] Proposta widget appare
- [ ] Accetta/rifiuta funziona
- [ ] Slider invia valori al plugin
- [ ] UI responsive e stilizzata

---

## Mock Data per Test

**Per Edo:**
```cpp
// Nel costruttore, invia messaggi fake per test
juce::Timer::callAfterDelay(1000, [this]() {
    onOscMessage("{\"event\":{\"type\":\"parameter_change\",\"target\":{\"track_id\":1,\"param_name\":\"volume\"},\"value\":{\"raw\":0.75}}}");
});
```

**Per Heartbroken:**
```javascript
// In useEffect, simula messaggio dal plugin
setTimeout(() => {
  window.receiveFromPlugin(JSON.stringify({
    message_type: 'event',
    event: {
      type: 'parameter_change',
      target: { track_id: 1, param_name: 'volume' },
      value: { raw: 0.75 }
    }
  }));
}, 1000);
```

---

## Domande?

**Edo:** Se blocchi su JUCE OSC, cerca `juce::OSCReceiver` nel JUCE demo.  
**Heartbroken:** Se blocchi su WebSocket, usa `window.receiveFromPlugin` mockato.

---

**Deadline: 96 ore**  
**Consegna:** Commit su branch `feat/osc-webview` + PR verso `master`.
