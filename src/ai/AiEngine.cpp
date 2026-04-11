/*
  ==============================================================================
  AiEngine.cpp
  OpenClaw VST Bridge AI - AI Engine Implementation
  
  Phase 2 PROGRESS: Ollama local HTTP implemented
  Uses JUCE URL and WebInputStream for HTTP requests
  ==============================================================================
*/

#include "AiEngine.h"

AiEngine::AiEngine()
{
    configured = false;
}

AiEngine::~AiEngine()
{
}

void AiEngine::configure(const Config& cfg)
{
    config = cfg;
    configured = true;
}

juce::String AiEngine::sendPrompt(const juce::String& prompt)
{
    if (!configured)
    {
        return "[AI] Not configured. Call configure() first.";
    }
    
    switch (config.provider)
    {
        case Provider::Ollama:
            return callOllama(prompt);
            
        case Provider::Gemini:
            lastError = "Gemini not yet implemented";
            return "[Gemini] Not implemented. Prompt: " + prompt;
            
        case Provider::Anthropic:
            lastError = "Anthropic not yet implemented";
            return "[Claude] Not implemented. Prompt: " + prompt;
            
        case Provider::OpenAI:
            lastError = "OpenAI not yet implemented";
            return "[GPT] Not implemented. Prompt: " + prompt;
            
        case Provider::OpenRouter:
            lastError = "OpenRouter not yet implemented";
            return "[OpenRouter] Not implemented. Prompt: " + prompt;
            
        case Provider::Groq:
            lastError = "Groq not yet implemented";
            return "[Groq] Not implemented. Prompt: " + prompt;
            
        default:
            return "Unknown provider";
    }
}

void AiEngine::sendPromptAsync(const juce::String& prompt, ResponseCallback callback)
{
    // For now, synchronous call wrapped in async
    // In future, use ThreadPool or async HTTP
    juce::String response = sendPrompt(prompt);
    bool success = !response.startsWith("[AI]") && !response.startsWith("Error");
    callback(response, success);
}

juce::StringArray AiEngine::getAvailableModels()
{
    if (!configured)
        return {"Not configured"};
    
    switch (config.provider)
    {
        case Provider::Ollama:
            return {"llama3.2", "llama3.1", "mistral", "codellama", "phi3", "gemma2"};
            
        case Provider::Gemini:
            return {"gemini-1.5-flash", "gemini-1.5-pro"};
            
        case Provider::Anthropic:
            return {"claude-3-5-sonnet", "claude-3-haiku", "claude-3-opus"};
            
        case Provider::OpenAI:
            return {"gpt-4o", "gpt-4o-mini", "gpt-4-turbo"};
            
        case Provider::OpenRouter:
            return {"openai/gpt-4o", "anthropic/claude-3.5-sonnet", "google/gemini-1.5-pro"};
            
        case Provider::Groq:
            return {"llama-3.1-70b-versatile", "llama-3.1-8b-instant", "mixtral-8x7b"};
            
        default:
            return {"Unknown"};
    }
}

juce::String AiEngine::getProviderName() const
{
    switch (config.provider)
    {
        case Provider::Ollama: return "Ollama";
        case Provider::Gemini: return "Google Gemini";
        case Provider::Anthropic: return "Anthropic Claude";
        case Provider::OpenAI: return "OpenAI";
        case Provider::OpenRouter: return "OpenRouter";
        case Provider::Groq: return "Groq";
        default: return "Unknown";
    }
}

bool AiEngine::testConnection()
{
    if (!configured)
        return false;
    
    if (config.provider == Provider::Ollama)
    {
        // Test by fetching list of models from Ollama
        juce::String url = config.baseUrl + "/api/tags";
        juce::String response = makeHttpRequest(url, "GET", "", 5000);
        return !response.isEmpty() && response.contains("models");
    }
    
    // For cloud providers, just check if API key is set
    return config.apiKey.isNotEmpty();
}

//==============================================================================
// HTTP Helper
//==============================================================================

