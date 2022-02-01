/*
  ==============================================================================

    Guitar.h
    Created: 21 Nov 2021 1:45:17pm
    Author:  Jack Wardman

  ==============================================================================
*/

#pragma once
#include "Karplus_String.h"

class Guitar
{
public:
    void initialise(double sampleRate, int samplesPerBlock);
    void startNote(float noteFreq, float velocity);
    void stopNote(float noteFreq);
    float renderSample(float excitationTimeParam, float brightnessParam, float pluckPosParam, int pitchBend);
    void applyConvolution (AudioBuffer<float> &outputBuffer);
private:
    double mSampleRate;
    float mY;
    Karplus_String strings[6];
    dsp::Convolution conv;
    File impulseResponse;
    dsp::ProcessSpec spec;

    float mOpenFrequency[6] = {329.36, 246.94f, 196.f, 146.83f, 110.f, 82.41f};
    float mStringFeedback[6] = {0.999, 0.997, 0.995, 0.993, 0.992, 0.992};
    int mStringCutoffs[6] = {10000, 7000, 4000, 2500, 2000, 1700};
    
};


