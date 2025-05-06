/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarObjectAudioProcessor::GuitarObjectAudioProcessor()
    : AudioProcessor (
#ifndef JucePlugin_PreferredChannelConfigurations
        BusesProperties()
        #if ! JucePlugin_IsMidiEffect
         #if ! JucePlugin_IsSynth
          .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
         #endif
          .withOutput ("Output", juce::AudioChannelSet::mono(), true)
        #endif
#endif
      )
{
    addParameter(excitationTimeParam = new juce::AudioParameterFloat(
            juce::ParameterID("excitationtime", 2), "Excitation Time",
            juce::NormalisableRange<float>(1.0f, 10.0f), 5.0f, "ms"));

        addParameter(brightnessParam = new juce::AudioParameterFloat(
            juce::ParameterID("brightness", 2), "Brightness",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.25f, "%"));

        addParameter(pluckPositionParam = new juce::AudioParameterFloat(
            juce::ParameterID("pluckposition", 2), "Pluck Position",
            juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f, "%"));
}

GuitarObjectAudioProcessor::~GuitarObjectAudioProcessor()
{
}

//==============================================================================
const juce::String GuitarObjectAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarObjectAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GuitarObjectAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GuitarObjectAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GuitarObjectAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarObjectAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarObjectAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarObjectAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarObjectAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarObjectAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarObjectAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    guitar.initialise(sampleRate, samplesPerBlock);
}

void GuitarObjectAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarObjectAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainInputChannelSet() == AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() == AudioChannelSet::mono()) {
        return true;
    } else {
        return false;
    }
}
#endif

void GuitarObjectAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    float* channelData = buffer.getWritePointer(0);

    float excitationTime = excitationTimeParam->get();
    float brightness = brightnessParam->get();
    float pluckPosition = pluckPositionParam->get();

    int pitchBend = 0;

    for (const auto metadata : midiMessages)
    {
        auto m = metadata.getMessage();
        if (m.isNoteOn())
            guitar.startNote(juce::MidiMessage::getMidiNoteInHertz(m.getNoteNumber()), m.getFloatVelocity());
        else if (m.isNoteOff())
            guitar.stopNote(juce::MidiMessage::getMidiNoteInHertz(m.getNoteNumber()));
    }

    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        channelData[i] = guitar.renderSample(excitationTime, brightness, pluckPosition, pitchBend);
    }

    guitar.applyConvolution(buffer);
}

//==============================================================================
bool GuitarObjectAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarObjectAudioProcessor::createEditor()
{
    return new GuitarObjectAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarObjectAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::XmlElement xml("PARAMETERS");
    xml.setAttribute("excitationtime", excitationTimeParam->get());
    xml.setAttribute("brightness", brightnessParam->get());
    xml.setAttribute("pluckposition", pluckPositionParam->get());
    copyXmlToBinary(xml, destData);
}

void GuitarObjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName("PARAMETERS"))
    {
        excitationTimeParam->setValueNotifyingHost((float)xml->getDoubleAttribute("excitationtime", 5.0f));
        brightnessParam->setValueNotifyingHost((float)xml->getDoubleAttribute("brightness", 0.25f));
        pluckPositionParam->setValueNotifyingHost((float)xml->getDoubleAttribute("pluckposition", 0.2f));
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarObjectAudioProcessor();
}
