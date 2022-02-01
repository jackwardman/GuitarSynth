/*
  ==============================================================================

    String.h
    Created: 19 Nov 2021 3:25:24pm
    Author:  Jack Wardman

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class Karplus_String
{
public:
    Karplus_String();
    void initialise(double sampleRate, int samplesPerBlock, float openFrequency);
    void startNote(float noteFrequency, float velocity);
//    void calculatePitchBend(int pitchBendAmount);
    float excSignal(float lengthInMilSecs, int lastMidiNote, float brightness);
    float linear_interpolation(float X, float Y, float phase);
    void calculate_coefficients(float cutoffFrequency);
    float renderSample(float excitationTimeParam, float brightnessParam, float pluckPosParam, float mY);
    void calculate_HP_Coefficients();
    
    float mFeedback;
    
private:
    int mLastMidiMessage = 0;
    int mCircularBufferLength = 0;
    int mPluckBufferLength = 0;
    AudioBuffer<float> mCircularBuffer;
    AudioBuffer<float> mPluckBuffer;

    float mLowestFrequency;
    
    int mWriteInx = 0;
    int mPluckWriteInx = 0;
    
    float mXz1 = 0;
    float mYz1 = 0;
    float mFiltOut = 0;
    float mXhp = 0;
    float mYhp = 0;
    
    float mNoteFreq = 440;
    float mPitchBendRatio = 1;
    float mFrequency;
    float mPhaseEx = 0;
    float mOpenFrequency;
    
    float mCutoffFrequency;
    float mFilterCoefficients[3];
    float mOutputCoefficients[3];

    float mVelocity = 0;
    double mSampleRate;
};
