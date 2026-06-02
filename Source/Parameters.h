#pragma once
#include <JuceHeader.h>

namespace Parameters
{
    static const juce::String nameState = "STATE";
    static const juce::String nameReverse = "REV";
    static const juce::String nameInputGain = "INGAIN";
    static const juce::String nameOutputGain = "OUTGAIN";
    static const juce::String nameDryWet = "DRYWET";
    static const juce::String nameFeedback = "FEEDBACK";

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
    {
        std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

        int id = 1;

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(nameState, id++), "Loop State",
            juce::StringArray{"Idle", "CountIn", "Recording", "Playing", "Overdubbing"}, 0));

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            juce::ParameterID(nameReverse, id++), "Reverse", false));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(nameInputGain, id++), "Input Gain",
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(nameOutputGain, id++), "Output Gain",
            juce::NormalisableRange<float>(0.0f, 2.0f, 0.01f), 1.0f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(nameDryWet, id++), "Dry/Wet",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID(nameFeedback, id++), "Feedback",
            juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

        return { params.begin(), params.end() };
    }
}
