/*
   ==============================================================================
   WebViewBridge.h
   OpenClaw VST Bridge AI - Communication bridge between C++ and React UI
   ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>

class WebViewBridge
{
public:
    WebViewBridge();
    ~WebViewBridge();
    
    // Initialize the web view with HTML content
    bool initialize(void* parentHandle, int width, int height);
    void shutdown();
    
    // Send message to React frontend
    void sendToFrontend(const juce::String& message);
    
    // Set callback for messages from React frontend
    using FrontendMessageCallback = std::function<void(const juce::String&)>;
    void setFrontendMessageCallback(FrontendMessageCallback callback);
    
    // Load a URL or HTML content
    void loadURL(const juce::String& url);
    void loadHTML(const juce::String& htmlContent, const juce::String& baseURL = {});
    
private:
    // Pointer to the web view component
    class WebViewComponent;
    std::unique_ptr<WebViewComponent> webViewComponent;
    
    // Callback for messages from frontend
    FrontendMessageCallback frontendMessageCallback;
};