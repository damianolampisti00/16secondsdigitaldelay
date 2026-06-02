#pragma once

#include "DSPInterfaces.h"
#include <JuceHeader.h>
#include <vector>
#include <algorithm>

enum class LoopState
{
    Idle,
    CountIn,
    Recording,
    Playing,
    Overdubbing
};

class LoopMemory
{
public:
    void allocate(uint32_t sizeInSamples)
    {
        memory.resize(sizeInSamples, 0);
        memorySize = sizeInSamples;
    }

    void writeSample(uint32_t index, int16_t sample)
    {
        if (index < memorySize)
            memory[index] = sample;
    }

    int16_t readSample(uint32_t index) const
    {
        if (index < memorySize)
            return memory[index];
        return 0;
    }

    void clear()
    {
        std::fill(memory.begin(), memory.end(), 0);
    }

    uint32_t getLength() const { return memorySize; }

private:
    std::vector<int16_t> memory;
    uint32_t memorySize{ 0 };
};

class ForwardPlayback : public PlaybackStrategy
{
public:
    uint32_t advancePointer(uint32_t current, uint32_t size) override
    {
        if (size == 0) return 0;
        return (current + 1) % size;
    }
};

class ReversePlayback : public PlaybackStrategy
{
public:
    uint32_t advancePointer(uint32_t current, uint32_t size) override
    {
        if (size == 0) return 0;
        if (current == 0) return size - 1;
        return current - 1;
    }
};

class MemoryController
{
public:
    MemoryController()
    {
        forwardStrategy = std::make_unique<ForwardPlayback>();
        reverseStrategy = std::make_unique<ReversePlayback>();
        currentStrategy = forwardStrategy.get();
    }

    void prepare(double maxSeconds, double maxSampleRate)
    {
        loopMemory.allocate(static_cast<uint32_t>(maxSeconds * maxSampleRate));
        readPointer = 0;
        writePointer = 0;
        loopLength = 0;
    }

    int16_t process(int16_t inputSample)
    {
        int16_t outputSample = 0;

        if (currentState == LoopState::Recording)
        {
            loopMemory.writeSample(writePointer, inputSample);
            outputSample = inputSample; // Monitor input
            
            loopLength = std::max(loopLength, writePointer + 1);
            advancePointers();
        }
        else if (currentState == LoopState::Playing)
        {
            outputSample = loopMemory.readSample(readPointer);
            advancePointers();
        }
        else if (currentState == LoopState::Overdubbing)
        {
            int16_t existingSample = loopMemory.readSample(readPointer);
            // Simple mix for overdub with feedback
            int32_t decayed = static_cast<int32_t>(static_cast<float>(existingSample) * feedbackAmount);
            int32_t mix = decayed + static_cast<int32_t>(inputSample);
            mix = std::clamp(mix, -2048, 2047); 
            
            loopMemory.writeSample(writePointer, static_cast<int16_t>(mix));
            outputSample = static_cast<int16_t>(mix);
            advancePointers();
        }
        else
        {
            // Idle
            outputSample = inputSample; 
        }

        return outputSample;
    }

    void setState(LoopState newState)
    {
        if (currentState == LoopState::Idle && newState == LoopState::Recording)
        {
            loopMemory.clear();
            readPointer = 0;
            writePointer = 0;
            loopLength = 0;
        }
        else if (newState == LoopState::Playing && currentState == LoopState::Recording)
        {
            // Lock the loop length
            readPointer = 0;
            writePointer = 0;
        }
        
        currentState = newState;
    }

    void setReverse(bool reverse)
    {
        isReverse = reverse;
        currentStrategy = isReverse ? reverseStrategy.get() : forwardStrategy.get();
    }

    void setFeedback(float fb) { feedbackAmount = fb; }

    LoopState getState() const { return currentState; }

private:
    void advancePointers()
    {
        uint32_t limit = (currentState == LoopState::Recording) ? loopMemory.getLength() : loopLength;
        if (limit > 0)
        {
            readPointer = currentStrategy->advancePointer(readPointer, limit);
            writePointer = currentStrategy->advancePointer(writePointer, limit);
        }
    }

    LoopState currentState{ LoopState::Idle };
    LoopMemory loopMemory;
    
    uint32_t readPointer{ 0 };
    uint32_t writePointer{ 0 };
    uint32_t loopLength{ 0 };
    bool isReverse{ false };
    float feedbackAmount{ 0.5f };

    std::unique_ptr<PlaybackStrategy> forwardStrategy;
    std::unique_ptr<PlaybackStrategy> reverseStrategy;
    PlaybackStrategy* currentStrategy{ nullptr };
};
