/*
  ==============================================================================

    Guitar.cpp
    Created: 21 Nov 2021 1:45:17pm
    Author:  Jack Wardman

  ==============================================================================
*/

#include "Guitar.h"

void Guitar::initialise(double sampleRate, int samplesPerBlock)
{
    for (int i = 0; i < 6; i++)
    {
        strings[i].initialise(sampleRate, samplesPerBlock, mOpenFrequency[i]);
        strings[i].mFeedback = mStringFeedback[i];
        strings[i].calculate_coefficients(mStringCutoffs[i]);
    }
    mSampleRate = sampleRate;
        
    impulseResponse = ("/Users/jackwardman/Documents/JuceProjects/GuitarSynth/IR/IR Guitar - Classical - AmBu2F - AKG CS1000.wav");
    // Initialise for convolution with the impulse response
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    spec.maximumBlockSize = samplesPerBlock;
    
    conv.reset();
    conv.loadImpulseResponse(impulseResponse, dsp::Convolution::Stereo::no, dsp::Convolution::Trim::no, impulseResponse.getSize(), dsp::Convolution::Normalise::yes);
    conv.prepare(spec);
}

void Guitar::startNote(float noteFreq, float velocity)
{
    if (noteFreq < 800 && noteFreq >= 328) {
        // reset the phase and last midi message variables to trigger the note
        strings[0].startNote(noteFreq, velocity);
        if (strings[0].mFeedback != 0.999f)
            strings[0].mFeedback = 0.999f;
    //  string 2
    } else if (noteFreq < 328 && noteFreq >= 245) {
        strings[1].startNote(noteFreq, velocity);
        if (strings[1].mFeedback != 0.997f)
            strings[1].mFeedback = 0.997f;
    //  string 3
    } else if (noteFreq < 245 && noteFreq >= 195) {
        strings[2].startNote(noteFreq, velocity);
        if (strings[2].mFeedback != 0.995f)
            strings[2].mFeedback = 0.995f;
    //  string 4
    } else if (noteFreq < 195 && noteFreq >= 145) {
        strings[3].startNote(noteFreq, velocity);
        if (strings[3].mFeedback != 0.993f)
            strings[3].mFeedback = 0.993f;
    //  string 5
    } else if (noteFreq < 145 && noteFreq >= 109) {
        strings[4].startNote(noteFreq, velocity);
        if (strings[4].mFeedback != 0.992f)
            strings[4].mFeedback = 0.992f;
    //  string 6
    } else if (noteFreq < 109 && noteFreq >= 81) {
        strings[5].startNote(noteFreq, velocity);
        if (strings[5].mFeedback != 0.992f)
            strings[5].mFeedback = 0.992f;
    }
}

void Guitar::stopNote(float noteFreq)
{
    if (noteFreq < 800 && noteFreq >= 328) {
        strings[0].mFeedback = 0.995f;
    //  string 2
    } else if (noteFreq < 328 && noteFreq >= 245) {
        strings[1].mFeedback = 0.992f;
    //  string 3
    } else if (noteFreq < 245 && noteFreq >= 195) {
        strings[2].mFeedback = 0.99f;
    //  string 4
    } else if (noteFreq < 195 && noteFreq >= 145) {
        strings[3].mFeedback = 0.99f;
    //  string 5
    } else if (noteFreq < 145 && noteFreq >= 109) {
        strings[4].mFeedback = 0.99f;
    //  string 6
    } else if (noteFreq < 109 && noteFreq >= 81) {
        strings[5].mFeedback = 0.985f;
    }
}



float Guitar::renderSample(float excitationTimeParam, float brightnessParam, float pluckPosParam, int pitchBend)
{
    float y = 0;
    for (int i = 0; i < 6; i++)
    {
//        strings[i].calculatePitchBend(pitchBend);
        y += strings[i].renderSample(excitationTimeParam, brightnessParam, pluckPosParam, mY);
    }
    y *= (1 - pluckPosParam);


    //  write the output sample to the host audio buffer
    mY = y;
    return y;
}

void Guitar::applyConvolution (AudioBuffer<float> &outputBuffer)
{
    //  perform convolution with guitar's impulse response
    dsp::AudioBlock<float> block(outputBuffer);
    dsp::ProcessContextReplacing<float> context(block);
    conv.process(context);
}
