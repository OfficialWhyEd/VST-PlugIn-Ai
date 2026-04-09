/*
  ==============================================================================
  OscHandler.h
  OpenClaw VST Bridge AI - OSC Communication Handler
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscHandler
{
public:
    OscHandler(int port = 9000);
    ~OscHandler();
    
    void start();
    void stop();
    
    void sendMessage(const juce::String& address, float value);
    void sendMessage(const juce::String& address, const juce::String& value);
    
private:
    int port;
    bool running = false;
};