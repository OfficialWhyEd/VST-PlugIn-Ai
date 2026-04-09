/*
  ==============================================================================
  PluginProcessor.h
  OpenClaw VST Bridge AI - Main Audio Processor
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <memory>

class AiEngine;
class OscHandler;

//==============================================================================
class OpenClawAudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    OpenClawAudioProcessor();
    ~OpenClawAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    #endif

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return "OpenClaw VST Bridge AI"; }

    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return true; }
    bool isMidiEffect() const override { return false; }

    //==============================================================================
    double getTailLengthSeconds() const override { return 0.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // OpenClaw specific methods
    
    // AI Engine
    void sendAiPrompt(const juce::String& prompt);
    juce::String getLastAiResponse() const { return lastAiResponse; }
    
    // Parameters
    juce::AudioProcessorValueTreeState& getParameters() { return parameters; }
    
    // OSC
    int getOscPort() const { return oscPort; }
    void setOscPort(int port);

private:
    //==============================================================================
    // Parameters
    juce::AudioProcessorValueTreeState parameters;
    
    // Parameter pointers
    std::atomic<float>* gainParam1 = nullptr;
    std::atomic<float>* gainParam2 = nullptr;
    
    // AI Engine
    std::unique_ptr<AiEngine> aiEngine;
    juce::String lastAiResponse;
    
    // OSC Handler
    std::unique_ptr<OscHandler> oscHandler;
    int oscPort = 9000;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenClawAudioProcessor)
};