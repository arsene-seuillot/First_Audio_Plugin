/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TestPluginAudioProcessor::TestPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), treeState(*this, nullptr, "PARAMETERS", createParameterLayout())
#endif
{
    // Penser à mettre le listener ici !
    treeState.addParameterListener("gain", this);
    treeState.addParameterListener("test", this);
}

TestPluginAudioProcessor::~TestPluginAudioProcessor()
{
    treeState.removeParameterListener("gain", this);
    treeState.removeParameterListener("test", this);
}


// ON IMPLÉMENTE LA FONCTION QUI CRÉÉE LA LISTE DES PARAMÈTERES DE L'AUDIO-TREE

juce::AudioProcessorValueTreeState::ParameterLayout TestPluginAudioProcessor::createParameterLayout() {
    
    std::vector <std::unique_ptr<juce::RangedAudioParameter>> params;
    
    // Ici on crée les différents paramètres du ParameterLayout
    auto pGain = std::make_unique<juce::AudioParameterFloat>("gain", "Gain", -24.0, 24.0, 10.0);
    auto pTest = std::make_unique<juce::AudioParameterInt>("test", "Test", -10, 10, 0);
    
    
    params.push_back(std::move(pGain));
    params.push_back(std::move(pTest));
    
    return {params.begin(), params.end()};
}


// MÉTHODE POUR NE METTRE À JOUR LES VALEURS QUE QUAND ELLES CHANGENT
void TestPluginAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {
    
    // Quand une nouvelle valeur est détectée dans le listener, newValue prend cette valeur
    // et on sait quel paramètre est concerné avec parameterID.

    if (parameterID == "gain") {
        rawGain = juce::Decibels::decibelsToGain(newValue);
        //DBG("Gain is :" << newValue);
    }
    if (parameterID == "test") {
        val_test = newValue;
        //DBG("Test vaut :" << newValue);
    }
}


//==============================================================================
const juce::String TestPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TestPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TestPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TestPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TestPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TestPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TestPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TestPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TestPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void TestPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TestPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rawGain = juce::Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("gain")));
    val_test = *treeState.getRawParameterValue("test");
}

void TestPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TestPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TestPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    // BLOC POUR BOUCLER SUR LE BUFFER
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);
      
        for (int sample=0; sample < buffer.getNumSamples(); sample++) {
            
            channelData[sample] *= rawGain;
        }

        // ..do something to the data...
    }
}

//==============================================================================
bool TestPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TestPluginAudioProcessor::createEditor()
{
    return new TestPluginAudioProcessorEditor (*this);
    // Éditor qui se base sur ce qu'on a dans le layout : permet de modifier les valeurs avec des sliders ou autres directement
    //return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void TestPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // Save parameters
    juce::MemoryOutputStream stream(destData, false);
    treeState.state.writeToStream(stream);
}

void TestPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // Recall parameters
    auto tree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));
    if (tree.isValid()) {
        treeState.state = tree;
        rawGain = juce::Decibels::decibelsToGain(static_cast<float>(*treeState.getRawParameterValue("gain")));
        val_test = *treeState.getRawParameterValue("test");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TestPluginAudioProcessor();
}
