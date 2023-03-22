/*
  ==============================================================================

    SFCComp.h
    Created: 29 Jun 2019 9:12:06pm
    Author:  dev

  ==============================================================================
*/
#pragma once

#include "EnvelopeDetector.h"

#include <ff_meters/ff_meters.h>
#include <juce_dsp/juce_dsp.h>

class Compressor
{
public:
    Compressor (bool autoRelease, float knee);

    Compressor (bool autoRelease);

    void reset (int numSamplesPerBlock);

    void setSampleRate (double inSampleRate);

    void setParams (float inAttack,
                    float inRelease,
                    float inRatio,
                    float inThreshold,
                    float inKnee);

    void makeMonoSidechain (const juce::dsp::AudioBlock<float>& inAudio, juce::AudioBuffer<float>& scSignal);

    void makeKneeCoeffs();

    float calculateGain (float analysisSample);

    void process (juce::dsp::AudioBlock<float>& inAudio);

    float getMakeupGain();

    void setAttack (float inAttack);
    void setRelease (float inRelease);
    void setRatio (float inRatio);
    void setKnee (float inKnee);
    void setThreshold (float inThreshold);

    void setMeterSource (FFAU::LevelMeterSource* source);
    void setOversampleMultiplier (const int o);

private:
    juce::WeakReference<FFAU::LevelMeterSource> meterSource;

    juce::Atomic<float> ratio { -1.0f },
        knee { -1.0f },
        msAttack { -1.0f },
        msRelease { -1.0f },
        threshold { -1.0 };

    EnvelopeDetector levelDetector;
    juce::AudioBuffer<float> analysisSignal;

    int overSampleMultiplier { 1 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Compressor)
};
