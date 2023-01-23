/*
  ==============================================================================

    EnvelopeDetector.cpp
    Created: 16 Jul 2019 10:00:47am
    Author:  José Díaz Rohena

  ==============================================================================
*/



#include "EnvelopeDetector.h"
#include "JuceHeader.h"
#include "AudioHelpers.h"
#include <math.h>

EnvelopeDetector::EnvelopeDetector (bool autoReleaseFlag) :
autoMode(autoReleaseFlag)
{
}

void EnvelopeDetector::reset()
{
    prevEnv.set (0.0);
    currentGain.set (0.0);
}

void EnvelopeDetector::setSampleRate(double inSampleRate)
{
    if (sampleRate.get() != inSampleRate)
    {
        sampleRate.set (inSampleRate);
        
        setTimeConstant (msAttack.get(), true);
        setTimeConstant (msRelease.get(), false);
    }
}

void EnvelopeDetector::setTimeConstant (double inTime, bool attack)
{
    jassert (inTime > 0.0);
    
    auto sr = sampleRate.get();
    
    auto timeCoeff = setCoefficient (inTime,
                                     kEnvelopeTimeConstant,
                                     sr);
    if (attack)
    {
        tauAttack.set (timeCoeff);
        msAttack.set(inTime);
    }
    else
    {
        tauRelease.set (timeCoeff);
        msRelease.set(inTime);
        
        if (autoMode)
            tauSlowRelease.set (setCoefficient (inTime * slowReleaseMultiplier, kEnvelopeTimeConstant, sr));
    }
}

inline double EnvelopeDetector::setCoefficient (double timeInMilliseconds,
                                         double timeScalar,
                                         double sampleRate)
{
    return exp(timeScalar / (0.001 * timeInMilliseconds * sampleRate));
}

void EnvelopeDetector::updateCurrentGain (double inGain) {currentGain.set (inGain); }

inline double EnvelopeDetector::getReleaseCoefficient()
{
    if (!autoMode)
        return tauRelease.get();

    return currentGain.get() < slowReleaseThreshold ? tauSlowRelease.get() : tauRelease.get();
}

double EnvelopeDetector::processSampleDecoupledBranched (double inputSample)
{
    ScopedNoDenormals noDenormals;
    
    auto prevOutput = prevEnv.get();
    auto coeff  = inputSample > prevOutput ? tauAttack.get() : tauRelease.get();
    auto smoothedSample = coeff * (prevOutput - inputSample) + inputSample;
    prevEnv.set (smoothedSample);

    return  smoothedSample;
}

