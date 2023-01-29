/*
  ==============================================================================

    SFCComp.cpp
    Created: 29 Jun 2019 9:12:06pm
    Author:  dev

  ==============================================================================
*/
#include "Compressor.h"

Compressor::Compressor (bool autoRelease, float knee)
    : autoReleaseFlag (autoRelease)
{
}

void Compressor::setSampleRate (double inSampleRate)
{
    levelDetector.setSampleRate (inSampleRate);
}

void Compressor::setMeterSource (FFAU::LevelMeterSource* source)
{
    if (source != nullptr && meterSource == nullptr)
        meterSource = source;
}

void Compressor::setAttack (float inAttack)
{
    levelDetector.setTimeConstant (inAttack, true);
}

void Compressor::setRelease (float inRelease)
{
    levelDetector.setTimeConstant (inRelease, false);
}

void Compressor::setRatio (float inRatio)
{
    ratio.set (inRatio);
}

void Compressor::setKnee (float inKnee)
{
    knee.set (inKnee);
}

void Compressor::setThreshold (float inThreshold)
{
    threshold.set (inThreshold);
}

void Compressor::reset (int samplesPerBlock)
{
    analysisSignal.setSize (1, samplesPerBlock, false, false, true);
    analysisSignal.clear();
    levelDetector.reset();
}

void Compressor::setOversampleMultiplier (const int o)
{
    overSampleMultiplier = o - 1;
}

inline void Compressor::makeMonoSidechain (const juce::dsp::AudioBlock<float>& inAudio, juce::AudioBuffer<float>& scSignal)
{
    scSignal.clear();
    auto* scWritePointer = scSignal.getWritePointer (0);

    auto numChannels = inAudio.getNumChannels();
    auto numSamples = static_cast<int> (inAudio.getNumSamples());

    for (int channel = 0; channel < numChannels; ++channel)
    {
        juce::FloatVectorOperations::addWithMultiply (scWritePointer,
                                                      inAudio.getChannelPointer (channel),
                                                      1.0f / numChannels,
                                                      numSamples);
    }
}

inline float Compressor::calculateGain (float inputSample)
{
    auto T = threshold.get();
    jassert (T != -1.0f);

    auto W = knee.get();
    jassert (W != -1.0f);

    auto overshoot = inputSample - T;
    auto doubleOvershoot = 2.0f * overshoot;
    auto R = ratio.get();
    auto cvOutput = 0.f;

    if (W > 0.0)
    {
        if (doubleOvershoot < -W)
        {
            cvOutput = inputSample;
        }

        else if (abs (doubleOvershoot) <= W)
        {
            cvOutput = inputSample + ((1.0f / R - 1.0f) * pow (overshoot + W / 2.f, 2)) / (2.f * W);
        }
        else
        {
            cvOutput = T + overshoot / R;
        }
    }
    else
    {
        cvOutput = inputSample > T ? T + overshoot / R : inputSample;
    }

    return -(inputSample - cvOutput);
}

inline float Compressor::getMakeupGain()
{
    return -threshold.get() * (1.0f / ratio.get() - 1.0f) / 2.0f;
}

void Compressor::process (juce::dsp::AudioBlock<float>& inAudio)
{
    makeMonoSidechain (inAudio, analysisSignal);
    auto numSamples = inAudio.getNumSamples();
    auto numChannels = inAudio.getNumChannels();

    for (int sample = 0; sample < numSamples; sample++)
    {
        auto controlVoltage = juce::Decibels::gainToDecibels (abs (analysisSignal.getSample (0, sample)));
        controlVoltage = levelDetector.processSampleDecoupledBranched (controlVoltage);

        controlVoltage = calculateGain (controlVoltage);
        controlVoltage = juce::Decibels::decibelsToGain (controlVoltage);

        meterSource->setReductionLevel (controlVoltage);

        analysisSignal.setSample (0, sample, controlVoltage);
    }

    for (int channel = 0; channel < numChannels; channel++)
    {
        juce::FloatVectorOperations::multiply (inAudio.getChannelPointer (channel),
                                               inAudio.getChannelPointer (channel),
                                               analysisSignal.getReadPointer (0),
                                               static_cast<int> (numSamples));
    }
}
