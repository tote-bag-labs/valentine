/*
  ==============================================================================

    Saturation.h
    Created: 16 Aug 2019 1:24:47pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

#include <array>

// TODO: there is some bounds checking that needs to be done intelligently here
// Basically, some of these algs have diff min maxes. The system blows up if they
// are violated. This whole thing needs a refactor anyway, so i'll just put the
// reminder here instead of making changes thoughtlessly:
// inverseHyperbolicTangent (interpolated) get a min of .001
// the others have a min of probably 1.0
// example: we need a way to specialize the class so all member variables,
// such as upcoming ones that can announce limits to other classes, are customized
// right now, the saturation param starts at 1.0 since that will not blow up for all of them
// but I am initializing the IIHT to .001 in Valentine. Awkward!

class Saturation
{
public:
    
    enum class Type
    {
        inverseHyperbolicSine,
        sineArcTangent,
        hyperbolicTangent,
        inverseHyperbolicSineInterp,
        interpolatedHyperbolicTangent
    };

    Saturation( Type sType, double asymm = 0.0 );
  
    void setParams(double inSaturation);
    
    void reset(double sampleRate);
    
    inline double calcGain (double inputSample, double sat);
    
    inline double processSample (double inputSample, int channel, double sat);
    
//==============================================================
    
    inline double inverseHyperbolicSine (double x, double gain);
    
    inline float inverseHyperbolicSineInterp (float x, float gain, int channel);
    
    inline float sineArcTangent (float x, float gain);
    
    inline double hyperbolicTangent (double x, double gain, int channel);
    
    inline double interpolatedHyperbolicTangent (double x, double gain, int channel);
    
    inline double hyperTanFirstAntiDeriv (double x);
    
    inline float invHypeSineAntiDeriv(float x);

//==============================================================

    void processBlock( juce::dsp::AudioBlock<float> &inAudio );

private:

    Type saturationType;

    double asymmetry {0.0};

    const double gainRampSec {.005};

    juce::SmoothedValue<double, juce::ValueSmoothingTypes::Linear> smoothedSat {1.0f};
    juce::AudioBuffer<double> xState;
    std::array<double, 2> antiderivState;

   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Saturation)

};
