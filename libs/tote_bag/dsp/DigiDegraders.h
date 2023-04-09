/*
  ==============================================================================

    SimpleZOH.h
    Created: 2 Sep 2019 10:40:50am
    Author:  dev

  ==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include <atomic>
#include <cmath>
#include <cstdlib>
#include <limits>

class SimpleZOH
{
public:
    SimpleZOH() {}

    void setResampleOffset (double inHostSr);
    void setParams (float inDownsampleCoeff);
    void processBlock (juce::AudioBuffer<float>& inAudio, int samplesPerBlock, int numChannels);
    float getZOHSample (const float* channelData, int sampleIndex, int dsCoef);

private:
    float resampleOffset { -1 },
        downsampleCoeff { 1.0f };
    bool sampleRateChanged { true };
    const double refSampleRate { 44100 };
    juce::Atomic<double> currentHostSampleRate;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleZOH)
};

//====================================================================================

class Bitcrusher
{
public:
    Bitcrusher() {}

    void setParams (float inBitDepth);
    void processBlock (juce::AudioBuffer<float>& inAudio, int samplesPerBlock, int numChannels);

    /** Returns a bitcrushed sample—with a twist.
     *
     *  The typical calculation for a uniform bitcrusher is:
     *
     *  quantization = 1 / (2 ^ bits)
     *  output = quantization * floor((input / quantization) + 0.5)
     *
     * This makes the quantization (nearest step) the same for negative
     * and positive numbers.
     *
     * Omitting the 0.5 results in quantization (nearest lesser step) that
     * effectively makes the signal greater for negative values and smaller
     * for positive values.
     *
     * It also sounds very cool.
     *
     */
    float asymmetricBitcrush (float inputSample, float bits);

private:
    juce::Atomic<float> bitDepth { 16.0f };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Bitcrusher)
};
