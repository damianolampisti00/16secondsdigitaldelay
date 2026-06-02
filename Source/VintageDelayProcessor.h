#pragma once

#include <JuceHeader.h>
#include "SignalPath.h"
#include "Compander.h"
#include "Converters.h"
#include "MemoryController.h"
#include "ClockGenerator.h"
#include "DryWet.h"

class VintageDelayProcessor
{
public:
    VintageDelayProcessor();
    ~VintageDelayProcessor();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void processBlock(juce::AudioBuffer<float>& buffer);

    // Setters for parameters
    void setLoopState(LoopState state);
    void setReverse(bool reverse);
    void setInputGain(float gain);
    void setOutputGain(float gain);
    void setDryWet(float dw);
    void setFeedback(float fb);

private:
    double sampleRate{ 44100.0 };

    DryWet dryWetter;

    InputStage inputStage;
    CompressorNE570 compressor;
    AntiAliasFilter antiAliasFilter;
    
    ADC12Bit adc;
    MemoryController memoryController;
    DAC12Bit dac;

    ReconstructionFilter reconFilter;
    ExpanderNE570 expander;
    OutputStage outputStage;

    ClockGenerator clock;
};
