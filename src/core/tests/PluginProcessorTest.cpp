/*
  ==============================================================================
    PluginProcessorTest.cpp
    Unit tests for WhyCremisiPluginProcessor
  ==============================================================================
*/

#include "../PluginProcessor.h"
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PluginProcessorTest : public juce::UnitTest
{
public:
    // La categoria e' una semplice stringa: juce::UnitTestCategory non
    // esiste, e con quel nome il file non compilava. Usiamo una categoria
    // nostra per non trascinarci dietro l'intera suite interna di JUCE,
    // che dura minuti e non riguarda questo plugin.
    PluginProcessorTest() : juce::UnitTest ("PluginProcessorTest", "whycremisi") {}
    
    void runTest() override
    {
        beginTest ("Initial state");
        {
            WhyCremisiProcessor processor;
            
            // Check that the processor is created successfully
            expect (processor.hasEditor(), "Processor should have an editor");
            // getName() restituisce JucePlugin_Name, che e' il nome con cui
            // il plugin si presenta al DAW. Prima qui ci si aspettava
            // "WhyCremisi VST Plugin", che non e' mai stato quel valore.
            expect (processor.getName() == "WhyCremisi", "Processor name should be correct");
        }
        
        beginTest ("Gain parameters");
        {
            WhyCremisiProcessor processor;
            
            // Check that we have the gain parameters
            expect (processor.getParameters().getParameter ("gain1") != nullptr, "gain1 parameter should exist");
            expect (processor.getParameters().getParameter ("gain2") != nullptr, "gain2 parameter should exist");
            
            // Set gain1 to +6dB using normalized value: (6 - (-60)) / 72 = 0.917
            auto* p1 = processor.getParameters().getParameter ("gain1");
            p1->setValueNotifyingHost (p1->convertTo0to1 (6.0f));
            
            // prepareToPlay va sempre chiamato prima di processBlock: e' il
            // contratto di JUCE, ed e' li' che i moduli allocano i propri
            // buffer. Questo test lo saltava, e il plugin ci andava in crash.
            processor.prepareToPlay (48000.0, 1024);

            // Create a simple buffer
            juce::AudioBuffer<float> buffer (2, 1024);
            buffer.clear();
            
            // Add a test signal (0.5)
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                    buffer.setSample (channel, sample, 0.5f);
            
            // Il gain sale con una rampa di 100 ms per non produrre scatti:
            // servono piu' blocchi prima che arrivi a destinazione. Un solo
            // blocco coglierebbe la rampa a meta' e il confronto sarebbe
            // sbagliato a prescindere dal comportamento del plugin.
            juce::MidiBuffer midi;
            for (int blocco = 0; blocco < 8; ++blocco)
            {
                for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
                    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                        buffer.setSample (channel, sample, 0.5f);

                processor.processBlock (buffer, midi);
            }
            
            // gainParam1 (+6dB = factor ~2.0) is applied via smoothedGain
            // Expected: 0.5 * ~2.0 ≈ 1.0
            const float tolerance = 0.01f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    float sampleValue = buffer.getSample (channel, sample);
                    expect (std::abs (sampleValue - 1.0f) < tolerance, 
                            "Sample should be ~1.0 after applying +6dB gain");
                }
            }
        }
        
        beginTest ("Gain zero");
        {
            WhyCremisiProcessor processor;
            
            // Set gain1 to 0dB (factor 1.0) using normalized value: (0 - (-60)) / 72 = 0.833
            auto* p1 = processor.getParameters().getParameter ("gain1");
            p1->setValueNotifyingHost (p1->convertTo0to1 (0.0f));
            
            juce::AudioBuffer<float> buffer (2, 1024);
            buffer.setSample (0, 0, 0.5f);
            
            juce::MidiBuffer midi;
            processor.processBlock (buffer, midi);
            
            expect (std::abs (buffer.getSample (0, 0) - 0.5f) < 0.01f, 
                    "With 0dB gain, signal should be unchanged");
        }
        
        beginTest ("Gain silence");
        {
            WhyCremisiProcessor processor;
            
            // Set gain1 to -60dB (factor ~0.001) using normalized value: (-60 - (-60)) / 72 = 0.0
            auto* p1 = processor.getParameters().getParameter ("gain1");
            p1->setValueNotifyingHost (p1->convertTo0to1 (-60.0f));
            
            juce::AudioBuffer<float> buffer (2, 1024);
            buffer.setSample (0, 0, 1.0f);
            
            juce::MidiBuffer midi;
            processor.processBlock (buffer, midi);
            
            // With -60dB, gain factor is ~0.001
            // So 1.0 * 0.001 = 0.001
            expect (std::abs (buffer.getSample (0, 0)) < 0.01f, 
                    "With -60dB gain, signal should be near zero");
        }
    }
};

static PluginProcessorTest pluginProcessorTest;