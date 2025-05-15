#pragma once

#include <JuceHeader.h>

class HorizontalMeter : public juce::Component, public juce::Timer
{
public:
    HorizontalMeter();
    ~HorizontalMeter() override;

    void setLevel(float newLevel); // Set the current meter level (0.0 to 1.0)

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    float level = 0.0f; // Current level
    float decayRate = 0.01f; // Rate at which the meter decays when no signal

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HorizontalMeter)
};
