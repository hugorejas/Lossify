#include "RotaryKnob.h"
#include "LookAndFeel.h"

RotaryKnob::RotaryKnob(const juce::String& text, juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& paramID)
	: m_Slider{},
	m_Label{},
	m_Attachment{ apvts, paramID.getParamID(), m_Slider }
{
	m_Slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	m_Slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
	//m_Slider.setSize(100, 100);
	addAndMakeVisible(m_Slider);

	m_Label.setText(text, juce::NotificationType::dontSendNotification);
	m_Label.setJustificationType(juce::Justification::horizontallyCentred);
	m_Label.setFont(juce::Font(14.0f, juce::Font::bold));
	//m_Label.setBorderSize(juce::BorderSize<int>{0, 0, 2, 0});
	m_Label.attachToComponent(&m_Slider, false);
	addAndMakeVisible(m_Label);

	setLookAndFeel(RotaryKnobLookAndFeel::Get());

	setSize(100, 120);

}

void RotaryKnob::resized()
{
	auto bounds = getLocalBounds();

	auto labelArea = bounds.removeFromTop(20);
	m_Label.setBounds(labelArea);

	m_Slider.setBounds(bounds.reduced(5));
}
