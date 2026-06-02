#pragma once

#include <JuceHeader.h>

class DryWet
{
public:
	DryWet(float defaultDryWet = 1.0f)
	{
		dryWetRatio = defaultDryWet;

		wetLevel.setCurrentAndTargetValue(0.0f);
		dryLevel.setCurrentAndTargetValue(0.0f);
	}

	~DryWet() {}

	void prepareToPlay(const double sr, const double maxNumSamples)
	{
		drySignal.setSize(2, maxNumSamples);

		wetLevel.reset(sr, 0.02);
		dryLevel.reset(sr, 0.02);

		updateInternalState();
	}

	void releaseResources()
	{
		drySignal.setSize(0, 0);
	}

	void copyDrySignal(const AudioBuffer<float>& srcBuffer)
	{
		auto numCh = srcBuffer.getNumChannels();
		auto numSmp = srcBuffer.getNumSamples();

		for (int ch = 0; ch < numCh; ++ch)
			drySignal.copyFrom(ch, 0, srcBuffer, ch, 0, numSmp);
	}

	void merge(AudioBuffer<float>& destBuffer)
	{
		auto numCh = destBuffer.getNumChannels();
		auto numSmp = destBuffer.getNumSamples();

		wetLevel.applyGain(destBuffer, numSmp);
		dryLevel.applyGain(drySignal, numSmp);

		for (int ch = 0; ch < numCh; ++ch)
			destBuffer.addFrom(ch, 0, drySignal, ch, 0, numSmp);
	}

	void setDryWetRatio(const float newValue)
	{
		dryWetRatio = newValue;
		updateInternalState();
	}

private:
	void updateInternalState()
	{
		wetLevel.setTargetValue(sqrt(dryWetRatio));
		dryLevel.setTargetValue(sqrt(1.0f - dryWetRatio));
	}

	float dryWetRatio;
	SmoothedValue<float,ValueSmoothingTypes::Linear> dryLevel;
	SmoothedValue<float,ValueSmoothingTypes::Linear> wetLevel;

	AudioBuffer<float> drySignal;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DryWet)
};