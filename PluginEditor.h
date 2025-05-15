#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "RotaryKnob.h"
#include "HorizontalMeter.h"

//==============================================================================
/**
*/
class LossifyAudioProcessorEditor  : public juce::AudioProcessorEditor,
	                                 private juce::Timer
{
public:
    LossifyAudioProcessorEditor (LossifyAudioProcessor&);
    ~LossifyAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    LossifyAudioProcessor& audioProcessor;

	RotaryKnob m_BitrateKnob;
	RotaryKnob m_ArtifactsKnob;
	RotaryKnob m_NoiseLevelKnob;

	HorizontalMeter m_MeterL;
	HorizontalMeter m_MeterR;

    // Logo image size and position
    const int logoWidth = 100;
    const int logoHeight = 100;
    const int logoX = 10;
    const int logoY = 10;

	void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LossifyAudioProcessorEditor)
};
