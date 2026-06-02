#pragma once

#include <JuceHeader.h>

class AudioBlock
{
public:
    virtual ~AudioBlock() = default;
    virtual float process(float sample) = 0;
    virtual void reset() = 0;
};

class Converter
{
public:
    virtual ~Converter() = default;
    virtual float process(float sample) = 0;

protected:
    int bitDepth{ 12 };
    float referenceVoltage{ 1.0f };
};

class Compander
{
public:
    virtual ~Compander() = default;
    virtual float process(float sample) = 0;

protected:
    // Forward declarations of components that would be owned
    class EnvelopeDetector* detector{ nullptr };
    class GainCell* gainCell{ nullptr };
};

class PlaybackStrategy
{
public:
    virtual ~PlaybackStrategy() = default;
    virtual uint32_t advancePointer(uint32_t current, uint32_t size) = 0;
};
