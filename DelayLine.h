#pragma once
#include <vector>
#include <memory>

class DelayLine
{
public:
    DelayLine();

    //Delay line initialized with a maximum delay size in samples
    void setMaximumDelayInSamples(int maxDelayInSamples);

    void reset();

    void write(float sample);

    //supports interpolation
    float read(float delayInSamples) const;

    //Set specific delay time in samples
    void setDelay(float delayInSamples);

private:
    std::vector<float> m_Buffer; 
    int m_BufferLength = 0;      
    int m_WriteIndex = 0;        
    float m_ReadIndex = 0.0f;   

    // Wrap the buffer index to ensure it stays within bounds
    int wrapIndex(int index) const;

    // Linear interpolation for fractional delay
    float interpolate(float index) const;
};
