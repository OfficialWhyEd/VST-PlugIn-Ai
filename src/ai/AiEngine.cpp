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

void AiEngine::updateConfig(std::function<void(Config&)> updater)
{
    updater(config);
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
            return callGemini(prompt);
            
        case Provider::Anthropic:
            return callAnthropic(prompt);
            
        case Provider::OpenAI:
            return callOpenAI(prompt);
            
        case Provider::OpenRouter:
            return callOpenRouter(prompt);
            
        case Provider::Groq:
            return callGroq(prompt);
            
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
    
    std::unique_ptr<juce::InputStream> stream;
    
    if (method == "GET")
    {
        stream = url.createInputStream(
            juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(timeoutMs)
            .withExtraHeaders(headers)
        );
    }
    else if (method == "POST")
    {
        // POST with body - use withPOSTData for older JUCE versions
        stream = url.withPOSTData(jsonBody.toRawUTF8()).createInputStream(
            juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(timeoutMs)
            .withExtraHeaders(headers)
        );
    }
    
    // Read response
    juce::String response;
    char buffer[4096];
    while (stream && !stream->isExhausted())
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
    
    if (!stream)
    {
        lastError = "Failed to create connection";
        return {};
    }
    
    // Get HTTP status code if available
    // Note: JUCE doesn't expose status code directly through base InputStream
    // We check if response is empty or contains error indicators
    if (response.isEmpty() && method == "POST")
    {
        // Could be a connection error or empty response
        // For POST, empty response might still be valid for some APIs
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
    int colonPos = json.indexOf(keyPos, ":");
    if (colonPos < 0)
        return {};
    
    int valueStart = colonPos + 1; // Skip colon
    
    // Skip whitespace
    while (valueStart < json.length() && (json[valueStart] == ' ' || json[valueStart] == '\t'))
        valueStart++;
    
    // Check if value is quoted
    if (json[valueStart] == '"')
    {
        valueStart++;
        int valueEnd = json.indexOf(valueStart, "\"");
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
// JSON Escape Helper
//==============================================================================

static juce::String escapeJsonString(const juce::String& input)
{
    juce::String result = input;
    result = result.replace("\\", "\\\\");
    result = result.replace("\"", "\\\"");
    result = result.replace("\n", "\\n");
    result = result.replace("\r", "\\r");
    result = result.replace("\t", "\\t");
    return result;
}

//==============================================================================
// Provider implementations
//==============================================================================

juce::String AiEngine::callOllama(const juce::String& prompt)
{
    // Ollama API: POST /api/generate
    // Also supports /api/chat for conversation history
    juce::String url = config.baseUrl + "/api/generate";
    
    // Escape JSON string properly
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    // Build JSON body manually
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"prompt\": \"" + escapedPrompt + "\",";
    jsonBody += "\"stream\": false,";
    jsonBody += "\"options\": {";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    if (config.maxTokens > 0)
        jsonBody += ", \"num_predict\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    jsonBody += "}";
    
    // Make HTTP POST request
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from Ollama. Is it running?";
        return "[ERROR] " + lastError;
    }
    
    // Parse response - Ollama returns: {"response": "...", "done": true}
    juce::String aiResponse = parseJsonResponse(response, "response");
    
    if (aiResponse.isEmpty())
    {
        // Check for error in response
        if (response.contains("error"))
        {
            juce::String errorMsg = parseJsonResponse(response, "error");
            lastError = "Ollama error: " + errorMsg;
            return "[ERROR] " + lastError;
        }
        // Try alternative parsing - response might be in different format
        // Some Ollama versions return response directly in body
        return "[Ollama] Response received but could not parse: " + response.substring(0, 200);
    }
    
    return aiResponse;
}

juce::String AiEngine::callGemini(const juce::String& prompt)
{
    if (config.apiKey.isEmpty())
    {
        lastError = "Gemini API key not configured";
        return "[ERROR] " + lastError;
    }
    
    // Google Gemini API: POST https://generativelanguage.googleapis.com/v1beta/models/{model}:generateContent
    juce::String url = "https://generativelanguage.googleapis.com/v1beta/models/" 
                       + config.model + ":generateContent?key=" + config.apiKey;
    
    // Escape prompt for JSON
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    juce::String jsonBody = "{";
    jsonBody += "\"contents\": [{";
    jsonBody += "\"parts\": [{";
    jsonBody += "\"text\": \"" + escapedPrompt + "\"";
    jsonBody += "}]";
    jsonBody += "}],";
    jsonBody += "\"generationConfig\": {";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    if (config.maxTokens > 0)
        jsonBody += ", \"maxOutputTokens\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    jsonBody += "}";
    
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from Gemini API";
        return "[ERROR] " + lastError;
    }
    
    // Parse Gemini response: candidates[0].content.parts[0].text
    // Simplified parsing - look for "text": "..."
    juce::String aiResponse = parseJsonResponse(response, "text");
    if (aiResponse.isEmpty())
    {
        lastError = "Failed to parse Gemini response";
        return "[ERROR] " + lastError + " | Raw: " + response.substring(0, 200);
    }
    
    return aiResponse;
}

