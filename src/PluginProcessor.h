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
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

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
    const int getSavedGUIwidth() { return savedWidth.get(); }

    FFAU::LevelMeterSource& getInputMeterSource() { return inputMeterSource; }
    FFAU::LevelMeterSource& getOutputMeterSource() { return outputMeterSource; }
    FFAU::LevelMeterSource& getGrMeterSource() { return grMeterSource; }

    /** Called in Editor destructor to save the gui width
     */
    void saveGUIwidth (const int w) { savedWidth.set (w); }

    juce::AudioProcessorValueTreeState treeState;

    ToteBagPresetManager& getPresetManager() { return presetManager; }

private:
    ToteBagPresetManager presetManager;

    FFAU::LevelMeterSource inputMeterSource;
    FFAU::LevelMeterSource outputMeterSource;
    FFAU::LevelMeterSource grMeterSource;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    bool presetIsLoading = false;

    using Oversampling = juce::dsp::Oversampling<float>;

    const int oversampleFactor { 2 };
    int circBuffDelay { -1 };

    const float boundedSatGain { 1.0f },
        downSampleRate { 27500.0f },
        RMStime { 50.0f };

    // drop gain before processing...maybe
    const float internalBias { juce::Decibels::decibelsToGain (-9.f) };
    const float invInternalBias { 1.0f / internalBias };

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> dryWet;

    const double dryWetRampLength { .10 }; // in seconds, used in .reset()

    // used to maintain state for ApplyGainRamp
    // compress and makeup are addressed in dB, interface-wise, but handled linearly here
    float smoothedGain { 1.0f },
        currentGain { juce::Decibels::decibelsToGain (FFCompParameterDefaults[getParameterIndex (VParameter::inputGain)]) },
        currentMakeup { juce::Decibels::decibelsToGain (FFCompParameterDefaults[getParameterIndex (VParameter::makeupGain)]) },
        currentNiceGain { 1.0f };

    // Used to index into ratio array
    size_t ratioIndex = 0;
    // Raises compressor threshold setting when Nice mode is on
    static constexpr float kNiceOffset = 9.0f;
    // Determines whether offset is used when setting compressor threshold
    bool niceModeOn = false;

    // compress and makeup are addressed in dB, interface-wise, but handled linearly here
    juce::Atomic<float> compressValue { juce::Decibels::decibelsToGain (FFCompParameterDefaults[getParameterIndex (VParameter::inputGain)]) },
        mixValue { 1.0f },
        makeupValue { juce::Decibels::decibelsToGain (FFCompParameterDefaults[getParameterIndex (VParameter::makeupGain)]) },
        currentWidth { 0.0f },
        currentHeight { 0.0f },
        newNiceGain { 1.0f },
        newNiceInvGain { 1.0f };

    juce::Atomic<int> savedWidth { 0 };

    juce::Atomic<bool> crushOn { false };
    juce::Atomic<bool> bypassOn { false };

    // This is used for, yes you guessed it, processing
    juce::AudioBuffer<float> processBuffer;

    // DSP objects
    std::array<std::unique_ptr<CircularBuffer<float>>, 2> unProcBuffers;
    std::array<std::unique_ptr<ThiranAllPass<double>>, 2> fracDelayFilters;
    std::unique_ptr<tote_bag::audio_helpers::SimpleOnePole<float>> dryWetFilter;
    std::unique_ptr<Oversampling> oversampler;
    std::unique_ptr<Compressor> ffCompressor;
    std::unique_ptr<Saturation> saturator;
    std::unique_ptr<Saturation> boundedSaturator;
    std::unique_ptr<SimpleZOH> simpleZOH;
    std::unique_ptr<Bitcrusher> bitCrush;

    juce::MessageManager* testManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValentineAudioProcessor)
};
