/*
   ==============================================================================
   OscHandler.h
   OpenClaw VST Bridge AI - OSC Communication Handler
   ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <functional>

class OscHandler
{
public:
    OscHandler(int port = 9000);
    ~OscHandler();
    
    void start();
    void stop();
    
    void sendMessage(const juce::String& address, float value);
    void sendMessage(const juce::String& address, const juce::String& value);
    void sendMessage(const juce::String& address, int value);
    
    // Callback for incoming messages
    using MessageCallback = std::function<void(const juce::String&, const juce::var&)>;
    void setMessageCallback(MessageCallback callback);
    
private:
    int port;
    bool running = false;
    juce::DatagramSocket socket;
    MessageCallback messageCallback;
    std::unique_ptr<juce::Thread> receiverThread;
    
    void receiverThreadFunction();
    static void parseOscMessage(const juce::String& address, juce::MemoryBlock& data, MessageCallback callback);
};