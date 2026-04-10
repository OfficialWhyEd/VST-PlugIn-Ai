/*
  ==============================================================================
  AiEngine.h
  OpenClaw VST Bridge AI - AI Engine for multi-provider support
  
  Phase 1: Placeholder - returns mock responses
  Phase 2: Will integrate Ollama, Gemini, Anthropic, OpenAI, OpenRouter, Groq
  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <memory>

class AiEngine
{
public:
    enum class Provider { 
        Ollama,      // Local LLM (default)
        Gemini,      // Google Gemini
        Anthropic,   // Claude
        OpenAI,      // GPT
        OpenRouter,  // Multi-provider
        Groq         // Fast inference
    };
    
    struct Config {
        Provider provider = Provider::Ollama;
        juce::String apiKey;
        juce::String model = "llama3.2";
        juce::String baseUrl = "http://localhost:11434";
        int timeoutMs = 30000;
        int maxTokens = 2048;
        float temperature = 0.7f;
    };
    
    AiEngine();
    ~AiEngine();
    
    //==============================================================================
    /** Configure the AI provider */
    void configure(const Config& config);
    
    //==============================================================================
    /** Send a prompt and get response (synchronous - blocks!) */
    juce::String sendPrompt(const juce::String& prompt);
    
    //==============================================================================
    /** Get available models for current provider */
    juce::StringArray getAvailableModels();
    
    //==============================================================================
    /** Test connection to provider */
    bool testConnection();
    
    //==============================================================================
    /** Get last error message */
    juce::String getLastError() const { return lastError; }
    
    //==============================================================================
    /** Check if configured */
    bool isConfigured() const { return configured; }

private:
    Config config;
    bool configured = false;
    juce::String lastError;
    
    //==============================================================================
    /** Internal implementations for each provider */
    juce::String callOllama(const juce::String& prompt);
    juce::String callGemini(const juce::String& prompt);
    juce::String callAnthropic(const juce::String& prompt);
    juce::String callOpenAI(const juce::String& prompt);
    juce::String callOpenRouter(const juce::String& prompt);
    juce::String callGroq(const juce::String& prompt);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AiEngine)
};