/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ProtectYourEars.h"

//==============================================================================
LossifyAudioProcessor::LossifyAudioProcessor()
	: AudioProcessor(BusesProperties()

		.withInput("Input", juce::AudioChannelSet::stereo(), true)

		.withOutput("Output", juce::AudioChannelSet::stereo(), true)),
	m_Params{ *this }

{

}

LossifyAudioProcessor::~LossifyAudioProcessor()
{
}

//==============================================================================
const juce::String LossifyAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool LossifyAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool LossifyAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool LossifyAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double LossifyAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int LossifyAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
	// so this should be at least 1, even if you're not really implementing programs.
}

int LossifyAudioProcessor::getCurrentProgram()
{
	return 0;
}

void LossifyAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String LossifyAudioProcessor::getProgramName(int index)
{
	return {};
}

void LossifyAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void LossifyAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	m_Params.PrepareToPlay(sampleRate);
	m_LossyArtifacts.prepare(sampleRate);

}

void LossifyAudioProcessor::releaseResources()
{
	m_LossyArtifacts.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool LossifyAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
#endif

	return true;
#endif
}
#endif

void LossifyAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	juce::ScopedNoDenormals noDenormals;
	auto totalNumInputChannels = getTotalNumInputChannels();
	auto totalNumOutputChannels = getTotalNumOutputChannels();

	for (auto i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	m_Params.Update();
	m_Params.Smoothen();

	float bitrate = m_Params.GetBitrate();
	float artifacts = m_Params.GetArtifacts();
	float noiseLevel = m_Params.GetNoiseLevel();

	m_LossyArtifacts.process(buffer, bitrate, artifacts, noiseLevel);//process the audio buffer with the LossyArtifacts class

	protectYourEars(buffer);
}

//==============================================================================
bool LossifyAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* LossifyAudioProcessor::createEditor()
{
	return new LossifyAudioProcessorEditor(*this);
}

//==============================================================================
void LossifyAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{

	copyXmlToBinary(*m_Params.GetAPVTS().copyState().createXml(), destData);

	DBG(m_Params.GetAPVTS().copyState().toXmlString());

}

void LossifyAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{

	std::unique_ptr<juce::XmlElement> xml{ getXmlFromBinary(data, sizeInBytes) };

	if (xml != nullptr && xml->hasTagName(m_Params.GetAPVTS().state.getType()))
	{
		m_Params.GetAPVTS().replaceState(juce::ValueTree::fromXml(*xml));
	}

}

juce::AudioProcessorValueTreeState& LossifyAudioProcessor::GetAPVTS()
{
	return m_Params.GetAPVTS();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new LossifyAudioProcessor();
}
