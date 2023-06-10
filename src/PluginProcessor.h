/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "ValentineParameters.h"

#include "tote_bag/dsp/CircularBuffer.h"
#include "tote_bag/dsp/Compressor.h"
#include "tote_bag/dsp/DigiDegraders.h"
#include "tote_bag/dsp/Saturation.h"
#include "tote_bag/dsp/ThiranAllpass.h"
#include "tote_bag/juce_gui/managers/ToteBagPresetManager.h"

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
*/

class ValentineAudioProcessor : public juce::AudioProcessor,
                                public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    ValentineAudioProcessor();
    ~ValentineAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void processBlockBypassed (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int) override;
    const juce::String getProgramName (int) override;
    void changeProgramName (int, const juce::String&) override;

    //==============================================================================
    void parameterChanged (const juce::String& parameter, float newValue) override;
    void initializeParams (double sampleRate);
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void initializeDSP();

    /** Prepares an input buffer for processing, accounting for possible changes
        to number of input or output channels, block size. Also delays the input buffer
        by the latency reported by the oversampler.
     */
    void prepareInputBuffer (juce::AudioBuffer<float>& buffer,
                             const int numInputChannels,
                             const int numOutputChannels,
                             const int samplesPerBlock);

    /** Returns the saved GUI width. Used by Editor to decide what dimesions
        to use during construction.
     */
    const int getSavedGUIwidth()
    {
        return savedWidth.get();
    }

    FFAU::LevelMeterSource& getInputMeterSource()
    {
        return inputMeterSource;
    }
    FFAU::LevelMeterSource& getOutputMeterSource()
    {
        return outputMeterSource;
    }
    FFAU::LevelMeterSource& getGrMeterSource()
    {
        return grMeterSource;
    }

    /** Called in Editor destructor to save the gui width
     */
    void saveGUIwidth (const int w)
    {
        savedWidth.set (w);
    }

    juce::AudioProcessorValueTreeState treeState;

    ToteBagPresetManager& getPresetManager()
    {
        return presetManager;
    }

private:
    /** Finds and sets the correct processing delay for compensate for latency
     *  caused by processing.
     *
     *  On @p init this will find the maximum reported latency for the system,
     *  report it to the host, and set clean buffer delays to that amount.
     *
     *  In all cases, it will use that maximum reported latency to find the correct
     *  amount of delay to apply to the processed buffer in order to ensure that the
     *  delay at output never changes, and is always integral, which can help hosts
     *  do latency compensation.
     */
    void updateLatencyCompensation (bool init);

    /** This is the maximum system latency, rounded up.
     *  Maximum system latency = oversampling latency + ADAA clipper latency.
     *
     *  Used as both the latency reported to host as well as the target for
     *  processing delay, which is calculated whenever the graph changes.
     *
     *  During processing, this should never change, as any changes to actual
     *  latency will reduce processing latency and therefore can be adjusted by
     *  increasing the processing delay. It may, however, increase if sample
     *  rate changes. So we need to update it when that happens.
     */
    int cleanBufferDelay = 0;

    /** As the name suggests, this is the latency introduced by oversampling.
     *  this should only change if host sample rate changes, so we cache it here
     *  as the clean buffer delay is only updated at that time. Otherwise, we risk
     *  getting the compensation wrong if somehow the oversampler latency changes
     *  without prepareToPlay() being called.
     */
    float overSamplingLatency = 0.0f;

    ToteBagPresetManager presetManager;

    FFAU::LevelMeterSource inputMeterSource;
    FFAU::LevelMeterSource outputMeterSource;
    FFAU::LevelMeterSource grMeterSource;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool presetIsLoading = false;

    using Oversampling = juce::dsp::Oversampling<float>;

    const int oversampleFactor {2};
    int circBuffDelay {-1};

    const float boundedSatGain {1.0f}, downSampleRate {27500.0f}, RMStime {50.0f};

    // drop gain before processing...maybe
    const float internalBias {juce::Decibels::decibelsToGain (-9.f)};
    const float invInternalBias {1.0f / internalBias};

    using SmoothedFloat = juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear>;
    SmoothedFloat dryWet;

    std::array<SmoothedFloat, 2> processedDelayTime;

    using LatencyCompensation =
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd>;
    LatencyCompensation processedDelayLine;
    LatencyCompensation cleanDelayLine;

    const double dryWetRampLength {.10}; // in seconds, used in .reset()

    // used to maintain state for ApplyGainRamp
    // compress and makeup are addressed in dB, interface-wise, but handled linearly here
    float smoothedGain {1.0f},
        currentGain {juce::Decibels::decibelsToGain (
            FFCompParameterDefaults[getParameterIndex (VParameter::inputGain)])},
        currentMakeup {juce::Decibels::decibelsToGain (
            FFCompParameterDefaults[getParameterIndex (VParameter::makeupGain)])};

    // compress and makeup are addressed in dB, interface-wise, but handled linearly here
    juce::Atomic<float> compressValue {juce::Decibels::decibelsToGain (
        FFCompParameterDefaults[getParameterIndex (VParameter::inputGain)])},
        mixValue {1.0f},
        makeupValue {juce::Decibels::decibelsToGain (
            FFCompParameterDefaults[getParameterIndex (VParameter::makeupGain)])},
        currentWidth {0.0f}, currentHeight {0.0f};

    juce::Atomic<int> savedWidth {0};

    juce::Atomic<bool> crushOn {false};
    juce::Atomic<bool> bypassOn {false};
    juce::Atomic<bool> clipOn {
        FFCompParameterDefaults[static_cast<size_t> (VParameter::outputClip)] > 0.0f};
    juce::Atomic<bool> clipOnState = false;

    juce::Atomic<bool> latencyChanged = false;

    // This is used for, yes you guessed it, processing
    juce::AudioBuffer<float> processBuffer;

    // DSP objects
    std::unique_ptr<tote_bag::audio_helpers::SimpleOnePole<float>> dryWetFilter;
    std::unique_ptr<Oversampling> oversampler;
    std::unique_ptr<Compressor> ffCompressor;
    std::unique_ptr<Saturation> saturator;
    std::unique_ptr<Saturation> boundedSaturator;
    std::unique_ptr<SimpleZOH> simpleZOH;
    std::unique_ptr<Bitcrusher> bitCrush;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValentineAudioProcessor)
};
