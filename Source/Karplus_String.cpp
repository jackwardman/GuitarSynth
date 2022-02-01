/*
  ==============================================================================

    String.cpp
    Created: 19 Nov 2021 3:25:24pm
    Author:  Jack Wardman

  ==============================================================================
*/

#include "Karplus_String.h"
Karplus_String::Karplus_String()
{
    
    
}
void Karplus_String::initialise(double sampleRate, int samplesPerBlock, float openFrequency)
{
    mCircularBufferLength = (int)((sampleRate / openFrequency)+1);
    mCircularBuffer.setSize(1, mCircularBufferLength);
    mCircularBuffer.clear();
    mNoteFreq = openFrequency;
//    mFrequency = mNoteFreq;
    mPluckBufferLength = mCircularBufferLength;
    mPluckBuffer.setSize(1, mPluckBufferLength);
    mPluckBuffer.clear();
    mOpenFrequency = openFrequency;
    
    calculate_HP_Coefficients();

    mSampleRate = sampleRate;
}

void Karplus_String::startNote(float noteFrequency, float velocity)
{
    mLastMidiMessage = 0;
    mPhaseEx = 0;
    mNoteFreq = noteFrequency;
//    mFrequency = mNoteFreq * mPitchBendRatio;
//    if (mFrequency < mOpenFrequency)
//        mFrequency = mOpenFrequency;
    
    mVelocity = velocity * 0.75;
}

//void Karplus_String::calculatePitchBend(int pitchBendAmount)
//{
//    float pitchBendInSemitones = (pitchBendAmount - 8192) * 2.f / 8192;
//    mPitchBendRatio = pow(2.f,(pitchBendInSemitones/12));
//    mFrequency = mNoteFreq * mPitchBendRatio;
//    if (mFrequency < mOpenFrequency)
//        mFrequency = mOpenFrequency;
//}

float Karplus_String::excSignal(float lengthInMilSecs, int lastMidiNote, float brightness)
{
    //    initialise excitation signal parameters
    float outputSample;
    float lengthInSecs = lengthInMilSecs * 0.001;
    float lengthInSamples = lengthInSecs * mSampleRate;
    float f = 1/lengthInSecs;

    //    if the last MIDI note is smaller than the excitation length in samples: generate the excitation sample value
    if (lastMidiNote <= lengthInSamples)
    {
        //  use the brightness parameter to dictate the balance between 1 cycle of triangle wave and the noise signal
        outputSample = mVelocity * (((1 - brightness) * (2/M_PI)*asin(sin(mPhaseEx))) +  (brightness * ((juce::Random::getSystemRandom().nextFloat() * 2) - 1)));
    }
    //    else generate 0 valued input sample
    else
    {
        outputSample = 0.0f;
    }
    //    update phase of the triangle wave and wrap around 2pi
    mPhaseEx += (2*M_PI*f)/mSampleRate;
    if (mPhaseEx > 2*M_PI)
        mPhaseEx -= 2*M_PI;
    //    return the output sample
    return outputSample;
}

float Karplus_String::linear_interpolation(float X, float Y, float Float)
{
    return (1 - Float) * X + Float * Y;
}

void Karplus_String::calculate_coefficients(float cutoffFrequency)
{
    // normalise cutoff frequency to 2pi rad/sec
    float normFreq = 2 * M_PI * cutoffFrequency / mSampleRate;
    // calculate ceofficients
    float a0 = 1 + tan(normFreq/2);
    float a1 = 1 - tan(normFreq/2);
    float b0 = tan(normFreq/2);
    float b1 = b0;
    
    // assign coefficients (divided by a0) to global array
    mFilterCoefficients[0] = (1/a0) * b0;
    mFilterCoefficients[1] = (1/a0) * b1;
    mFilterCoefficients[2] = (1/a0) * a1;

}

float Karplus_String::renderSample(float excitationTimeParam, float brightnessParam, float pluckPosParam, float mY)
{
    //   call excSignal function to generate excitation signal sample
    float x = excSignal(excitationTimeParam, mLastMidiMessage, brightnessParam);
    
    //   calculate the read index postion
    float readInx = mWriteInx - ((mSampleRate/mNoteFreq)-1);
    float pluckReadInx = mPluckWriteInx - ((mSampleRate * pluckPosParam)/mOpenFrequency);
    
    //   wrap the read index both ways
    if (readInx < 0) {
        readInx += mCircularBufferLength;
    }
    if (readInx >= mCircularBufferLength) {
        readInx -= mCircularBufferLength;
    }
    if (pluckReadInx < 0) {
        pluckReadInx += mPluckBufferLength;
    }
    if (pluckReadInx >= mPluckBufferLength) {
        pluckReadInx -= mPluckBufferLength;
    }
    
    
    //   initialise parameters for interpolation
    int readX = (int)readInx;
    int readY = readX + 1;
    float ReadFloat = readInx - readX;
    if (readY >= mCircularBufferLength)
        readY -= mCircularBufferLength;
    
    int pluckReadX = (int)pluckReadInx;
    int pluckReadY = pluckReadX + 1;
    float pluckReadFloat = pluckReadInx - pluckReadX;
    if (pluckReadY >= mPluckBufferLength)
        pluckReadY -= mPluckBufferLength;
    
    
    float Interp = linear_interpolation(mCircularBuffer.getSample(0, readX), mCircularBuffer.getSample(0, readY), ReadFloat);
    float pluckInterp = linear_interpolation(mPluckBuffer.getSample(0, pluckReadX), mPluckBuffer.getSample(0, pluckReadY), pluckReadFloat);
    
    
    //  Calculate the filter output
    mFiltOut = (mFilterCoefficients[0] * Interp) +  (mFilterCoefficients[1] * mXz1) + (mFilterCoefficients[2] * mYz1);
    //   Update previous sample values
    mXz1 = Interp;
    mYz1 = mFiltOut;
    
    mPluckBuffer.setSample(0, mPluckWriteInx, x);

    
    //  calculate the output sample
    float y = (x + (mFeedback * mFiltOut) - pluckInterp) + 0.0001 * mY;
    
    // High pass the final output to get rid of the DC offset
    float highPass = (mOutputCoefficients[0] * y) + (mOutputCoefficients[1] * mXhp) - (-mOutputCoefficients[2] * mYhp);
    mXhp = y;
    mYhp = highPass;
    
    //  write the output sample to the circular buffer
    mCircularBuffer.setSample(0, mWriteInx, y);
    
    
    //  increment and wrap the write index
    mWriteInx++;
    if (mWriteInx >= mCircularBufferLength) {
        mWriteInx = 0;
    }
    mPluckWriteInx++;
    if (mPluckWriteInx >= mCircularBufferLength) {
        mPluckWriteInx = 0;
    }

    //  increment the last midi message variable
    mLastMidiMessage++;
    
    //  write the output sample to the host audio buffer
    return highPass;
}


void Karplus_String::calculate_HP_Coefficients()
{
    float normFreq = 2 * M_PI * 50 / mSampleRate;
    // calculate ceofficients
    float a0 = (1 + tan(normFreq/2));
    float a1 = (1 - tan(normFreq/2));
    float b0 = 1;
    float b1 = -1;
    
    // assign coefficients (divided by a0) to global array
    mOutputCoefficients[0] = (1/a0) * b0;
    mOutputCoefficients[1] = (1/a0) * b1;
    mOutputCoefficients[2] = (1/a0) * a1;
}
