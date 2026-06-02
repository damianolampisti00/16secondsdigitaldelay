#pragma once

#include <cmath>

class ClockArtifacts
{
public:
    void prepare(double sr) { sampleRate = sr; }

    float processClock(float freq)
    {
        // Add simple wow/flutter using a slow sine wave
        phase += 2.0 * 3.14159265359 * 0.5 / sampleRate; // 0.5 Hz wow
        if (phase > 2.0 * 3.14159265359) phase -= 2.0 * 3.14159265359;
        
        float modulation = 1.0f + wowDepth * std::sin(phase);
        return freq * modulation;
    }

    void setWowDepth(float depth) { wowDepth = depth; }

private:
    double sampleRate{ 44100.0 };
    double phase{ 0.0 };
    float wowDepth{ 0.005f };
    float jitterAmount{ 0.0f };
    float flutterDepth{ 0.0f };
};

class ClockGenerator
{
public:
    ClockGenerator() {}

    void prepare(double sr) 
    { 
        sampleRate = sr; 
        artifacts.prepare(sr);
    }

    bool tick()
    {
        float currentFreq = artifacts.processClock(clockFrequency);
        phase += currentFreq / sampleRate;
        if (phase >= 1.0)
        {
            phase -= 1.0;
            return true; // Clock tick generated!
        }
        return false;
    }

    void setFrequency(float freq) { clockFrequency = freq; }
    float getCurrentFrequency() const { return clockFrequency; }

private:
    double sampleRate{ 44100.0 };
    float clockFrequency{ 32000.0f }; // Typical vintage sample rate approx
    double phase{ 0.0 };

    ClockArtifacts artifacts;
};
