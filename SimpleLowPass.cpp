#include "SimpleLowPass.h"
#include <JuceHeader.h>
#include <cmath>

void SimpleLowPass::prepare(double newSampleRate, float initialCutoff)
{
    sampleRate = newSampleRate;
    cutoffFrequency = initialCutoff;
    updateCoefficients();
    reset();
}

void SimpleLowPass::reset()
{
    z1_1 = z2_1 = 0.0f; // Reset states for Filter 1
    z1_2 = z2_2 = 0.0f; // Reset states for Filter 2
}

void SimpleLowPass::setCutoffFrequency(float newCutoff)
{
    cutoffFrequency = newCutoff;
    updateCoefficients();
}

float SimpleLowPass::processSample(float sample)
{
    // First filter stage
    float y1 = b0_1 * sample + z1_1;
    z1_1 = b1_1 * sample - a1_1 * y1 + z2_1;
    z2_1 = b2_1 * sample - a2_1 * y1;

    // Second filter stage
    float y2 = b0_2 * y1 + z1_2;
    z1_2 = b1_2 * y1 - a1_2 * y2 + z2_2;
    z2_2 = b2_2 * y1 - a2_2 * y2;

    return y2;
}

void SimpleLowPass::updateCoefficients()
{
    // Calculate normalized cutoff frequency (omegaC) and related terms
    float omegaC = 2.0f * juce::MathConstants<float>::pi * cutoffFrequency / static_cast<float>(sampleRate);
    float cosOmegaC = std::cos(omegaC);
    float sinOmegaC = std::sin(omegaC);

    float alpha = sinOmegaC / std::sqrt(2.0f); // Q = sqrt(2) for Linkwitz-Riley

    // Coefficients for a 2nd-order Butterworth filter
    float b0 = (1.0f - cosOmegaC) / 2.0f;
    float b1 = 1.0f - cosOmegaC;
    float b2 = (1.0f - cosOmegaC) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosOmegaC;
    float a2 = 1.0f - alpha;

    // Normalize coefficients by a0
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;

    // Assign coefficients for both cascaded filters (identical coefficients)
    b0_1 = b0;
    b1_1 = b1;
    b2_1 = b2;
    a1_1 = a1;
    a2_1 = a2;

    b0_2 = b0;
    b1_2 = b1;
    b2_2 = b2;
    a1_2 = a1;
    a2_2 = a2;
}