juce::String AiEngine::callAnthropic(const juce::String& prompt)
{
    if (config.apiKey.isEmpty())
    {
        lastError = "Anthropic API key not configured";
        return "[ERROR] " + lastError;
    }
    
    // Anthropic API: POST https://api.anthropic.com/v1/messages
    juce::String url = "https://api.anthropic.com/v1/messages";
    
    // Escape prompt for JSON
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"max_tokens\": " + juce::String(config.maxTokens > 0 ? config.maxTokens : 1024) + ",";
    jsonBody += "\"messages\": [{";
    jsonBody += "\"role\": \"user\",";
    jsonBody += "\"content\": \"" + escapedPrompt + "\"";
    jsonBody += "}],";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    jsonBody += "}";
    
    // Note: Anthropic requires custom header "x-api-key" instead of Authorization Bearer
    // We'll need to modify makeHttpRequest for this, but for now use standard auth
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from Anthropic API";
        return "[ERROR] " + lastError;
    }
    
    // Parse Anthropic response: content[0].text
    juce::String aiResponse = parseJsonResponse(response, "text");
    if (aiResponse.isEmpty())
    {
        lastError = "Failed to parse Anthropic response";
        return "[ERROR] " + lastError + " | Raw: " + response.substring(0, 200);
    }
    
    return aiResponse;
}

juce::String AiEngine::callOpenAI(const juce::String& prompt)
{
    if (config.apiKey.isEmpty())
    {
        lastError = "OpenAI API key not configured";
        return "[ERROR] " + lastError;
    }
    
    // OpenAI API: POST https://api.openai.com/v1/chat/completions
    juce::String url = "https://api.openai.com/v1/chat/completions";
    
    // Escape prompt for JSON
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"messages\": [{";
    jsonBody += "\"role\": \"user\",";
    jsonBody += "\"content\": \"" + escapedPrompt + "\"";
    jsonBody += "}],";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    if (config.maxTokens > 0)
        jsonBody += ", \"max_tokens\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from OpenAI API";
        return "[ERROR] " + lastError;
    }
    
    // Parse OpenAI response: choices[0].message.content
    // First try to find "content" key
    juce::String aiResponse = parseJsonResponse(response, "content");
    if (aiResponse.isEmpty())
    {
        lastError = "Failed to parse OpenAI response";
        return "[ERROR] " + lastError + " | Raw: " + response.substring(0, 200);
    }
    
    return aiResponse;
}

juce::String AiEngine::callOpenRouter(const juce::String& prompt)
{
    if (config.apiKey.isEmpty())
    {
        lastError = "OpenRouter API key not configured";
        return "[ERROR] " + lastError;
    }
    
    // OpenRouter API: POST https://openrouter.ai/api/v1/chat/completions
    juce::String url = "https://openrouter.ai/api/v1/chat/completions";
    
    // Escape prompt for JSON
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"messages\": [{";
    jsonBody += "\"role\": \"user\",";
    jsonBody += "\"content\": \"" + escapedPrompt + "\"";
    jsonBody += "}],";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    if (config.maxTokens > 0)
        jsonBody += ", \"max_tokens\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from OpenRouter API";
        return "[ERROR] " + lastError;
    }
    
    // Parse OpenRouter response (same format as OpenAI): choices[0].message.content
    juce::String aiResponse = parseJsonResponse(response, "content");
    if (aiResponse.isEmpty())
    {
        lastError = "Failed to parse OpenRouter response";
        return "[ERROR] " + lastError + " | Raw: " + response.substring(0, 200);
    }
    
    return aiResponse;
}

juce::String AiEngine::callGroq(const juce::String& prompt)
{
    if (config.apiKey.isEmpty())
    {
        lastError = "Groq API key not configured";
        return "[ERROR] " + lastError;
    }
    
    // Groq API: POST https://api.groq.com/openai/v1/chat/completions
    juce::String url = "https://api.groq.com/openai/v1/chat/completions";
    
    // Escape prompt for JSON
    juce::String escapedPrompt = escapeJsonString(prompt);
    
    juce::String jsonBody = "{";
    jsonBody += "\"model\": \"" + config.model + "\",";
    jsonBody += "\"messages\": [{";
    jsonBody += "\"role\": \"user\",";
    jsonBody += "\"content\": \"" + escapedPrompt + "\"";
    jsonBody += "}],";
    jsonBody += "\"temperature\": " + juce::String(config.temperature);
    if (config.maxTokens > 0)
        jsonBody += ", \"max_tokens\": " + juce::String(config.maxTokens);
    jsonBody += "}";
    
    juce::String response = makeHttpRequest(url, "POST", jsonBody, config.timeoutMs);
    
    if (response.isEmpty())
    {
        if (lastError.isEmpty())
            lastError = "Empty response from Groq API";
        return "[ERROR] " + lastError;
    }
    
    // Parse Groq response (same format as OpenAI): choices[0].message.content
    juce::String aiResponse = parseJsonResponse(response, "content");
    if (aiResponse.isEmpty())
    {
        lastError = "Failed to parse Groq response";
        return "[ERROR] " + lastError + " | Raw: " + response.substring(0, 200);
    }
    
    return aiResponse;
}
