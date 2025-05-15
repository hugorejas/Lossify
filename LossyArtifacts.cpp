#include "LossyArtifacts.h"
#include <algorithm>

LossyArtifacts::LossyArtifacts()
{
}

LossyArtifacts::~LossyArtifacts()
{
}

void LossyArtifacts::prepare(double sampleRate)
{
	this->sampleRate = sampleRate;

	// Prepare band-splitting filters
	juce::dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = 512; // Match buffer size
	spec.numChannels = 2;        // Stereo processing

	lowPassFilter1.prepare(spec);
	lowPassFilter1.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
	lowPassFilter1.setCutoffFrequency(200.f);

	highPassFilter1.prepare(spec);
	highPassFilter1.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
	highPassFilter1.setCutoffFrequency(200.f);

	lowPassFilter2.prepare(spec);
	lowPassFilter2.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
	lowPassFilter2.setCutoffFrequency(2000.0f);

	highPassFilter2.prepare(spec);
	highPassFilter2.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
	highPassFilter2.setCutoffFrequency(2000.0f);

	// Prepare high-cut filter
	highCutFilter.prepare(spec);
	highCutFilter.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
	highCutFilter.setCutoffFrequency(16000.0f);

	// Prepare DelayLines
	delayLines.clear();
	delayLines.resize(spec.numChannels);
	for (auto& delayLine : delayLines)
	{
		delayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.1f)); // 100 ms max delay
		delayLine.reset();
	}

	// Resize buffers for each band
	lowBandBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
	midBandBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
	highBandBuffer.setSize(spec.numChannels, spec.maximumBlockSize);

	//for the meter
	leftChannelRMS = 0.0f;
	rightChannelRMS = 0.0f;

}

void LossyArtifacts::reset()
{

	lowPassFilter1.reset();
	highPassFilter1.reset();
	lowPassFilter2.reset();
	highPassFilter2.reset();

	highCutFilter.reset();

	for (auto& delayLine : delayLines)
		delayLine.reset();

	//for the meter
	leftChannelRMS = 0.0f;
	rightChannelRMS = 0.0f;
}

