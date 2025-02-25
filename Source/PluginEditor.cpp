/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestPluginAudioProcessorEditor::TestPluginAudioProcessorEditor (TestPluginAudioProcessor& p)
: AudioProcessorEditor (&p), audioProcessor (p), waveformDisplay(p)
{
    // C'est ici qu'on met les composants et qu'on gère leur style
    addAndMakeVisible(waveformDisplay);
    addAndMakeVisible(fader);
    fader.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    // La taille de la boite de texte ne dépassera pas la taille totale du composant !
    fader.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 32);
    fader.setRange(-24.0, 24.0, 0.1);
    fader.setColour(juce::Slider::ColourIds::trackColourId, juce::Colours::whitesmoke);
    fader.setDoubleClickReturnValue(true, 0.0);
    
    // On attache le fader au paramètre "gain" de treeState.
    faderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.treeState, "gain", fader);
    
    // On peut changer la taille de la fenêtre, avec un ratio particulier
    setResizable(true, true);
    setResizeLimits(500, 250, 1500, 750);
    getConstrainer()->setFixedAspectRatio(1.618);
    
    setSize (1000, 500);
}

TestPluginAudioProcessorEditor::~TestPluginAudioProcessorEditor()
{
}

//==============================================================================
void TestPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black.brighter(0.1).withAlpha(0.5f));
    
    g.setColour (juce::Colours::whitesmoke);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Ceci est un test.", getLocalBounds(), juce::Justification::centred, 1);
}

void TestPluginAudioProcessorEditor::resized() {
    
    // On peut récupérer en temps réel la taille de la fenêtre pour adapter les composants
    auto leftMargin = getWidth()*0.02;
    auto topMargin = getHeight()*0.04;
    auto faderSize = getWidth()*0.05;
    fader.setBounds(40, 160, faderSize, 5*faderSize);
    waveformDisplay.setBounds(0, 0, getWidth(), 150);
}
