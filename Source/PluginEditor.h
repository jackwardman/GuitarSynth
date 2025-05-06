/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GuitarObjectAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GuitarObjectAudioProcessorEditor (GuitarObjectAudioProcessor&);
    ~GuitarObjectAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Slider mExcitationTimeSlider;
    Slider mBrightnessSlider;
    Slider mPluckPositionSlider;
    
    std::unique_ptr<juce::SliderParameterAttachment> excitationTimeAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> brightnessAttachment;
    std::unique_ptr<juce::SliderParameterAttachment> pluckPositionAttachment;

    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GuitarObjectAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarObjectAudioProcessorEditor)
};
