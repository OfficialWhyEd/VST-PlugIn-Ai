/*
  ==============================================================================
  PluginProcessor.cpp
  OpenClaw VST Bridge AI - Main Audio Processor Implementation
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "OscHandler.h"
#include "AiEngine.h"

//==============================================================================
OpenClawAudioProcessor::OpenClawAudioProcessor()
    : parameters(*this, nullptr, "Parameters", []() -> juce::AudioProcessorValueTreeState::ParameterLayout
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
        
        // Gain parameters (8 tracks)
        for (int i = 1; i <= 8; ++i)
        {
            params.push_back(std::make_unique<juce::AudioParameterFloat>(
                "gain" + juce::String(i), 
                "Gain " + juce::String(i), 
                -60.0f, 12.0f, 0.0f));
        }
        
        // AI Enable
        params.push_back(std::make_unique<juce::AudioParameterBool>(
            "aiEnabled", "AI Enabled", true));
        
        // OSC Port
        params.push_back(std::make_unique<juce::AudioParameterInt>(
            "oscPort", "OSC Port", 1024, 65535, 9000));
        
        return { params.begin(), params.end() };
    }())
{
    // Get parameter pointers
    gainParam1 = parameters.getRawParameterValue("gain1");
    gainParam2 = parameters.getRawParameterValue("gain2");
    
    // Initialize OSC Handler
    oscHandler = std::make_unique<OscHandler>(oscPort);
    
    // Set callback for incoming OSC messages
    oscHandler->setCallback([this](const juce::String& address, float value) {
        handleOscMessage(address, value);
    });
    
    // Initialize AI Engine (placeholder)
    aiEngine = std::make_unique<AiEngine>();
}

OpenClawAudioProcessor::~OpenClawAudioProcessor()
{
    // Stop OSC before destruction
    if (oscHandler)
        oscHandler->stop();
}

//==============================================================================
void OpenClawAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    
    // Start OSC Handler
    if (oscHandler && !oscHandler->isRunning())
    {
        oscHandler->start();
        DBG("[OpenClaw] OSC Handler started on port " + juce::String(oscPort));
    }
}

void OpenClawAudioProcessor::releaseResources()
{
    // Stop OSC Handler
    if (oscHandler)
    {
        oscHandler->stop();
        DBG("[OpenClaw] OSC Handler stopped");
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool OpenClawAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Support mono and stereo
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::mono())
        return true;
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo())
        return true;
    
    return false;
}
#endif

void OpenClawAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    // Apply gain from parameter
    if (gainParam1 != nullptr)
    {
        float gain = juce::Decibels::decibelsToGain(gainParam1->load());
        buffer.applyGain(gain);
    }
    
    // Process MIDI messages
    for (const auto midiMessage : midiMessages)
    {
        auto message = midiMessage.getMessage();
        if (message.isController())
        {
            int ccNumber = message.getControllerNumber();
            int ccValue = message.getControllerValue();
            juce::ignoreUnused(ccNumber, ccValue);
            // TODO: Map MIDI CC to parameters
        }
    }
}

//==============================================================================
void OpenClawAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String OpenClawAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return "Default";
}

void OpenClawAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void OpenClawAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void OpenClawAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}

//==============================================================================
// OpenClaw specific methods

void OpenClawAudioProcessor::sendAiPrompt(const juce::String& prompt)
{
    if (aiEngine)
        lastAiResponse = aiEngine->sendPrompt(prompt);
}

void OpenClawAudioProcessor::setOscPort(int port)
{
    oscPort = port;
    if (oscHandler)
        oscHandler->setPort(port);
}

bool OpenClawAudioProcessor::isOscConnected() const
{
    return oscHandler ? oscHandler->isConnected() : false;
}

juce::StringArray OpenClawAudioProcessor::getOscLog() const
{
    return oscHandler ? oscHandler->getMessageLog() : juce::StringArray();
}

void OpenClawAudioProcessor::clearOscLog()
{
    if (oscHandler)
        oscHandler->clearLog();
}

void OpenClawAudioProcessor::handleOscMessage(const juce::String& address, float value)
{
    // This is called from the OSC listener thread!
    // We need to be thread-safe here
    
    DBG("[OpenClaw] OSC received: " + address + " = " + juce::String(value, 3));
    
    // Map common OSC addresses to parameters
    // Reaper uses: /track/1/volume, /track/1/pan, etc.
    // Ableton OSC uses similar patterns
    
    // Example mapping:
    if (address.contains("volume") || address.contains("gain"))
    {
        // Convert 0-1 range to dB (-60 to +12)
        float dbValue = -60.0f + value * 72.0f;  // 72dB range
        // Note: We can't modify parameters from audio thread safely
        // This is just for logging/MIDI mapping in Phase 2
    }
    else if (address.contains("play"))
    {
        // Transport play
        DBG("[OpenClaw] Transport: PLAY");
    }
    else if (address.contains("stop"))
    {
        // Transport stop
        DBG("[OpenClaw] Transport: STOP");
    }
    else if (address.contains("record"))
    {
        // Transport record
        DBG("[OpenClaw] Transport: RECORD");
    }
    
    // In Phase 2, we'll forward these to the UI via AsyncUpdater
    // and implement bidirectional OSC (plugin -> DAW)
}

//==============================================================================
juce::AudioProcessorEditor* OpenClawAudioProcessor::createEditor()
{
    return new OpenClawAudioProcessorEditor(*this);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OpenClawAudioProcessor();
}