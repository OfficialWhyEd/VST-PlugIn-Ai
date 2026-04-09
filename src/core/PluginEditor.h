/*
  ==============================================================================
  PluginEditor.h
  OpenClaw VST Bridge AI - Plugin Editor (GUI)
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class OpenClawAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    OpenClawAudioProcessorEditor(OpenClawAudioProcessor&);
    ~OpenClawAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // Reference to processor
    OpenClawAudioProcessor& audioProcessor;
    
    // GUI Components
    juce::Label titleLabel;
    juce::Slider gainSlider1;
    juce::Slider gainSlider2;
    juce::Label gainLabel1;
    juce::Label gainLabel2;
    juce::TextButton aiButton;
    juce::TextEditor aiPrompt;
    
    // Attachment for parameter automation
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment1;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment2;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OpenClawAudioProcessorEditor)
};