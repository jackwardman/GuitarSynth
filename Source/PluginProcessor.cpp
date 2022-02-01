/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarObjectAudioProcessor::GuitarObjectAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::mono(), true)
                     #endif
                       )
#endif
{
    //    Define user controlable parameters with ranges
    addParameter(mExcitationTimeParameter = new AudioParameterFloat("excitationtime",
                                                            "Excitation Time",
                                                            1.f,
                                                            10.f,
                                                            5.f));

    
    addParameter(mBrightnessParameter = new AudioParameterFloat("brightness",
                                                           "Brightness",
                                                           0.f,
                                                           1.f,
                                                           0.25f));
    
    addParameter(mPluckPositionParameter = new AudioParameterFloat("pluckposition",
                                                           "Pluck Position",
                                                           0.f,
                                                           1.f,
                                                           0.2f));
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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    float* channelData = buffer.getWritePointer(0);
    
    for (int i = 0; i < buffer.getNumSamples(); i++)
    {
        int pitchBend;
        for (const auto metadata : midiMessages)
        {
            // grab the metadata from that message
            auto m = metadata.getMessage();
            // if midi message note on is called, reset lastMidiMessage to 0 to trigger
            if (m.isNoteOn() == true)
            {
                guitar.startNote(MidiMessage::getMidiNoteInHertz(m.getNoteNumber()), m.getFloatVelocity());
            }
            else if (m.isNoteOff() == true)
            {
                guitar.stopNote(MidiMessage::getMidiNoteInHertz(m.getNoteNumber()));
            }
//            else if (m.isPitchWheel() == true)
//            {
//                pitchBend = m.getPitchWheelValue();
//                DBG(pitchBend);
//            }
        }
        channelData[i] = guitar.renderSample(*mExcitationTimeParameter, *mBrightnessParameter, *mPluckPositionParameter, pitchBend);
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
    std::unique_ptr<XmlElement> xml(new XmlElement("GuitarSynth"));
    xml-> setAttribute("excitationtime", *mExcitationTimeParameter);
    xml->setAttribute("brightness", *mBrightnessParameter);
    xml->setAttribute("pluckposition", *mPluckPositionParameter);
    
    copyXmlToBinary(*xml, destData);
}

void GuitarObjectAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml.get() != nullptr && xml->hasTagName("GuitarSynth")) {
        *mExcitationTimeParameter = xml->getDoubleAttribute("excitationtime");
        *mBrightnessParameter = xml->getDoubleAttribute("brightness");
        *mPluckPositionParameter = xml->getDoubleAttribute("pluckposition");
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarObjectAudioProcessor();
}
