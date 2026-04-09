/*
  ==============================================================================
  PluginProcessor.cpp
  OpenClaw VST Bridge AI - Main Audio Processor Implementation
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

// Forward declarations for incomplete types
class AiEngine {};
class OscHandler {};

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
    
    // Initialize AI Engine (placeholder - will be implemented)
    // aiEngine = std::make_unique<AiEngine>();
    
    // Initialize OSC Handler (placeholder - will be implemented)
    // oscHandler = std::make_unique<OscHandler>(oscPort);
}

OpenClawAudioProcessor::~OpenClawAudioProcessor() = default;

//==============================================================================
void OpenClawAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare AI Engine
    // if (aiEngine)
    //     aiEngine->prepare(sampleRate, samplesPerBlock);
    
    // Start OSC Handler
    // if (oscHandler)
    //     oscHandler->start();
}

void OpenClawAudioProcessor::releaseResources()
{
    // Stop OSC Handler
    // if (oscHandler)
    //     oscHandler->stop();
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
    
    // Process MIDI messages (placeholder)
    for (const auto midiMessage : midiMessages)
    {
        auto message = midiMessage.getMessage();
        // Handle MIDI CC messages for parameter control
        if (message.isController())
        {
            int ccNumber = message.getControllerNumber();
            int ccValue = message.getControllerValue();
            // Map MIDI CC to parameters (to be implemented)
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
    juce::ignoreUnused(prompt);
    // TODO: Send prompt to AI Engine
    // if (aiEngine)
    //     lastAiResponse = aiEngine->sendPrompt(prompt);
}

void OpenClawAudioProcessor::setOscPort(int port)
{
    oscPort = port;
    // TODO: Restart OSC handler with new port
    // if (oscHandler)
    //     oscHandler->setPort(port);
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