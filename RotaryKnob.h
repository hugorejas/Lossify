#pragma once
#include <JuceHeader.h>

class RotaryKnob : public juce::Component
{
public:
	RotaryKnob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& paramID);

	void resized() override;

private:
	juce::Slider m_Slider;
	juce::Label m_Label;
	juce::AudioProcessorValueTreeState::SliderAttachment m_Attachment;

};