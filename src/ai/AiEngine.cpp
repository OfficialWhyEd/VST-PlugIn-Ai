/*
  ==============================================================================
  AiEngine.cpp
  OpenClaw VST Bridge AI - AI Engine Implementation
  ==============================================================================
*/

#include "AiEngine.h"

AiEngine::AiEngine()
{
}

AiEngine::~AiEngine()
{
}

void AiEngine::configure(const Config& cfg)
{
    config = cfg;
}

juce::String AiEngine::sendPrompt(const juce::String& prompt)
{
    juce::ignoreUnused(prompt);
    // TODO: Implement AI provider calls
    return "AI response placeholder";
}