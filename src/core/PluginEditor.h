/*
  ==============================================================================
  PluginEditor.h
  OpenClaw VST Bridge AI - Plugin Editor with WebView UI
  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../ui/WebViewBridge.h"

// Forward declaration
class OpenClawAudioProcessor;

//==============================================================================
// Custom WebBrowserComponent che espone i listener methods
class OpenClawWebBrowserComponent : public juce::WebBrowserComponent
{
public:
    OpenClawWebBrowserComponent();
    
    // Callback che può essere impostata dall'esterno
    std::function<bool(const juce::String&)> onPageAboutToLoad;
    std::function<void(const juce::String&)> onPageFinishedLoading;
    
protected:
    // Override dei metodi protected di WebBrowserComponent
    bool pageAboutToLoad(const juce::String& url) override;
    void pageFinishedLoading(const juce::String& url) override;
};

//==============================================================================
class OpenClawAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OpenClawAudioProcessorEditor(OpenClawAudioProcessor&);
    ~OpenClawAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    // Gestione messaggi dal frontend
    void handleFrontendMessage(const nlohmann::json& message);
    
    // Invia messaggi al frontend
    void sendToFrontend(const nlohmann::json& message);

private:
    // Reference al processor
    OpenClawAudioProcessor& audioProcessor;
    
    // WebView Bridge per comunicazione C++ <-> JS
    WebViewBridge webViewBridge;
    
    // UI Components
    std::unique_ptr<OpenClawWebBrowserComponent> webView;
    
    // Fallback UI (quando WebView non disponibile)
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Slider gainSlider1;
    juce::Slider gainSlider2;
    juce::Label gainLabel1;
    juce::Label gainLabel2;
    juce::TextButton aiButton;
    juce::TextEditor aiPrompt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment1;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment2;
    
    // Carica la UI (WebView o fallback)
    void loadUI();
    void setupFallbackUI();
    void setupWebView();
    
    // URL della UI React (in development o built)
    juce::String getUIURL() const;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenClawAudioProcessorEditor)
};