void LossyArtifacts::process(juce::AudioBuffer<float>& buffer, float bitrate, float artifacts, float noiseLevel)
{
	//Old Implementation
	/*
	//Map bitrate to cutoff frequency
	float cutoffFrequency = juce::jmap(bitrate, 32.0f, 320.0f, 8000.0f, 16000.0f);

	DBG("Bitrate: " << bitrate << ", Cutoff Frequency: " << cutoffFrequency);

	// Update cutoff frequency if bitrate has changed
	if (lastBitrate != bitrate)
	{
		lastBitrate = bitrate;
		lowPassFilter.setCutoffFrequency(cutoffFrequency);
	}
	
	float leftRMS = calculateChannelRMS(buffer, 0);
	float rightRMS = buffer.getNumChannels() > 1 ? calculateChannelRMS(buffer, 1) : leftRMS;

	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			// Temporal Smearing
			applySmearing(channelData[sample], channel);

			// Bandlimiting
			//channelData[sample] = lowPassFilter.processSample(channelData[sample]);

			
			// Quantization
			channelData[sample] = applyQuantization(channelData[sample], artifacts);

			// Noise Injection
			float noise = generateNoise(noiseLevel * (channel == 0 ? leftRMS : rightRMS));
			channelData[sample] += noise;
			


		}
	}
	*/


	//New band Splitting Implementation
	
	// Map bitrate to cutoff frequency
	float cutoffFrequency = juce::jmap(bitrate, 32.0f, 320.0f, 8000.0f, 16000.0f);

	//DBG("Bitrate: " << bitrate << ", Cutoff Frequency: " << cutoffFrequency);

	// Update cutoff frequency if bitrate has changed
	if (lastBitrate != bitrate)
	{
		lastBitrate = bitrate;
		highCutFilter.setCutoffFrequency(cutoffFrequency);
	}

	// Delay line settings
	float delayTimeInMs = 20.0f; // 20 ms delay
	float delayTimeInSamples = (delayTimeInMs / 1000.0f) * static_cast<float>(sampleRate);
	float feedbackAmount = 0.3f; // Feedback control (40%)
	float delayMix = 0.3f;       // Wet/dry mix (50%)

	// Calculate RMS for each channel
	float leftRMS = calculateChannelRMS(buffer, 0);
	float rightRMS = buffer.getNumChannels() > 1 ? calculateChannelRMS(buffer, 1) : leftRMS;

	// Copy input buffer into band buffers
	lowBandBuffer.makeCopyOf(buffer);
	midBandBuffer.makeCopyOf(buffer);
	highBandBuffer.makeCopyOf(buffer);

	// Process low band
	juce::dsp::AudioBlock<float> lowBlock(lowBandBuffer);
	lowPassFilter1.process(juce::dsp::ProcessContextReplacing<float>(lowBlock));
	//highPassFilter1.process(juce::dsp::ProcessContextReplacing<float>(lowBlock));

	// Process mid band
	juce::AudioBuffer<float> tempBuffer; // Temporary buffer for intermediate processing
	tempBuffer.setSize(midBandBuffer.getNumChannels(), midBandBuffer.getNumSamples());
	tempBuffer.makeCopyOf(midBandBuffer); // Copy original mid-band data into temp buffer

	// Process the first filter (high-pass) into the temp buffer
	juce::dsp::AudioBlock<float> tempBlock(tempBuffer);
	highPassFilter1.process(juce::dsp::ProcessContextReplacing<float>(tempBlock));

	// Process the second filter (low-pass) into the original mid-band buffer
	juce::dsp::AudioBlock<float> midBlock(midBandBuffer);
	lowPassFilter2.process(juce::dsp::ProcessContextReplacing<float>(midBlock));

	// Process high band
	juce::dsp::AudioBlock<float> highBlock(highBandBuffer);
	highPassFilter2.process(juce::dsp::ProcessContextReplacing<float>(highBlock));

	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		auto* lowBand = lowBandBuffer.getWritePointer(channel);
		auto* midBand = midBandBuffer.getWritePointer(channel);
		auto* highBand = highBandBuffer.getWritePointer(channel);

		float channelRMS = (channel == 0) ? leftRMS : rightRMS;

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			// Add scaled noise first
			float noise = generateNoise(noiseLevel * channelRMS * 0.5f);
			lowBand[sample] += noise * 0.2f;
			midBand[sample] += noise * 0.5f;
			highBand[sample] += noise * 0.8f;

			// Apply temporal smearing to mid band
			//applySmearing(midBand[sample], channel);

			// Apply quantization to mid and high bands
			midBand[sample] = juce::jlimit(-1.0f, 1.0f, applyQuantization(midBand[sample], artifacts));
			highBand[sample] = juce::jlimit(-1.0f, 1.0f, applyQuantization(highBand[sample], artifacts));
		}
	}

	// Recombine bands into the output buffer
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		auto* output = buffer.getWritePointer(channel);
		auto* lowBand = lowBandBuffer.getReadPointer(channel);
		auto* midBand = midBandBuffer.getReadPointer(channel);
		auto* highBand = highBandBuffer.getReadPointer(channel);

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			// Combine processed bands
			float combinedSample = lowBand[sample] + midBand[sample] + highBand[sample];
			output[sample] = combinedSample;
		}
	}

	// Apply delay line to the entire buffer
	for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
	{
		auto* channelData = buffer.getWritePointer(channel);
		auto& delayLine = delayLines[channel];

		for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
		{
			// Read the delayed sample
			float delayedSample = delayLine.read(delayTimeInSamples);

			// Add feedback to the delay line
			float drySample = channelData[sample];
			float feedbackSample = drySample + (delayedSample * feedbackAmount);
			delayLine.write(feedbackSample);

			// Mix the dry and delayed signals
			channelData[sample] = juce::jlimit(-1.0f, 1.0f, drySample * (1.0f - delayMix) + delayedSample * delayMix);
		}
	}

	// Apply the high-cut filter to the entire buffer
	juce::dsp::AudioBlock<float> outputBlock(buffer);
	highCutFilter.process(juce::dsp::ProcessContextReplacing<float>(outputBlock));

	// Update RMS values for the meter
	leftChannelRMS = calculateChannelRMS(buffer, 0);
	if (buffer.getNumChannels() > 1)
		rightChannelRMS = calculateChannelRMS(buffer, 1);
	else
		rightChannelRMS = leftChannelRMS; // Mono case

}

float LossyArtifacts::calculateChannelRMS(const juce::AudioBuffer<float>& buffer, int channel)
{
	const auto* channelData = buffer.getReadPointer(channel);
	float sum = 0.0f;

	for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
	{
		sum += channelData[sample] * channelData[sample];
	}


	return std::sqrt(sum / buffer.getNumSamples());
}

float LossyArtifacts::applyQuantization(float sample, float bitDepth)
{
	// Clamp bit depth to valid range
	bitDepth = juce::jlimit(4.0f, 24.0f, bitDepth);

	// Precompute quantization steps

	float steps = std::pow(2.0f, bitDepth);

	// Apply quantization
	return std::round(sample * steps) / steps;
}

float LossyArtifacts::generateNoise(float noiseLevel)
{
	noiseLevel = juce::jlimit(0.0f, 1.0f, noiseLevel); // Clamp noise level between 0 and 1

	return (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * noiseLevel;
}
