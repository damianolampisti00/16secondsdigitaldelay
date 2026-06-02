/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
_16secondsdigitaldelayAudioProcessor::_16secondsdigitaldelayAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
    , apvts(*this, nullptr, "Parameters", Parameters::createParameterLayout())
{
    apvts.addParameterListener(Parameters::nameState, this);
    apvts.addParameterListener(Parameters::nameReverse, this);
    apvts.addParameterListener(Parameters::nameInputGain, this);
    apvts.addParameterListener(Parameters::nameOutputGain, this);
    apvts.addParameterListener(Parameters::nameDryWet, this);
    apvts.addParameterListener(Parameters::nameFeedback, this);
}

_16secondsdigitaldelayAudioProcessor::~_16secondsdigitaldelayAudioProcessor()
{
}

//==============================================================================
const juce::String _16secondsdigitaldelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool _16secondsdigitaldelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool _16secondsdigitaldelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool _16secondsdigitaldelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double _16secondsdigitaldelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int _16secondsdigitaldelayAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int _16secondsdigitaldelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void _16secondsdigitaldelayAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String _16secondsdigitaldelayAudioProcessor::getProgramName (int index)
{
    return {};
}

void _16secondsdigitaldelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void _16secondsdigitaldelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    vintageDelay.prepareToPlay(sampleRate, samplesPerBlock);
}

void _16secondsdigitaldelayAudioProcessor::releaseResources()
{
    vintageDelay.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool _16secondsdigitaldelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void _16secondsdigitaldelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // Process the block through the Vintage Delay
    vintageDelay.processBlock(buffer);
}

//==============================================================================
bool _16secondsdigitaldelayAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* _16secondsdigitaldelayAudioProcessor::createEditor()
{
    return new _16secondsdigitaldelayAudioProcessorEditor (*this);
}

//==============================================================================
void _16secondsdigitaldelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void _16secondsdigitaldelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void _16secondsdigitaldelayAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    if (parameterID == Parameters::nameState)
        vintageDelay.setLoopState(static_cast<LoopState>(static_cast<int>(newValue)));
    else if (parameterID == Parameters::nameReverse)
        vintageDelay.setReverse(newValue >= 0.5f);
    else if (parameterID == Parameters::nameInputGain)
        vintageDelay.setInputGain(newValue);
    else if (parameterID == Parameters::nameOutputGain)
        vintageDelay.setOutputGain(newValue);
    else if (parameterID == Parameters::nameDryWet)
        vintageDelay.setDryWet(newValue);
    else if (parameterID == Parameters::nameFeedback)
        vintageDelay.setFeedback(newValue);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new _16secondsdigitaldelayAudioProcessor();
}
