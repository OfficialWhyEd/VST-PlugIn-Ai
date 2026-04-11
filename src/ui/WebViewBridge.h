/*
   ==============================================================================
   WebViewBridge.h
   OpenClaw VST Bridge AI - Communication bridge between C++ and React UI
   ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>
#include <nlohmann/json.hpp>

class WebViewBridge
{
public:
    WebViewBridge();
    ~WebViewBridge();
    
    // Initialize the web view
    void initialize(juce::WebBrowserComponent* webView);
    void shutdown();
    
    // Send JSON message to React frontend
    void sendToFrontend(const nlohmann::json& message);
    
    // Set callback for messages from React frontend
    using FrontendMessageCallback = std::function<void(const nlohmann::json&)>;
    void setFrontendMessageCallback(FrontendMessageCallback callback);
    
    // Handle message from frontend (called by JavaScript)
    void handleMessageFromFrontend(const juce::String& jsonString);
    
private:
    juce::WebBrowserComponent* webView = nullptr;
    FrontendMessageCallback frontendMessageCallback;
    
    // Convert JSON to JavaScript call
    void evaluateJavaScript(const juce::String& jsCode);
};