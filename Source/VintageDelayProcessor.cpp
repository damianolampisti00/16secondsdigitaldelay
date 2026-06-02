#include "VintageDelayProcessor.h"

VintageDelayProcessor::VintageDelayProcessor()
{
}

VintageDelayProcessor::~VintageDelayProcessor()
{
}

void VintageDelayProcessor::prepareToPlay(double sr, int samplesPerBlock)
{
    sampleRate = sr;
    
    compressor.prepare(sr);
    expander.prepare(sr);
    
    antiAliasFilter.prepare(sr);
    antiAliasFilter.setCutoff(8000.0f);
    
    reconFilter.prepare(sr);
    reconFilter.setCutoff(8000.0f);

    // 16 seconds max delay time
    memoryController.prepare(16.0, sr);

    clock.prepare(sr);
    dryWetter.prepareToPlay(sr, samplesPerBlock);
}

void VintageDelayProcessor::releaseResources()
{
    dryWetter.releaseResources();
}

void VintageDelayProcessor::processBlock(juce::AudioSampleBuffer& buffer)
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    dryWetter.copyDrySignal(buffer);

    auto* channelDataL = buffer.getWritePointer(0);
    auto* channelDataR = numChannels > 1 ? buffer.getWritePointer(1) : nullptr;

    for (int i = 0; i < numSamples; ++i)
    {
        // Mono mix down if stereo
        float inputSample = channelDataL[i];
        if (channelDataR != nullptr)
        {
            inputSample = (inputSample + channelDataR[i]) * 0.5f;
        }

        // 1. Analog Input Path
        float processed = inputStage.process(inputSample);
        processed = compressor.process(processed);
        processed = antiAliasFilter.process(processed);

        // 2. Converters & Memory
        // Convert to 12-bit float approx
        float toMemory = adc.process(processed);
        
        // Memory Controller process (using 12-bit float in [-1, 1] mapped to int16 range)
        int16_t intSample = static_cast<int16_t>(toMemory * 2047.0f);
        int16_t memOut = memoryController.process(intSample);
        
        float fromMemory = dac.process(static_cast<float>(memOut));

        // 3. Analog Output Path
        processed = reconFilter.process(fromMemory);
        processed = expander.process(processed);
        processed = outputStage.process(processed);

        // Output routing (wet only, drywet merges later)
        channelDataL[i] = processed;
        if (channelDataR != nullptr)
        {
            channelDataR[i] = processed;
        }
    }

    dryWetter.merge(buffer);
}

void VintageDelayProcessor::setLoopState(LoopState state)
{
    memoryController.setState(state);
}

void VintageDelayProcessor::setReverse(bool reverse)
{
    memoryController.setReverse(reverse);
}

void VintageDelayProcessor::setInputGain(float gain)
{
    inputStage.setGain(gain);
}

void VintageDelayProcessor::setOutputGain(float gain)
{
    outputStage.setOutputGain(gain);
}

void VintageDelayProcessor::setDryWet(float dw)
{
    dryWetter.setDryWetRatio(dw);
}

void VintageDelayProcessor::setFeedback(float fb)
{
    memoryController.setFeedback(fb);
}
