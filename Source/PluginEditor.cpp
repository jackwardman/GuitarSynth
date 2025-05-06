#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarObjectAudioProcessorEditor::GuitarObjectAudioProcessorEditor (GuitarObjectAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (600, 200);

    // Excitation Time
    mExcitationTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mExcitationTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mExcitationTimeSlider);

    excitationTimeAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.getExcitationTimeParam(), mExcitationTimeSlider);

    // Brightness
    mBrightnessSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mBrightnessSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mBrightnessSlider);

    brightnessAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.getBrightnessParam(), mBrightnessSlider);

    // Pluck Position
    mPluckPositionSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mPluckPositionSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(mPluckPositionSlider);

    pluckPositionAttachment = std::make_unique<juce::SliderParameterAttachment>(
        *audioProcessor.getPluckPositionParam(), mPluckPositionSlider);
}

GuitarObjectAudioProcessorEditor::~GuitarObjectAudioProcessorEditor()
{
}

//==============================================================================
void GuitarObjectAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Physical Modelling Guitar Synthesizer", getLocalBounds(), juce::Justification::centredBottom, 1);

    g.drawText("Pluck Position", (getWidth() / 4 ) - 50, (getHeight() / 4), 100, 100, juce::Justification::centredBottom);
    g.drawText("Excitation Time", ((getWidth() / 4 ) * 2) - 50, (getHeight() / 4), 100, 100, juce::Justification::centredBottom);
    g.drawText("Brightness", ((getWidth() / 4 ) * 3) - 50, (getHeight() / 4), 100, 100, juce::Justification::centredBottom);
}

void GuitarObjectAudioProcessorEditor::resized()
{
    mPluckPositionSlider.setBounds((getWidth() / 4 ) - 50, (getHeight() / 4), 100, 100);
    mExcitationTimeSlider.setBounds(((getWidth() / 4 ) * 2) - 50, (getHeight() / 4), 100, 100);
    mBrightnessSlider.setBounds(((getWidth() / 4 ) * 3) - 50, (getHeight() / 4), 100, 100);
}
