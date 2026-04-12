/*
  ==============================================================================
  PluginEditor.cpp
  OpenClaw VST Bridge AI - Plugin Editor with WebView UI
  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"

//==============================================================================
OpenClawAudioProcessorEditor::OpenClawAudioProcessorEditor(OpenClawAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size (standard VST3 size)
    setSize(800, 600);
    
    // Prova a caricare la WebView UI, altrimenti fallback
    loadUI();
    
    DBG("[PluginEditor] Editor creato, WebView: " + juce::String(webView != nullptr ? "SI" : "NO (fallback)"));
}

OpenClawAudioProcessorEditor::~OpenClawAudioProcessorEditor()
{
    // Cleanup
    if (webView)
    {
        webView->removeListener(this);
        webView.reset();
    }
    
    webViewBridge.shutdown();
    
    DBG("[PluginEditor] Editor distrutto");
}

//==============================================================================
void OpenClawAudioProcessorEditor::loadUI()
{
    // Controlla se esiste la build della UI React
    juce::File uiBuildDir = juce::File::getCurrentWorkingDirectory()
        .getChildFile("webview-ui")
        .getChildFile("dist");
    
    // Alternativa: cerca nella cartella del plugin
    juce::File pluginDir = juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getParentDirectory();
    juce::File altUiDir = pluginDir.getChildFile("webview-ui").getChildFile("dist");
    
    bool useWebView = uiBuildDir.exists() || altUiDir.exists();
    
    if (useWebView)
    {
        setupWebView();
    }
    else
    {
        setupFallbackUI();
    }
}

void OpenClawAudioProcessorEditor::setupWebView()
{
    // Crea WebBrowserComponent
    webView = std::make_unique<juce::WebBrowserComponent>();
    webView->addListener(this);
    addAndMakeVisible(*webView);
    
    // Inizializza il bridge
    webViewBridge.initialize(webView.get());
    
    // Setup callback per messaggi dal frontend
    webViewBridge.setFrontendMessageCallback([this](const nlohmann::json& msg) {
        handleFrontendMessage(msg);
    });
    
    // Carica la UI
    juce::String uiUrl = getUIURL();
    DBG("[PluginEditor] Caricamento UI da: " + uiUrl);
    webView->goToURL(uiUrl);
}

void OpenClawAudioProcessorEditor::setupFallbackUI()
{
    // UI nativa JUCE (quando React non è disponibile)
    
    // Title label
    titleLabel.setText("OpenClaw VST Bridge AI (Fallback UI)", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Gain Sliders
    gainSlider1.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    gainSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider1.setRange(-60.0, 12.0, 0.1);
    gainSlider1.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider1);
    
    gainLabel1.setText("Gain 1", juce::dontSendNotification);
    gainLabel1.setJustificationType(juce::Justification::centred);
    gainLabel1.attachToComponent(&gainSlider1, false);
    addAndMakeVisible(gainLabel1);
    
    gainSlider2.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    gainSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider2.setRange(-60.0, 12.0, 0.1);
    gainSlider2.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider2);
    
    gainLabel2.setText("Gain 2", juce::dontSendNotification);
    gainLabel2.setJustificationType(juce::Justification::centred);
    gainLabel2.attachToComponent(&gainSlider2, false);
    addAndMakeVisible(gainLabel2);
    
    // AI Button
    aiButton.setButtonText("Ask AI");
    aiButton.onClick = [this]() {
        audioProcessor.sendAiPrompt(aiPrompt.getText());
    };
    addAndMakeVisible(aiButton);
    
    // AI Prompt
    aiPrompt.setMultiLine(true);
    aiPrompt.setReturnKeyStartsNewLine(false);
    aiPrompt.setText("WebView non disponibile. Costruisci la UI React con: cd webview-ui && npm run build");
    addAndMakeVisible(aiPrompt);
    
    // Parameter attachments
    gainAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "gain1", gainSlider1);
    
    gainAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "gain2", gainSlider2);
}

juce::String OpenClawAudioProcessorEditor::getUIURL() const
{
    // Cerca la build della UI React
    juce::Array<juce::File> possiblePaths;
    
    // Path relativo al working directory (development)
    possiblePaths.add(juce::File::getCurrentWorkingDirectory()
        .getChildFile("webview-ui")
        .getChildFile("dist")
        .getChildFile("index.html"));
    
    // Path relativo all'eseguibile (production)
    possiblePaths.add(juce::File::getSpecialLocation(juce::File::currentApplicationFile)
        .getParentDirectory()
        .getChildFile("webview-ui")
        .getChildFile("dist")
        .getChildFile("index.html"));
    
    // Path assoluto (installazione)
    possiblePaths.add(juce::File("/usr/share/openclaw-vst/webview-ui/dist/index.html"));
    
    for (auto& path : possiblePaths)
    {
        if (path.existsAsFile())
        {
            DBG("[PluginEditor] UI trovata in: " + path.getFullPathName());
            return juce::URL(path).toString(false);
        }
    }
    
    // Fallback: development server
    DBG("[PluginEditor] UI build non trovata, uso localhost:5173");
    return "http://localhost:5173";
}

//==============================================================================
// WebBrowserComponent::Listener

bool OpenClawAudioProcessorEditor::pageAboutToLoad(const juce::String& url)
{
    DBG("[PluginEditor] pageAboutToLoad: " + url.substring(0, 100));
    
    // Passa al bridge per gestire messaggi app://
    if (webViewBridge.handleMessageFromURL(url))
    {
        // URL consumato dal bridge (era un messaggio)
        return false; // Blocca navigazione
    }
    
    // Altrimenti lascia caricare la pagina
    return true;
}

void OpenClawAudioProcessorEditor::pageFinishedLoading(const juce::String& url)
{
    DBG("[PluginEditor] Pagina caricata: " + url.substring(0, 100));
    
    // Inietta il bridge JavaScript nella pagina
    if (webView)
    {
        // Inietta script che crea window.__openclawBridge
        // Questo deve essere fatto prima che React inizializzi
        juce::String bridgeScript = R"(
            if (typeof window.__openclawBridge === 'undefined') {
                window.__openclawBridge = {
                    receiveMessage: function(jsonStr) {
                        try {
                            const msg = JSON.parse(jsonStr);
                            window.dispatchEvent(new CustomEvent('openclaw-message', { detail: msg }));
                        } catch (e) {
                            console.error('[OpenClaw] Failed to parse message:', e);
                        }
                    },
                    sendMessage: function(msg) {
                        const jsonStr = JSON.stringify(msg);
                        window.location.href = 'app://message/' + encodeURIComponent(jsonStr);
                    }
                };
                console.log('[OpenClaw] Bridge initialized');
            }
        )";
        
        // Non possiamo usare evaluateJavaScript (privato), quindi lo facciamo via goToURL
        juce::String jsCode = "javascript:" + bridgeScript;
        webView->goToURL(jsCode);
        
        // Notifica al frontend che il plugin è pronto
        webViewBridge.sendToFrontend({
            {"type", "plugin.init"},
            {"id", nullptr},
            {"timestamp", juce::Time::currentTimeMillis()},
            {"payload", {
                {"version", "1.0.0"},
                {"capabilities", {"widgets", "ai", "osc"}}
            }}
        });
    }
}

//==============================================================================
// Gestione messaggi dal frontend

void OpenClawAudioProcessorEditor::handleFrontendMessage(const nlohmann::json& message)
{
    // Log del messaggio ricevuto
    DBG("[PluginEditor] Messaggio dal frontend: " + juce::String(message.dump().data()));
    
    // Estrai il tipo
    if (!message.contains("type"))
    {
        DBG("[PluginEditor] ERRORE: messaggio senza 'type'");
        return;
    }
    
    std::string type = message["type"];
    juce::String msgType(type.data(), type.size());
    
    // Gestione per tipo
    if (msgType == "plugin.init")
    {
        // UI pronta, può ricevere dati
        DBG("[PluginEditor] UI React pronta");
        
        // Invia stato attuale
        webViewBridge.sendTransport(false, false, 120.0f, 0.0f);
    }
    else if (msgType == "daw.command")
    {
        // Comando per il DAW
        if (message.contains("payload") && message["payload"].contains("command"))
        {
            std::string cmd = message["payload"]["command"];
            DBG("[PluginEditor] Comando DAW: " + juce::String(cmd.data()));
            
            // TODO: Implementare comandi OSC verso DAW
            // audioProcessor.sendOscCommand(...);
        }
    }
    else if (msgType == "daw.request")
    {
        // Richiesta info dal DAW
        if (message.contains("payload") && message["payload"].contains("request"))
        {
            std::string req = message["payload"]["request"];
            DBG("[PluginEditor] Richiesta DAW: " + juce::String(req.data()));
            
            // Rispondi con i dati disponibili
            // TODO: Implementare risposte
        }
    }
    else if (msgType == "ai.prompt")
    {
        // Richiesta AI
        if (message.contains("payload") && message["payload"].contains("prompt"))
        {
            std::string prompt = message["payload"]["prompt"];
            std::string reqId = message.value("id", "");
            
            DBG("[PluginEditor] Prompt AI: " + juce::String(prompt.data()).substring(0, 50));
            
            // Invia al processor che gestisce l'AI
            audioProcessor.sendAiPrompt(juce::String(prompt.data()));
            
            // TODO: Rispondere con stream o risposta completa
            webViewBridge.sendAIResponse(juce::String(reqId.data()), 
                "AI non ancora implementata. Prompt ricevuto: " + juce::String(prompt.data()).substring(0, 100));
        }
    }
    else if (msgType == "widget.valueChange")
    {
        // Widget modificato dall'utente
        if (message.contains("payload"))
        {
            auto& payload = message["payload"];
            std::string widgetId = payload.value("widgetId", "");
            float value = payload.value("value", 0.0f);
            
            DBG("[PluginEditor] Widget " + juce::String(widgetId.data()) + " = " + juce::String(value));
            
            // TODO: Mappare a parametro DAW
        }
    }
    else if (msgType == "config.get" || msgType == "config.set")
    {
        // Gestione configurazione
        DBG("[PluginEditor] Config: " + msgType);
    }
    else
    {
        DBG("[PluginEditor] Tipo messaggio non gestito: " + msgType);
    }
}

void OpenClawAudioProcessorEditor::sendToFrontend(const nlohmann::json& message)
{
    webViewBridge.sendToFrontend(message);
}

//==============================================================================
void OpenClawAudioProcessorEditor::paint(juce::Graphics& g)
{
    if (!webView)
    {
        // Solo per fallback UI
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::black.withAlpha(0.3f));
        g.fillRect(0, 0, getWidth(), 40);
    }
}

void OpenClawAudioProcessorEditor::resized()
{
    if (webView)
    {
        // WebView occupa tutto lo spazio
        webView->setBounds(0, 0, getWidth(), getHeight());
    }
    else
    {
        // Layout fallback UI
        titleLabel.setBounds(0, 10, getWidth(), 30);
        
        int sliderWidth = 100;
        int sliderHeight = 100;
        int startX = 50;
        int startY = 80;
        
        gainSlider1.setBounds(startX, startY, sliderWidth, sliderHeight);
        gainSlider2.setBounds(startX + sliderWidth + 50, startY, sliderWidth, sliderHeight);
        
        int aiY = 250;
        aiPrompt.setBounds(50, aiY, getWidth() - 100, 100);
        aiButton.setBounds(getWidth() - 150, aiY + 110, 100, 30);
    }
}