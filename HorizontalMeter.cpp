#include "HorizontalMeter.h"

HorizontalMeter::HorizontalMeter()
{
    startTimerHz(30); // Update 30 times per second
}

HorizontalMeter::~HorizontalMeter()
{
    stopTimer();
}

void HorizontalMeter::setLevel(float newLevel)
{
    // Clamp the level between 0.0 and 1.0
    level = juce::jlimit(0.0f, 1.0f, newLevel);
    repaint();
}

void HorizontalMeter::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds();
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(bounds);

    g.setColour(juce::Colours::limegreen);
    int meterWidth = static_cast<int>(bounds.getWidth() * level);
    g.fillRect(bounds.removeFromLeft(meterWidth));
}

void HorizontalMeter::resized()
{
}

void HorizontalMeter::timerCallback()
{
    // Gradually reduce the level to simulate signal decay
    if (level > 0.0f)
    {
        level -= decayRate;
        if (level < 0.0f)
            level = 0.0f;

        repaint();
    }
}
