#pragma once

#include <JuceHeader.h>

namespace Colors
{
	namespace RotaryKnob
	{
		static const juce::Colour KnobLineFillColor{ 159,211,0 };
	}
}

class Fonts
{
public:
	Fonts() = delete;

	static juce::Font GetFont(float height = 16.f);
	static juce::Font GetNewFont(float height = 16.f);

private:
	static const juce::Typeface::Ptr m_Typeface;
	static const juce::Typeface::Ptr m_NewTypeface;


};

class RotaryKnobLookAndFeel : public juce::LookAndFeel_V4
{

public:
	RotaryKnobLookAndFeel();

	static RotaryKnobLookAndFeel* Get()
	{
		static RotaryKnobLookAndFeel instance{};
		return &instance;
	}

	void drawRotarySlider(juce::Graphics& graphics, int x, int y, int width, int height,
		float sliderPosProportional, float rotaryStartAngle,
		float rotaryEndAngle, juce::Slider& slider) override;

	juce::Font getLabelFont(juce::Label& label) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnobLookAndFeel)
};