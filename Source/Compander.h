#pragma once

#include "DSPInterfaces.h"
#include <cmath>
#include <algorithm>
#include <memory>

class EnvelopeDetector
{
public:
    void prepare(double sr) 
    { 
        sampleRate = sr; 
        setAttackTime(10.0f); // Default ~10ms
        setReleaseTime(50.0f); // Default ~50ms
    }

    float process(float sample)
    {
        float rect = std::abs(sample);
        if (rect > envelope)
            envelope += attackCoeff * (rect - envelope);
        else
            envelope += releaseCoeff * (rect - envelope);
        return envelope;
    }

    void reset() { envelope = 0.0f; }

    void setAttackTime(float ms) { attackCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (ms * 0.001 * sampleRate))); }
    void setReleaseTime(float ms) { releaseCoeff = static_cast<float>(1.0 - std::exp(-1.0 / (ms * 0.001 * sampleRate))); }

private:
    double sampleRate{ 44100.0 };
    float attackCoeff{ 0.01f };
    float releaseCoeff{ 0.001f };
    float envelope{ 0.0f };
};

class GainCell
{
public:
    float computeGain(float env, bool isCompressor)
    {
        float e = std::max(env, 1e-6f); // avoid div by zero
        if (isCompressor)
            return std::sqrt(referenceLevel / e);
        else
            return std::sqrt(e / referenceLevel);
    }

    void setReferenceLevel(float ref) { referenceLevel = ref; }

private:
    float referenceLevel{ 0.1f };
};

class CompressorNE570 : public Compander
{
public:
    CompressorNE570() 
    {
        detectorObj = std::make_unique<EnvelopeDetector>();
        gainCellObj = std::make_unique<GainCell>();
        detector = detectorObj.get();
        gainCell = gainCellObj.get();
    }

    void prepare(double sr) { detector->prepare(sr); }
    
    float process(float sample) override
    {
        float env = detector->process(sample);
        float gain = gainCell->computeGain(env, true);
        return sample * gain;
    }

private:
    std::unique_ptr<EnvelopeDetector> detectorObj;
    std::unique_ptr<GainCell> gainCellObj;
};

class ExpanderNE570 : public Compander
{
public:
    ExpanderNE570() 
    {
        detectorObj = std::make_unique<EnvelopeDetector>();
        gainCellObj = std::make_unique<GainCell>();
        detector = detectorObj.get();
        gainCell = gainCellObj.get();
    }

    void prepare(double sr) { detector->prepare(sr); }

    float process(float sample) override
    {
        float env = detector->process(sample);
        float gain = gainCell->computeGain(env, false);
        return sample * gain;
    }

private:
    std::unique_ptr<EnvelopeDetector> detectorObj;
    std::unique_ptr<GainCell> gainCellObj;
};
