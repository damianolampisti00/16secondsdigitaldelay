#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Parameters.h"

_16secondsdigitaldelayAudioProcessorEditor::_16secondsdigitaldelayAudioProcessorEditor (_16secondsdigitaldelayAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // State Combo
    addAndMakeVisible(stateCombo);
    stateCombo.addItemList({"Idle", "CountIn", "Recording", "Playing", "Overdubbing"}, 1);
    stateAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, Parameters::nameState, stateCombo);

    // Reverse Toggle
    addAndMakeVisible(reverseToggle);
    reverseToggle.setButtonText("Reverse");
    reverseAttachment = std::make_unique<ButtonAttachment>(audioProcessor.apvts, Parameters::nameReverse, reverseToggle);

    // Input Gain
    addAndMakeVisible(inputGainSlider);
    inputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    inputGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    inputGainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, Parameters::nameInputGain, inputGainSlider);

    // Output Gain
    addAndMakeVisible(outputGainSlider);
    outputGainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    outputGainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, Parameters::nameOutputGain, outputGainSlider);

    // Dry/Wet
    addAndMakeVisible(dryWetSlider);
    dryWetSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    dryWetSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    dryWetAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, Parameters::nameDryWet, dryWetSlider);

    // Feedback
    addAndMakeVisible(feedbackSlider);
    feedbackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    feedbackAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, Parameters::nameFeedback, feedbackSlider);

    setSize (400, 350);
}

_16secondsdigitaldelayAudioProcessorEditor::~_16secondsdigitaldelayAudioProcessorEditor()
{
}

void _16secondsdigitaldelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colours::darkgrey);
    g.setColour (juce::Colours::white);
    g.setFont(14.0f);
    
    g.drawText("Loop State:", 20, 20, 100, 20, juce::Justification::centredLeft);
    g.drawText("Input Gain:", 20, 100, 100, 20, juce::Justification::centredLeft);
    g.drawText("Output Gain:", 20, 150, 100, 20, juce::Justification::centredLeft);
    g.drawText("Dry/Wet:", 20, 200, 100, 20, juce::Justification::centredLeft);
    g.drawText("Feedback:", 20, 250, 100, 20, juce::Justification::centredLeft);
}

void _16secondsdigitaldelayAudioProcessorEditor::resized()
{
    stateCombo.setBounds(120, 20, 150, 20);
    reverseToggle.setBounds(120, 60, 100, 20);
    inputGainSlider.setBounds(120, 100, 250, 20);
    outputGainSlider.setBounds(120, 150, 250, 20);
    dryWetSlider.setBounds(120, 200, 250, 20);
    feedbackSlider.setBounds(120, 250, 250, 20);
}
