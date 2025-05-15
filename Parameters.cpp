#include "Parameters.h"

template<typename T>
void CastParameter(const juce::AudioProcessorValueTreeState& apvts, const juce::ParameterID& paramID, T& destination)
{
    destination = dynamic_cast<T>(apvts.getParameter(paramID.getParamID()));
}

Parameters::Parameters(juce::AudioProcessor& processor)
    : m_APVTS{ processor, nullptr, "APVTS", createParameterLayout() },
    m_BitrateParam{},
    m_ArtifactsParam{},
    m_NoiseLevelParam{}
{
    CastParameter(m_APVTS, BitrateParamID, m_BitrateParam);
    CastParameter(m_APVTS, ArtifactsParamID, m_ArtifactsParam);
    CastParameter(m_APVTS, NoiseLevelParamID, m_NoiseLevelParam);
}

void Parameters::PrepareToPlay(double sampleRate)
{
    const double smoothDuration = 0.001;
    //DBG("Smoothing Time: 0.01 seconds for sample rate: " << sampleRate);

    m_BitrateSmoother.reset(sampleRate, smoothDuration);
    m_ArtifactsSmoother.reset(sampleRate, smoothDuration);
    m_NoiseLevelSmoother.reset(sampleRate, smoothDuration);

    // Initialize smoothed values to the current parameter values
    m_BitrateSmoother.setCurrentAndTargetValue(m_BitrateParam->get());
    m_ArtifactsSmoother.setCurrentAndTargetValue(m_ArtifactsParam->get());
    m_NoiseLevelSmoother.setCurrentAndTargetValue(m_NoiseLevelParam->get());
}

void Parameters::Reset()
{
    m_Bitrate = 0.f;
    m_Artifacts = 0.f;
    m_NoiseLevel = 0.f;

    m_BitrateSmoother.setCurrentAndTargetValue(m_BitrateParam->get());
    m_ArtifactsSmoother.setCurrentAndTargetValue(m_ArtifactsParam->get());
    m_NoiseLevelSmoother.setCurrentAndTargetValue(m_NoiseLevelParam->get());
}

void Parameters::Update()
{
    // Parameter Update
    m_BitrateSmoother.setTargetValue(m_BitrateParam->get());
    m_ArtifactsSmoother.setTargetValue(m_ArtifactsParam->get());
    m_NoiseLevelSmoother.setTargetValue(m_NoiseLevelParam->get());
}

void Parameters::Smoothen()
{
    // Smooth each parameter
    m_Bitrate = m_BitrateSmoother.getNextValue();
    m_Artifacts = m_ArtifactsSmoother.getNextValue();
    m_NoiseLevel = m_NoiseLevelSmoother.getNextValue();

    //DBG("Smoothed Bitrate: " << m_Bitrate);
    //DBG("Smoothed Artifacts: " << m_Artifacts);
    //DBG("Smoothed Noise Level: " << m_NoiseLevel);

}

float Parameters::GetBitrate() const
{
    //DBG("Bitrate parameter value: " << m_BitrateParam->get());
    return m_Bitrate;
}

float Parameters::GetArtifacts() const
{
    //DBG("Artifacts parameter value: " << m_ArtifactsParam->get());
    return m_Artifacts;
}

float Parameters::GetNoiseLevel() const
{
    //DBG("Noise Level parameter value: " << m_NoiseLevelParam->get());
    return m_NoiseLevel;
}

juce::AudioProcessorValueTreeState& Parameters::GetAPVTS()
{
    return m_APVTS;
}

juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout parameters;

    parameters.add(std::make_unique<juce::AudioParameterFloat>(
        BitrateParamID.getParamID(), "Bitrate", juce::NormalisableRange<float>{ MinBitrate, MaxBitrate, 1.f }, 128.f));

    parameters.add(std::make_unique<juce::AudioParameterFloat>(
        ArtifactsParamID.getParamID(), "Artifacts", juce::NormalisableRange<float>{ 4.f, 24.f, 0.1f, 0.5f }, 16.f));

    parameters.add(std::make_unique<juce::AudioParameterFloat>(
        NoiseLevelParamID.getParamID(), "Noise Level", juce::NormalisableRange<float>{ 0.f, 1.f, 0.01f }, 0.5f));

    return parameters;
}
