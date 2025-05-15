#pragma once

class SimpleLowPass
{
public:
    SimpleLowPass() = default; 
    ~SimpleLowPass() = default;

    void prepare(double sampleRate, float initialCutoff); // Prepare the filter
    void reset();                                         // Reset the filter
    void setCutoffFrequency(float cutoff);               // Set cutoff frequency
    float processSample(float sample);                   // Process a single sample

private:
    double sampleRate = 44100.0; // Default sample rate
    float cutoffFrequency = 1000.0f; // Default cutoff frequency

    // Coefficients for two cascaded 2nd-order filters
    float b0_1 = 0.0f, b1_1 = 0.0f, b2_1 = 0.0f, a1_1 = 0.0f, a2_1 = 0.0f; // Filter 1
    float b0_2 = 0.0f, b1_2 = 0.0f, b2_2 = 0.0f, a1_2 = 0.0f, a2_2 = 0.0f; // Filter 2

    // State variables for two cascaded filters
    float z1_1 = 0.0f, z2_1 = 0.0f; // Filter 1 states
    float z1_2 = 0.0f, z2_2 = 0.0f; // Filter 2 states

    void updateCoefficients(); // Internal function to update filter coefficients
};
