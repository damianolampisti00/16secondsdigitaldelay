#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class _16secondsdigitaldelayAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    _16secondsdigitaldelayAudioProcessorEditor (_16secondsdigitaldelayAudioProcessor&);
    ~_16secondsdigitaldelayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    _16secondsdigitaldelayAudioProcessor& audioProcessor;

    juce::ComboBox stateCombo;
    juce::ToggleButton reverseToggle;
    juce::Slider inputGainSlider;
    juce::Slider outputGainSlider;
    juce::Slider dryWetSlider;
    juce::Slider feedbackSlider;

    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    std::unique_ptr<ComboBoxAttachment> stateAttachment;
    std::unique_ptr<ButtonAttachment> reverseAttachment;
    std::unique_ptr<SliderAttachment> inputGainAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;
    std::unique_ptr<SliderAttachment> dryWetAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (_16secondsdigitaldelayAudioProcessorEditor)
};
