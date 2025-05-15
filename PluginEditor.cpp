#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "LossyArtifacts.h"

//==============================================================================
LossifyAudioProcessorEditor::LossifyAudioProcessorEditor (LossifyAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    m_BitrateKnob{"Bitrate", audioProcessor.GetAPVTS(), Parameters::BitrateParamID},
	m_ArtifactsKnob{ "Artifacts", audioProcessor.GetAPVTS(), Parameters::ArtifactsParamID },
	m_NoiseLevelKnob{ "Noise", audioProcessor.GetAPVTS(), Parameters::NoiseLevelParamID },
	m_MeterL(),
	m_MeterR()
{
	addAndMakeVisible(m_BitrateKnob);
	addAndMakeVisible(m_ArtifactsKnob);
	addAndMakeVisible(m_NoiseLevelKnob);

	addAndMakeVisible(m_MeterL);
	addAndMakeVisible(m_MeterR);

	startTimerHz(30);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (450, 350);
}

LossifyAudioProcessorEditor::~LossifyAudioProcessorEditor()
{
	stopTimer();

}

//==============================================================================
void LossifyAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour(192, 192, 192));

    g.setColour(juce::Colour(0, 120, 215)); // Windows 98 blue
    g.fillRect(0, 0, getWidth(), 30);

    g.setColour (juce::Colours::grey);

    auto logoA = juce::ImageCache::getFromMemory(BinaryData::_98_logo_png, BinaryData::_98_logo_pngSize);
    g.drawImage(logoA, 5, 5, 20, 20, 0, 0, logoA.getWidth(), logoA.getHeight());

    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(15.0f, juce::Font::bold));
    g.drawText("Lossify - MP3ify Your Audio", 30, 5, getWidth() - 35, 20, juce::Justification::centredLeft);

	auto logoB = juce::ImageCache::getFromMemory(BinaryData::lossify_logo_png, BinaryData::lossify_logo_pngSize);
	g.drawImage(logoB, 10, 40, 120, 120, 0, 0, logoB.getWidth(), logoB.getHeight());
}

void LossifyAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    const int knobSize = 120;
    const int spacing = 20;
    const int topMargin = 180;

    m_BitrateKnob.setBounds(spacing, topMargin, knobSize, knobSize);
    m_ArtifactsKnob.setBounds(m_BitrateKnob.getRight() + spacing, topMargin, knobSize, knobSize);
	m_NoiseLevelKnob.setBounds(m_ArtifactsKnob.getRight() + spacing, topMargin, knobSize, knobSize);

    m_MeterL.setBounds(140, 80, 300, 20);
	m_MeterR.setBounds(140, 110, 300, 20);
}

void LossifyAudioProcessorEditor::timerCallback()
{
    auto& artifacts = audioProcessor.getLossyArtifacts();

    // Scaled RMS values to fill the meter more visually
    float leftRMS = artifacts.getLeftChannelRMS() * 4.0f; // Scale up the value
    float rightRMS = artifacts.getRightChannelRMS() * 4.0f; // Scale up the value

    // Clamped values to ensure they don't exceed the meter range
    m_MeterL.setLevel(juce::jlimit(0.0f, 1.0f, leftRMS));
    m_MeterR.setLevel(juce::jlimit(0.0f, 1.0f, rightRMS));

    repaint();
}
