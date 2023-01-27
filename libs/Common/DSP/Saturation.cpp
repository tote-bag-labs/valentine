/*
  ==============================================================================

    Saturation.cpp
    Created: 16 Aug 2019 1:24:47pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#include "Saturation.h"
#include "Common/DSP/AudioHelpers.h"

Saturation::Saturation (Type sType, double asymm)
    : saturationType (sType)
    , asymmetry (asymm)
{
}

void Saturation::setParams (double inSaturation)
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

inline double Saturation::calcGain (double inputSample, double sat)
{
    return ((inputSample >= 0.0 && asymmetry > 0.0) || (inputSample < 0.0 && asymmetry < 0.0)) ? sat * (1.0 + 4.0 * abs (asymmetry)) : sat;
}

//===============================================================================
inline double Saturation::inverseHyperbolicSine (double x, double gain)
{
    double xScaled = x * gain;
    return log (xScaled + sqrt (xScaled * xScaled + 1.0)) / gain;
}

inline float Saturation::inverseHyperbolicSineInterp (float x, float gain, int channel)
{
    auto stateSample = xState.getSample (channel, 0);
    auto diff = x - stateSample;

    auto antiDeriv = invHypeSineAntiDeriv (x);
    auto output = 0.0f;
    if (abs (diff) < 0.001f)
    {
        auto input = (x + stateSample) / 2.f;
        output = inverseHyperbolicSine (input, 1.0f);
    }
    else
    {
        output = (antiDeriv - antiderivState[channel]) / diff;
    }

    xState.setSample (channel, 0, x);
    antiderivState[channel] = antiDeriv;

    return output;
}

inline float Saturation::invHypeSineAntiDeriv (float x)
{
    return x * inverseHyperbolicSine (x, 1.0f) - sqrt (x * x + 1.0);
}

inline float Saturation::sineArcTangent (float x, float gain)
{
    float xScaled = x * gain;
    return (xScaled / sqrt (1.f + xScaled * xScaled)) / gain;
}

inline double Saturation::hyperbolicTangent (double x, double gain, int channel)
{
    double xScaled = x * gain;
    return std::tanh (xScaled) / gain;
}

inline double Saturation::interpolatedHyperbolicTangent (double x, double gain, int channel)

{
    auto stateSample = xState.getSample (channel, 0);
    auto diff = x - stateSample;
    auto output = 0.0f;
    auto antiDeriv = hyperTanFirstAntiDeriv (x);

    if (abs (diff) < 0.001)
    {
        auto input = (x + stateSample) / 2.f;
        output = hyperbolicTangent (input, 1.0f, channel);
    }
    else
    {
        output = (antiDeriv - antiderivState[channel]) / diff;
    }

    xState.setSample (channel, 0, x);
    antiderivState[channel] = antiDeriv;

    return output;
}

double Saturation::hyperTanFirstAntiDeriv (double x)
{
    return std::log (tote_bag::audio_helpers::ClampedCosh (x));
}

//===============================================================================

inline double Saturation::processSample (double inputSample, int channel, double sat)
{
    auto gain = calcGain (inputSample, sat);

    // mod matrix?
    // function pointer?
    // branching can prevent optimization
    // store a pointer to a processing class

    switch (saturationType)
    {
        case Type::inverseHyperbolicSine:
            return inverseHyperbolicSine (inputSample, gain);

        case Type::sineArcTangent:
            return sineArcTangent (inputSample, gain);

        case Type::hyperbolicTangent:
            return hyperbolicTangent (inputSample, gain, channel);

        case Type::inverseHyperbolicSineInterp:
            return inverseHyperbolicSineInterp (inputSample * gain, gain, channel) / gain;

        case Type::interpolatedHyperbolicTangent:
            return interpolatedHyperbolicTangent (inputSample * gain, gain, channel) / gain;

        default:
            //somehow the distortion type was not set. It must be set!
            jassert (false);
    }
}

void Saturation::processBlock (juce::dsp::AudioBlock<float>& inAudio)
{
    auto numChannels = inAudio.getNumChannels();
    auto samplesPerBlock = inAudio.getNumSamples();

    for (int i = 0; i < samplesPerBlock; ++i)
    {
        for (int j = 0; j < numChannels; ++j)
        {
            inAudio.setSample (j,
                               i,
                               processSample (inAudio.getSample (j, i),
                                              j,
                                              smoothedSat.getNextValue()));
        }
    }
}
