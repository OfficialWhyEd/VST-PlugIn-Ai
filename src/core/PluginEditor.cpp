/*
  ==============================================================================
  PluginEditor.cpp
  OpenClaw VST Bridge AI - Plugin Editor Implementation
  ==============================================================================
*/

#include "PluginEditor.h"

//==============================================================================
OpenClawAudioProcessorEditor::OpenClawAudioProcessorEditor(OpenClawAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(800, 600);
    
    // Title label
    titleLabel.setText("OpenClaw VST Bridge AI", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);
    
    // Gain Sliders
    gainSlider1.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    gainSlider1.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider1.setRange(-60.0, 12.0, 0.1);
    gainSlider1.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider1);
    
    gainLabel1.setText("Gain 1", juce::dontSendNotification);
    gainLabel1.setJustificationType(juce::Justification::centred);
    gainLabel1.attachToComponent(&gainSlider1, false);
    addAndMakeVisible(gainLabel1);
    
    gainSlider2.setSliderStyle(juce::Slider::RotaryHorizontalDrag);
    gainSlider2.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    gainSlider2.setRange(-60.0, 12.0, 0.1);
    gainSlider2.setTextValueSuffix(" dB");
    addAndMakeVisible(gainSlider2);
    
    gainLabel2.setText("Gain 2", juce::dontSendNotification);
    gainLabel2.setJustificationType(juce::Justification::centred);
    gainLabel2.attachToComponent(&gainSlider2, false);
    addAndMakeVisible(gainLabel2);
    
    // AI Button
    aiButton.setButtonText("Ask AI");
    aiButton.onClick = [this]() {
        audioProcessor.sendAiPrompt(aiPrompt.getText());
    };
    addAndMakeVisible(aiButton);
    
    // AI Prompt
    aiPrompt.setMultiLine(true);
    aiPrompt.setReturnKeyStartsNewLine(false);
    aiPrompt.setText("Ask the AI about your mix...");
    addAndMakeVisible(aiPrompt);
    
    // Parameter attachments
    gainAttachment1 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "gain1", gainSlider1);
    
    gainAttachment2 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getParameters(), "gain2", gainSlider2);
}

OpenClawAudioProcessorEditor::~OpenClawAudioProcessorEditor()
{
    // Clean up attachments
    gainAttachment1.reset();
    gainAttachment2.reset();
}

//==============================================================================
void OpenClawAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Dark background
    g.fillAll(juce::Colours::darkgrey);
    
    // Add some subtle styling
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillRect(0, 0, getWidth(), 40);
}

void OpenClawAudioProcessorEditor::resized()
{
    // Title
    titleLabel.setBounds(0, 10, getWidth(), 30);
    
    // Sliders
    int sliderWidth = 100;
    int sliderHeight = 100;
    int startX = 50;
    int startY = 80;
    
    gainSlider1.setBounds(startX, startY, sliderWidth, sliderHeight);
    gainSlider2.setBounds(startX + sliderWidth + 50, startY, sliderWidth, sliderHeight);
    
    // AI Section
    int aiY = 250;
    aiPrompt.setBounds(50, aiY, getWidth() - 100, 100);
    aiButton.setBounds(getWidth() - 150, aiY + 110, 100, 30);
}