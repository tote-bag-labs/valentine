/*
  ==============================================================================

    EnvelopeDetector.h
    Created: 16 Jul 2019 10:00:47am
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

/**
    A peak level detector.
 */
class EnvelopeDetector
{
public:
    
    EnvelopeDetector (bool autoReleaseFlag);
    
    void reset();
    
    void setSampleRate( double inSampleRate );
    
    void setParams (double inAttack, double inRelease);
    
    void setTimeConstant( double inTime, bool attack );
    
    /** returns a release coefficient depending on whether or not the envelope
        has been set to auto, and then whether or not it meets the threshold
        to used the slow attack
     */
    double getReleaseCoefficient();
    
    double processSampleDecoupledBranched ( double inputSample );
    
    void updateAutoReleaseCounter(bool advance);
    
    void updateCurrentGain(double inGain);
    
    /** sets the time coefficients used in envelope detector
    */
    static double setCoefficient( double timeInMilliseconds,
                                 double timeScalar,
                                 double sampleRate);
    
private:

    bool autoMode {false};
    
    juce::Atomic<double> sampleRate {0.0},
                   prevEnv {0.0},
                   currentGain {0.0},
                   msAttack  {-1},
                   msRelease {-1},
                   tauAttack {0.0},
                   tauRelease {0.0},
                   tauSlowRelease {0.0};
    
    const double slowReleaseThreshold {-12.0};
    const int slowReleaseMultiplier {2};

    static constexpr double kEnvelopeTimeConstant = -0.99967234081320612357829304641019;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopeDetector)
};


