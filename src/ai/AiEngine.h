/*
  ==============================================================================
  AiEngine.h
  OpenClaw VST Bridge AI - AI Engine for multi-provider support
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

class AiEngine
{
public:
    enum class Provider { Ollama, Gemini, Anthropic, OpenAI, OpenRouter, Groq };
    
    struct Config {
        Provider provider;
        juce::String apiKey;
        juce::String model;
        juce::String baseUrl;
        int timeoutMs = 10000;
    };
    
    AiEngine();
    ~AiEngine();
    
    void configure(const Config& config);
    juce::String sendPrompt(const juce::String& prompt);
    
private:
    Config config;
};