#pragma once
#include <JuceHeader.h>

inline void protectYourEars(juce::AudioBuffer<float>& buffer)
{
	bool warningReady = true;

	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		float* pChannelData = buffer.getWritePointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			float sampleData = pChannelData[sample];
			bool mute = false;

			//Check for Inf
			if (std::isinf(sampleData))
			{
				DBG("PROTECT YOUR EARS: ERROR - INF detected in audio buffer, silencing...");
				mute = true;
			}
			//Check for nan
			else if (std::isnan(sampleData))
			{
				DBG("PROTECT YOUR EARS: ERROR - NAN detected in audio buffer, silencing...");
				mute = true;
			}
			//Check for > 2 -> mute
			else if (sampleData > 2.f || sampleData < -2.f)
			{
				DBG("PROTECT YOUR EARS: ERROR - Sample out of range, silencing...");
				mute = true;
			}
			//Check for > 1 -> give first warning
			else if (sampleData > 1.f || sampleData < -1.f)
			{
				if (warningReady)
				{
					DBG("PROTECT YOUR EARS: WARNING - Sample out of range: " << sampleData << "...");
					warningReady = false; //Makes it so we don't spam this every sample in the buffer
				}
			}

			if (mute)
			{
				buffer.clear();
				return;
			}
		}
	}
}