#pragma once
#include <JuceHeader.h>

class Parameters
{
public:

    Parameters(juce::AudioProcessor& processor);

    void PrepareToPlay(double sampleRate);
    void Reset();
    void Update();
    void Smoothen();

public:
    // Parameter IDs
    inline static const juce::ParameterID BitrateParamID{ "Bitrate" };
    inline static const juce::ParameterID ArtifactsParamID{ "Artifacts" };
    inline static const juce::ParameterID NoiseLevelParamID{ "NoiseLevel" };

    // Min-Max Values
    inline static const float MinBitrate{ 32.f };
    inline static const float MaxBitrate{ 320.f };

public:
    // Getters
    float GetBitrate() const;
    float GetArtifacts() const;
    float GetNoiseLevel() const;
    juce::AudioProcessorValueTreeState& GetAPVTS();

private:
    juce::AudioProcessorValueTreeState m_APVTS;

    // Bitrate
    juce::AudioParameterFloat* m_BitrateParam;
    juce::LinearSmoothedValue<float> m_BitrateSmoother;
    float m_Bitrate;

    // Artifacts
    juce::AudioParameterFloat* m_ArtifactsParam;
    juce::LinearSmoothedValue<float> m_ArtifactsSmoother;
    float m_Artifacts;

    // Noise Level
    juce::AudioParameterFloat* m_NoiseLevelParam;
    juce::LinearSmoothedValue<float> m_NoiseLevelSmoother;
    float m_NoiseLevel;

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};
