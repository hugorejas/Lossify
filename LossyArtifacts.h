#pragma once
#include <JuceHeader.h>
#include "SimpleLowPass.h"
#include "DelayLine.h"
#include <vector>

class LossyArtifacts
{
public:
    LossyArtifacts();                      
    ~LossyArtifacts();                      

    void prepare(double sampleRate);       
    void reset();                        
    void process(juce::AudioBuffer<float>& buffer, float bitrate, float artifacts, float noiseLevel);

    float getLeftChannelRMS() const { return leftChannelRMS; }
    float getRightChannelRMS() const { return rightChannelRMS; }

private:

	double sampleRate = 44100.0; // Sample rate of the audio buffer
	float lastBitrate = 0.0f; // cache for bitrate changes

	//Band-splitting filters
	juce::dsp::LinkwitzRileyFilter<float> lowPassFilter1;
	juce::dsp::LinkwitzRileyFilter<float> highPassFilter1;
	juce::dsp::LinkwitzRileyFilter<float> lowPassFilter2;
	juce::dsp::LinkwitzRileyFilter<float> highPassFilter2;

	//Band-splitting buffers
    juce::AudioBuffer<float> lowBandBuffer;
	juce::AudioBuffer<float> midBandBuffer;
	juce::AudioBuffer<float> highBandBuffer;

    // Global high-cut filter
    juce::dsp::LinkwitzRileyFilter<float> highCutFilter;

	// Delay lines for artifacts
	std::vector<DelayLine> delayLines;

    // RMS Calc
    float calculateChannelRMS(const juce::AudioBuffer<float>& buffer, int channel);

    // Quantization
    float applyQuantization(float sample, float bitDepth);

    // Noise Injection
    float generateNoise(float noiseLevel);

    // RMS values for LED meter
    float leftChannelRMS = 0.0f; // RMS for the left channel
    float rightChannelRMS = 0.0f; // RMS for the right channel

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LossyArtifacts)
};
