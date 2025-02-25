/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================

// CLASSE POUR L'AFFICHAGE DE LA WAVEFORM

//==============================================================================

class WaveformDisplay : public juce::Component, private juce::Timer
{
public:
    
    WaveformDisplay(TestPluginAudioProcessor& p) : audioProcessor(p) {
        startTimerHz(20);
    }

    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black); // Fond noir
        g.setColour(juce::Colours::white); // Couleur de la waveform
        
        auto width = getWidth();
        auto height = getHeight();
        
        auto* buffer = audioProcessor.waveformBuffer.getReadPointer(0); // Premier canal
        int numSamples = audioProcessor.waveformBuffer.getNumSamples();

        if (numSamples > 0)
        {
            juce::Path waveformPath;
            waveformPath.startNewSubPath(0, height / 2);

            for (int i = 0; i < width; ++i)
            {
                int sampleIndex = juce::jmap(i, 0, width, 0, numSamples); // Mapper largeur → samples
                float sampleValue = buffer[sampleIndex] * (height / 2);   // Amplitude normalisée
                waveformPath.lineTo(i, height / 2 - sampleValue);
            }

            g.strokePath(waveformPath, juce::PathStrokeType(2.0f));
        }
    }

    void resized() override {}

private:
    TestPluginAudioProcessor& audioProcessor;
    void timerCallback() override
        {
            repaint(); // Rafraîchit l'affichage régulièrement
        }
};


//==============================================================================


class TestPluginAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    
    TestPluginAudioProcessorEditor (TestPluginAudioProcessor&);
    ~TestPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    TestPluginAudioProcessor& audioProcessor;
    
    juce::Slider fader;
    // initialisation de l'attache du fader
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> faderAttachment;
    
    WaveformDisplay waveformDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TestPluginAudioProcessorEditor)
};




