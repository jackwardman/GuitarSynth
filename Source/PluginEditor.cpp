/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarObjectAudioProcessorEditor::GuitarObjectAudioProcessorEditor (GuitarObjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
    
    auto& params = processor.getParameters();
    
    AudioParameterFloat* mExcitationTimeParameter = (AudioParameterFloat*)params.getUnchecked(0);
    
    mExcitationTimeSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mExcitationTimeSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mExcitationTimeSlider.setRange(mExcitationTimeParameter->range.start, mExcitationTimeParameter->range.end);
    mExcitationTimeSlider.setValue(*mExcitationTimeParameter);
    addAndMakeVisible(mExcitationTimeSlider);
    
    mExcitationTimeSlider.onValueChange = [this, mExcitationTimeParameter] { *mExcitationTimeParameter = mExcitationTimeSlider.getValue(); };
    mExcitationTimeSlider.onDragStart = [mExcitationTimeParameter] { mExcitationTimeParameter->beginChangeGesture(); };
    mExcitationTimeSlider.onDragEnd = [mExcitationTimeParameter] { mExcitationTimeParameter->endChangeGesture(); };
    
    AudioParameterFloat* mBrightnessParameter = (AudioParameterFloat*)params.getUnchecked(1);
    
    mBrightnessSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
    mBrightnessSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mBrightnessSlider.setRange(mBrightnessParameter->range.start, mBrightnessParameter->range.end);
    mBrightnessSlider.setValue(*mBrightnessParameter);
    mBrightnessSlider.setSkewFactorFromMidPoint(0.25f);
    addAndMakeVisible(mBrightnessSlider);
    
    mBrightnessSlider.onValueChange = [this, mBrightnessParameter] { *mBrightnessParameter = mBrightnessSlider.getValue(); };
    mBrightnessSlider.onDragStart = [mBrightnessParameter] { mBrightnessParameter->beginChangeGesture(); };
    mBrightnessSlider.onDragEnd = [mBrightnessParameter] { mBrightnessParameter->endChangeGesture(); };
    
    
    AudioParameterFloat* mPluckPositionParameter = (AudioParameterFloat*)params.getUnchecked(2);
    
    mPluckPositionSlider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    mPluckPositionSlider.setRange(mPluckPositionParameter->range.start, mPluckPositionParameter->range.end);
    mPluckPositionSlider.setValue(*mPluckPositionParameter);
    addAndMakeVisible(mPluckPositionSlider);
    
    mPluckPositionSlider.onValueChange = [this, mPluckPositionParameter] { *mPluckPositionParameter = mPluckPositionSlider.getValue(); };
    mPluckPositionSlider.onDragStart = [mPluckPositionParameter] { mPluckPositionParameter->beginChangeGesture(); };
    mPluckPositionSlider.onDragEnd = [mPluckPositionParameter] { mPluckPositionParameter->endChangeGesture(); };
}

GuitarObjectAudioProcessorEditor::~GuitarObjectAudioProcessorEditor()
{
}

//==============================================================================
void GuitarObjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Physical Modelling Guitar Synthesizer", getLocalBounds(), juce::Justification::centredBottom, 1);
    
    g.drawText("Excitation Time", ((getWidth() / 4 ) * 1.5) - (100 / 2), (getHeight() / 4), 100, 100, Justification::centredBottom);
    g.drawText("Brightness", ((getWidth() / 4 ) * 2.5) - (100 / 2), (getHeight() / 4), 100, 100, Justification::centredBottom);
    g.drawText("Pluck Position", ((getWidth() / 4 ) * 0.8) - (100 / 2), (getHeight() / 4) - 70, 100, 100, Justification::left);
}

void GuitarObjectAudioProcessorEditor::resized()
{
    auto sliderLeft = 120;
    mPluckPositionSlider.setBounds(sliderLeft, (getHeight() / 8), getWidth() - sliderLeft - 10, 20);
    mExcitationTimeSlider.setBounds(((getWidth() / 4 ) * 1.5) - (100 / 2), (getHeight() / 4), 100, 100);
    mBrightnessSlider.setBounds(((getWidth() / 4 ) * 2.5) - (100 / 2), (getHeight() / 4), 100, 100);
}
