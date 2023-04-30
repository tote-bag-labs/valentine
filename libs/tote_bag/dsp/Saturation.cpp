/*
  ==============================================================================

    Saturation.cpp
    Created: 16 Aug 2019 1:24:47pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#include "Saturation.h"
#include "tote_bag/dsp/AudioHelpers.h"

Saturation::Saturation (Type sType, float asymm)
    : saturationType (sType)
    , asymmetry (asymm)
{
}

void Saturation::setParams (float inSaturation)
{
    smoothedSat.setTargetValue (inSaturation);
}

void Saturation::reset (double sampleRate)
{
    if (xState.getNumChannels() > 0)
        xState.clear();

    else if (saturationType == Type::inverseHyperbolicSineInterp || saturationType == Type::interpolatedHyperbolicTangent)
    {
        xState.setSize (2, 1);
        xState.clear();
        antiderivState.fill (0.0);
    }

    smoothedSat.reset (sampleRate, gainRampSec);
}

inline float Saturation::calcGain (float inputSample, float sat)
{
    return ((inputSample >= 0.0f && asymmetry > 0.0f) || (inputSample < 0.0f && asymmetry < 0.0f)) ? sat * (1.0f + 4.0f * abs (asymmetry)) : sat;
}

//===============================================================================
inline float Saturation::invHypeSineAntiDeriv (float x)
{
    return x * std::asinh (x) - sqrt (x * x + 1.0f);
}

inline float Saturation::inverseHyperbolicSineInterp (float x, size_t channel)
{
    auto stateSample = xState.getSample (static_cast<int> (channel), 0);
    auto diff = x - stateSample;

    auto antiDeriv = invHypeSineAntiDeriv (x);
    auto output = 0.0f;
    if (abs (diff) < 1.0e-4f)
    {
        auto input = (x + stateSample) / 2.f;
        output = std::asinh (input);
    }
    else
    {
        output = (antiDeriv - antiderivState[channel]) / diff;
    }

    xState.setSample (static_cast<int> (channel), 0, x);
    antiderivState[channel] = antiDeriv;

    return output;
}

inline float Saturation::sineArcTangent (float x, float gain)
{
    float xScaled = x * gain;
    return (xScaled / sqrt (1.f + xScaled * xScaled)) / gain;
}

inline float Saturation::hyperbolicTangent (float x)
{
    return std::tanh (x);
}

float Saturation::hyperTanFirstAntiDeriv (float x)
{
    using namespace tote_bag::audio_helpers;

    // Casting to double is necessary to avoid overflow in the exponential function
    const auto x1 = clampedCosh (static_cast<double> (x));
    const auto x2 = std::log (x1);

    return static_cast<float> (x2);
}

inline float Saturation::interpolatedHyperbolicTangent (float x, size_t channel)

{
    auto stateSample = xState.getSample (static_cast<int> (channel), 0);
    auto diff = x - stateSample;
    auto output = 0.0f;
    auto antiDeriv = hyperTanFirstAntiDeriv (x);

    if (abs (diff) < 0.001)
    {
        auto input = (x + stateSample) / 2.f;
        output = hyperbolicTangent (input);
    }
    else
    {
        output = (antiDeriv - antiderivState[channel]) / diff;
    }

    xState.setSample (static_cast<int> (channel), 0, x);
    antiderivState[channel] = antiDeriv;

    return output;
}

//===============================================================================

inline float Saturation::processSample (float inputSample, size_t channel, float sat)
{
    auto gain = calcGain (inputSample, sat);

    // mod matrix?
    // function pointer?
    // branching can prevent optimization
    // store a pointer to a processing class

    switch (saturationType)
    {
        case Type::inverseHyperbolicSine:
            return std::asinh (inputSample * gain) * compensationGain<inverseHyperbolicSineTag> (gain);

        case Type::sineArcTangent:
            return sineArcTangent (inputSample, gain);

        case Type::hyperbolicTangent:
            return hyperbolicTangent (inputSample * gain) * compensationGain<hyperbolicTangentTag> (gain);

        case Type::inverseHyperbolicSineInterp:
            return inverseHyperbolicSineInterp (inputSample * gain, channel) * compensationGain<inverseHyperbolicSineTag> (gain);

        case Type::interpolatedHyperbolicTangent:
            return interpolatedHyperbolicTangent (inputSample * gain, channel) * compensationGain<hyperbolicTangentTag> (gain);

        default:
            //somehow the distortion type was not set. It must be set!
            jassert (false);
    }
}

void Saturation::processBlock (juce::dsp::AudioBlock<float>& inAudio)
{
    auto numChannels = inAudio.getNumChannels();
    auto samplesPerBlock = inAudio.getNumSamples();

    for (size_t i = 0; i < samplesPerBlock; ++i)
    {
        for (size_t j = 0; j < numChannels; ++j)
        {
            auto input = inAudio.getChannelPointer (j);
            auto x = input[i];

            auto y = processSample (x, j, smoothedSat.getNextValue());

            input[i] = y;
        }
    }
}