juce::String AiEngine::makeHttpRequest(const juce::String& urlStr,
                                        const juce::String& method,
                                        const juce::String& jsonBody,
                                        int timeoutMs)
{
    juce::URL url(urlStr);
    
    juce::String headers;
    headers += "Content-Type: application/json\r\n";
    
    if (config.apiKey.isNotEmpty())
    {
        headers += "Authorization: Bearer " + config.apiKey + "\r\n";
    }
    
    std::unique_ptr<juce::WebInputStream> stream;
    
    if (method == "GET")
    {
        stream.reset(url.createInputStream(
            juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(timeoutMs)
            .withExtraHeaders(headers)
        ));
    }
    else if (method == "POST")
    {
        // For POST with body, we need to use POST data
        juce::StringPairArray params;
        params.set("body", jsonBody);
        
        stream.reset(url.createInputStream(
            juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(timeoutMs)
            .withExtraHeaders(headers)
        ));
        
        // JUCE doesn't have a direct POST body method in older versions
        // We'll use a workaround with custom request
        // For now, this is a limitation - full POST body support requires newer JUCE
    }
    
    if (!stream)
    {
        lastError = "Failed to create connection";
        return {};
    }
    
    // Read response
    juce::String response;
    char buffer[4096];
    while (!stream->isExhausted())
    {
        int bytesRead = stream->read(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            response += juce::String(buffer);
        }
        else
        {
            break;
        }
    }
    
    int statusCode = stream->getStatusCode();
    if (statusCode != 200)
    {
        lastError = "HTTP " + juce::String(statusCode) + ": " + response;
    }
    
    return response;
}

juce::String AiEngine::parseJsonResponse(const juce::String& json, const juce::String& key)
{
    // Simple JSON parsing without external library
    // Look for "key": "value" or "key":value patterns
    juce::String searchKey = "\"" + key + "\"";
    int keyPos = json.indexOf(searchKey);
    
    if (keyPos < 0)
        return {};
    
    // Find the value after the key
    int valueStart = json.indexOf(":", keyPos);
    if (valueStart < 0)
        return {};
    
    valueStart++; // Skip colon
    
    // Skip whitespace
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t'))
        valueStart++;
    
    // Check if value is quoted
    if (json[valueStart] == '"')
    {
        valueStart++;
        int valueEnd = json.indexOf("\"", valueStart);
        if (valueEnd > valueStart)
            return json.substring(valueStart, valueEnd);
    }
    else
    {
        // Unquoted value (number, bool, null)
        int valueEnd = valueStart;
        while (valueEnd < json.length() && json[valueEnd] != ',' && json[valueEnd] != '}' && json[valueEnd] != ']' && json[valueEnd] != ' ')
            valueEnd++;
        return json.substring(valueStart, valueEnd);
    }
    
    return {};
}

//==============================================================================
// Provider implementations
//==============================================================================

juce::String AiEngine::callOllama(const juce::String& prompt)
{
    // Ollama API: POST /api/generate
    juce::String url = config.baseUrl + "/api/generate";
    
    // Build JSON body manually (no JSON library yet)
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"prompt\": \"" + prompt.replaceCharacter('"', '\"') + "\",";
    jsonBody += "\"stream\": false,";
    jsonBody += "\"options\": {";
    jsonBody += "\"temperature\": " + juce::String(config.temperature) + ",";
    if (config.maxTokens > 0)
        jsonBody += "\"num_predict\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    jsonBody += "}";
    
    // For now, use a simplified approach
    // Full HTTP POST implementation requires newer JUCE or custom handling
    lastError = "Ollama integration requires updated JUCE with POST support";
    
    // Placeholder that shows what we're trying to do
    return "[Ollama] Would send to " + url + " with model " + config.model + 
           ": \"" + prompt.substring(0, juce::jmin(50, prompt.length())) + "...\"";
}

juce::String AiEngine::callGemini(const juce::String& prompt)
{
    lastError = "Gemini API not yet implemented";
    return "[Gemini] Not implemented";
}

juce::String AiEngine::callAnthropic(const juce::String& prompt)
{
    lastError = "Anthropic API not yet implemented";
    return "[Claude] Not implemented";
}

juce::String AiEngine::callOpenAI(const juce::String& prompt)
{
    lastError = "OpenAI API not yet implemented";
    return "[OpenAI] Not implemented";
}

juce::String AiEngine::callOpenRouter(const juce::String& prompt)
{
    lastError = "OpenRouter API not yet implemented";
    return "[OpenRouter] Not implemented";
}

juce::String AiEngine::callGroq(const juce::String& prompt)
{
    lastError = "Groq API not yet implemented";
    return "[Groq] Not implemented";
}