/*
  ==============================================================================

    Saturation.h
    Created: 16 Aug 2019 1:24:47pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

#include "tote_bag/utils/type_helpers.hpp"

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
    enum class Type {
        inverseHyperbolicSine,
        sineArcTangent,
        hyperbolicTangent,
        inverseHyperbolicSineInterp,
        interpolatedHyperbolicTangent
    };

    Saturation (Type sType, float asymm = 0.0);

    void setParams (float inSaturation);

    void reset (double sampleRate);

    inline float calcGain (float inputSample, float sat);

    inline float processSample (float inputSample, size_t channel, float sat);

    //==============================================================

    inline float invHypeSineAntiDeriv (float x);

    inline float inverseHyperbolicSineInterp (float x, size_t channel);

    inline float sineArcTangent (float x, float gain);

    inline float hyperTanFirstAntiDeriv (float x);

    inline float interpolatedHyperbolicTangent (float x, size_t channel);

    //==============================================================

    void processBlock (juce::dsp::AudioBlock<float>& inAudio);

private:
    // tags - first step towards a templated version of this class
    struct inverseHyperbolicSineTag
    {
    };
    struct hyperbolicTangentTag
    {
    };

    /** Returns the compensation gain for a given saturation type and input gain.
     *  This allows us to increase input gain without changing the output level.
     *
     *  Generally, the gain is found using the following: 1.0 / f(inputGain).
     *  With some functions this will inappropriately boost output level
     *  for small values. In these cases, we simply return 1.0 / inputGain.
     *
     *  Note: The tolerance point determining when we use this alternate calculation
     *  may need to be adjusted for different saturation types.
     */
    template <typename SaturationType, typename FloatType>
    auto compensationGain (FloatType inputGain)
    {
        if constexpr (std::is_same<SaturationType, inverseHyperbolicSineTag>::value)
        {
            // Tolerance determined by eyballing graphs in desmos
            constexpr FloatType tolerance = 1.02;
            if (inputGain <= tolerance)
            {
                return static_cast<FloatType> (1.0) / inputGain;
            }
            return static_cast<FloatType> (1.0) / std::asinh (inputGain);
        }
        else if constexpr (std::is_same<SaturationType, hyperbolicTangentTag>::value)
        {
            // Tolerance determined by eyballing graphs in desmos
            constexpr FloatType tolerance = 1.02;
            if (inputGain <= tolerance)
            {
                return static_cast<FloatType> (1.0) / inputGain;
            }
            return static_cast<FloatType> (1.0) / std::tanh (inputGain);
        }
        else
        {
            static_assert (tote_bag::type_helpers::dependent_false<SaturationType>::value,
                           "Unsupported saturation type.");
        }
    }

    Type saturationType;

    float asymmetry {0.0f};

    const float gainRampSec {.005f};

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedSat {1.0f};
    juce::AudioBuffer<float> xState;
    // I guess we're assuming first order ADAA, stereo here.
    std::array<float, 2> antiderivState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Saturation)
};
