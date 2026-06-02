#pragma once

#include <JuceHeader.h>
#include "DSPInterfaces.h"

class InputStage : public AudioBlock
{
public:
    InputStage() = default;

    float process(float sample) override
    {
        // Apply gain and saturation
        float gained = sample * gain;
        // Simple soft clipping for saturation
        return std::tanh(gained * saturationAmount);
    }

    void reset() override
    {
        // Nothing to reset
    }

    void setGain(float newGain) { gain = newGain; }
    void setSaturation(float newSat) { saturationAmount = newSat; }

private:
    float gain{ 1.0f };
    float saturationAmount{ 1.0f };
};

class AntiAliasFilter : public AudioBlock
{
public:
    AntiAliasFilter() = default;

    void prepare(double sampleRate)
    {
        filter.prepare({ sampleRate, 1, 1 });
        updateCoefficients(sampleRate);
    }

    float process(float sample) override
    {
        return filter.processSample(sample);
    }

    void reset() override
    {
        filter.reset();
    }

    void setCutoff(float newCutoff)
    {
        cutoff = newCutoff;
    }

private:
    void updateCoefficients(double sampleRate)
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);
    }

    float cutoff{ 8000.0f }; // Typical for vintage delays
    juce::dsp::IIR::Filter<float> filter;
};

class ReconstructionFilter : public AudioBlock
{
public:
    ReconstructionFilter() = default;

    void prepare(double sampleRate)
    {
        filter.prepare({ sampleRate, 1, 1 });
        updateCoefficients(sampleRate);
    }

    float process(float sample) override
    {
        return filter.processSample(sample);
    }

    void reset() override
    {
        filter.reset();
    }

    void setCutoff(float newCutoff)
    {
        cutoff = newCutoff;
    }

private:
    void updateCoefficients(double sampleRate)
    {
        *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, cutoff);
    }

    float cutoff{ 8000.0f };
    juce::dsp::IIR::Filter<float> filter;
};

class OutputStage : public AudioBlock
{
public:
    OutputStage() = default;

    float process(float sample) override
    {
        return sample * outputGain;
    }

    void reset() override
    {
    }

    void setOutputGain(float newGain)
    {
        outputGain = newGain;
    }

private:
    float outputGain{ 1.0f };
};
