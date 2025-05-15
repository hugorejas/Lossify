#include "DelayLine.h"
#include <algorithm>
#include <cmath>

DelayLine::DelayLine()
{
}

void DelayLine::setMaximumDelayInSamples(int maxDelayInSamples)
{
    m_BufferLength = maxDelayInSamples;
    m_Buffer.resize(m_BufferLength, 0.0f); // Initialize buffer with zeros
    reset(); // Reset indices and state
}

void DelayLine::reset()
{
    std::fill(m_Buffer.begin(), m_Buffer.end(), 0.0f); // Clear the buffer
    m_WriteIndex = 0;
    m_ReadIndex = 0.0f;
}

void DelayLine::write(float sample)
{
    // Write the sample to the current write index
    m_Buffer[m_WriteIndex] = sample;

    // Increment and wrap the write index
    m_WriteIndex = wrapIndex(m_WriteIndex + 1);
}

float DelayLine::read(float delayInSamples) const
{
    // Calculate the fractional read position based on the desired delay
    float fractionalReadIndex = static_cast<float>(m_WriteIndex) - delayInSamples;

    // Wrap the fractional index to ensure it stays within bounds
    if (fractionalReadIndex < 0.0f)
        fractionalReadIndex += static_cast<float>(m_BufferLength);

    // Use interpolation to read the delayed sample
    return interpolate(fractionalReadIndex);
}

void DelayLine::setDelay(float delayInSamples)
{
    // Set the read index based on the desired delay
    m_ReadIndex = static_cast<float>(m_WriteIndex) - delayInSamples;

    // Wrap the read index if necessary
    if (m_ReadIndex < 0.0f)
        m_ReadIndex += static_cast<float>(m_BufferLength);
}

int DelayLine::wrapIndex(int index) const
{
    // Wrap the index around the buffer length
    return index % m_BufferLength;
}

float DelayLine::interpolate(float index) const
{
    // Integer part of the index
    int intIndex = static_cast<int>(index);

    // Fractional part of the index
    float frac = index - static_cast<float>(intIndex);

    int nextIndex = wrapIndex(intIndex + 1);

    float sample1 = m_Buffer[intIndex];
    float sample2 = m_Buffer[nextIndex];

    // Linearly interpolation between the two samples
    return sample1 + frac * (sample2 - sample1);
}
