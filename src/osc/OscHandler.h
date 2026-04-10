/*
  ==============================================================================
  OscHandler.h
  OpenClaw VST Bridge AI - OSC Communication Handler
  
  Listens for OSC messages from DAW (Reaper, Ableton, etc.)
  and forwards them to the plugin. Also sends OSC back to DAW.
  
  Phase 1: Receive-only (listen + log)
  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>
#include <juce_events/juce_events.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <thread>
#include <functional>
#include <atomic>
#include <map>

class OscHandler
{
public:
    //==============================================================================
    /** Callback type for incoming OSC messages */
    using OscCallback = std::function<void(const juce::String& address, float value)>;
    using OscStringCallback = std::function<void(const juce::String& address, const juce::String& value)>;
    
    // Legacy callback type for backward compatibility
    using MessageCallback = std::function<void(const juce::String&, const juce::var&)>;
    
    //==============================================================================
    OscHandler(int port = 9000);
    ~OscHandler();
    
    //==============================================================================
    /** Start/stop the OSC listener */
    void start();
    void stop();
    bool isRunning() const { return running.load(); }
    
    //==============================================================================
    /** Send OSC messages to DAW */
    void sendMessage(const juce::String& address, float value);
    void sendMessage(const juce::String& address, const juce::String& value);
    void sendMessage(const juce::String& address, int value);
    
    //==============================================================================
    /** Set callbacks for incoming messages */
    void setCallback(OscCallback callback);
    void setStringCallback(OscStringCallback callback);
    void setMessageCallback(MessageCallback callback);  // Legacy support
    
    //==============================================================================
    /** Set the OSC port (restarts listener if running) */
    void setPort(int newPort);
    int getPort() const { return port; }
    
    //==============================================================================
    /** Get the OSC message log (for UI/debug) */
    const juce::StringArray& getMessageLog() const { return messageLog; }
    void clearLog() { messageLog.clear(); }
    
    //==============================================================================
    /** OSC learn mode - captures next incoming address */
    void enableLearnMode(bool enable) { learnMode.store(enable); }
    bool isInLearnMode() const { return learnMode.load(); }
    juce::String getLearnedAddress() const { return learnedAddress; }
    
    //==============================================================================
    /** Connection status */
    bool isConnected() const { return connected.load(); }
    int getMessagesReceived() const { return messagesReceived.load(); }

private:
    //==============================================================================
    int port;
    std::atomic<bool> running{false};
    std::atomic<bool> connected{false};
    std::atomic<bool> learnMode{false};
    std::atomic<int> messagesReceived{0};
    
    juce::String learnedAddress;
    
    //==============================================================================
    /** Listener thread */
    std::unique_ptr<std::thread> listenerThread;
    void listenerLoop();
    
    //==============================================================================
    /** Message parsing */
    void handleOscPacket(const char* data, int size);
    void handleOscMessage(const juce::String& address, const juce::String& typeTag, 
                          const char* arguments, int argSize);
    
    //==============================================================================
    /** Callbacks */
    OscCallback callback;
    OscStringCallback stringCallback;
    MessageCallback messageCallback;  // Legacy
    
    //==============================================================================
    /** Message log (thread-safe via lock) */
    juce::StringArray messageLog;
    juce::CriticalSection logLock;
    void addToLog(const juce::String& msg);
    
    //==============================================================================
    /** UDP socket (using JUCE's UDP) */
    std::unique_ptr<juce::DatagramSocket> socket;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscHandler)
};