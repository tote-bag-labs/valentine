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

    inline float hyperbolicTangent (float x);

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

    template <typename SaturationType, typename FloatType>
    auto compensationGain (FloatType inputGain)
    {
        if constexpr (std::is_same<SaturationType, inverseHyperbolicSineTag>::value)
        {
            return static_cast<FloatType> (1.0) / std::asinh (inputGain);
        }
        else if constexpr (std::is_same<SaturationType, hyperbolicTangentTag>::value)
        {
            return static_cast<FloatType> (1.0) / hyperbolicTangent (inputGain);
        }
        else
        {
            static_assert (tote_bag::type_helpers::dependent_false<SaturationType>::value, "Unsupported saturation type.");
        }
    }

    Type saturationType;

    float asymmetry { 0.0f };

    const float gainRampSec { .005f };

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> smoothedSat { 1.0f };
    juce::AudioBuffer<float> xState;
    std::array<float, 2> antiderivState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Saturation)
};
