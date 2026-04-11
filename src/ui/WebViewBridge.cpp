/*
   ==============================================================================
   WebViewBridge.cpp
   OpenClaw VST Bridge AI - Communication bridge between C++ and React UI
   ==============================================================================
*/

#include "WebViewBridge.h"
#include <juce_core/juce_core.h>

WebViewBridge::WebViewBridge() = default;
WebViewBridge::~WebViewBridge() = default;

void WebViewBridge::initialize(juce::WebBrowserComponent* webBrowser)
{
    webView = webBrowser;
}

void WebViewBridge::shutdown()
{
    webView = nullptr;
}

void WebViewBridge::sendToFrontend(const nlohmann::json& message)
{
    if (!webView)
        return;
    
    // Escape JSON for JavaScript
    juce::String jsonStr = message.dump();
    jsonStr = jsonStr.replace("\"", "\\\"").replace("\n", "\\n");
    
    // Use goToURL with javascript: scheme (works in JUCE)
    juce::String jsCode = "javascript:if(window.receiveFromBackend){window.receiveFromBackend(\"" + jsonStr + "\");}";
    webView->goToURL(jsCode);
}

void WebViewBridge::setFrontendMessageCallback(FrontendMessageCallback callback)
{
    frontendMessageCallback = std::move(callback);
}

void WebViewBridge::handleMessageFromFrontend(const juce::String& jsonString)
{
    if (frontendMessageCallback)
    {
        try
        {
            nlohmann::json message = nlohmann::json::parse(jsonString.toStdString());
            frontendMessageCallback(message);
        }
        catch (const std::exception& e)
        {
            juce::Logger::writeToLog("WebViewBridge: Failed to parse JSON: " + juce::String(e.what()));
        }
    }
}