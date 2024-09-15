/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "tote_bag/utils/tbl_math.hpp"

//==============================================================================

namespace detail
{
inline constexpr float kNeg6dbGain = 0.5011872336f;
inline constexpr int kOversampleFactor = 2;
inline constexpr float kDownSampleRate = 27500.0f;
inline constexpr float kRmsTime = 50.0f;
inline constexpr double kDryWetRampLength = .10;

/** Returns a juce::String for a given value, with
 *  the level of precision adjusted based on the value
 *  size.
 */
inline juce::String getPrecisionAdjustedValueString (float value)
{
    const auto absValue = std::abs (value);

    if (absValue < 10.0f)
    {
        return juce::String (absValue < .001 ? 0.0f : value, 2);
    }
    if (absValue < 100.0f)
    {
        return juce::String (value, 1);
    }
    return juce::String (juce::roundToInt (value));
}

inline std::function<juce::String (float, int)>
    makeStringFromValueFunction (VParameter param)
{
    if ((param == VParameter::attack) || (param == VParameter::release)
        || (param == VParameter::inputGain) || (param == VParameter::makeupGain))
    {
        return [] (float value, int) { return getPrecisionAdjustedValueString (value); };
    }
    if (param == VParameter::ratio)
    {
        return [] (float value, int) {
            const auto absValue = std::abs (value);

            if (absValue >= FFCompParameterMax[static_cast<size_t> (VParameter::ratio)])
            {
                return juce::String (juce::CharPointer_UTF8 ("∞"));
            }
            return getPrecisionAdjustedValueString (value);
        };
    }
    else
    {
        return [] (float value, int) { return juce::String (juce::roundToInt (value)); };
    }
}
} // namespace detail

ValentineAudioProcessor::ValentineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
                          .withInput ("Sidechain", juce::AudioChannelSet::stereo(), true)
        #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                          )
    , treeState (*this, nullptr, "PARAMETER", createParameterLayout())
    , presetManager (this)
    , processedDelayLine (32)
    , cleanDelayLine (32)
#endif
{
    initializeDSP();

    // sign up parameter listeners
    for (auto param : tote_bag::valentine::parameterIDs())
    {
        treeState.addParameterListener (param, this);
    }

    //initialize processor params
    const auto defaultRatio =
        FFCompParameterDefaults[static_cast<size_t> (VParameter::ratio)];
    ffCompressor->setRatio (defaultRatio);

    const auto kneeValue = tote_bag::math::piecewiseRemap (kRatioControlPoints,
                                                           kKneeControlPoints,
                                                           defaultRatio);
    ffCompressor->setKnee (kneeValue);

    const auto thresholdValue = tote_bag::math::piecewiseRemap (kRatioControlPoints,
                                                                kThresholdControlPoints,
                                                                defaultRatio);
    ffCompressor->setThreshold (thresholdValue);

    bitCrush->setParams (kMaxBits);
    saturator->setParams (kMinSaturationGain);
}

ValentineAudioProcessor::~ValentineAudioProcessor()
{
    for (auto param : tote_bag::valentine::parameterIDs())
    {
        treeState.removeParameterListener (param, this);
    }
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
    ValentineAudioProcessor::createParameterLayout()
{
    using namespace tote_bag;

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (size_t i = 0; i < numParams; ++i)
    {
        const auto paramType = static_cast<VParameter> (i);

        if (paramType == VParameter::bypass || paramType == VParameter::outputClipEnable
            || paramType == VParameter::crushEnable
            || paramType == VParameter::saturateEnable)
        {
            const bool defaultValue = FFCompParameterDefaults[i] > 0.5f;

            params.push_back (std::make_unique<juce::AudioParameterBool> (
                juce::ParameterID {valentine::parameterID (i), ValentineParameterVersion},
                FFCompParameterLabel()[i],
                defaultValue));
        }
        else
        {
            auto rangeToUse =
                juce::NormalisableRange<float> (FFCompParameterMin[i],
                                                FFCompParameterMax[i],
                                                FFCompParameterIncrement[i]);
            rangeToUse.setSkewForCentre (FFCompParamCenter[i]);

            auto stringFromValue = detail::makeStringFromValueFunction (paramType);

            params.push_back (std::make_unique<juce::AudioParameterFloat> (
                juce::ParameterID {valentine::parameterID (i), ValentineParameterVersion},
                FFCompParameterLabel()[i],
                rangeToUse,
                FFCompParameterDefaults[i],
                VParameterUnit()[i],
                juce::AudioProcessorParameter::genericParameter,
                std::move (stringFromValue),
                nullptr));
        }
    }

    return {params.begin(), params.end()};
}

//==============================================================================

const juce::String ValentineAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ValentineAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ValentineAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ValentineAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ValentineAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ValentineAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int ValentineAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ValentineAudioProcessor::setCurrentProgram (int)
{
}

const juce::String ValentineAudioProcessor::getProgramName (int)
{
    return {};
}

void ValentineAudioProcessor::changeProgramName (int, const juce::String&)
{
}

//==============================================================================
void ValentineAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    using namespace tote_bag;

    const auto oversampleMultiplier =
        static_cast<int> (pow (2, detail::kOversampleFactor));

    processBuffer.setSize (2, samplesPerBlock);
    processBuffer.clear();

    ffCompressor->setAttack (
        *treeState.getRawParameterValue (valentine::parameterID (VParameter::attack)));
    ffCompressor->setRelease (
        *treeState.getRawParameterValue (valentine::parameterID (VParameter::release)));
    ffCompressor->setSampleRate (sampleRate * oversampleMultiplier);
    ffCompressor->reset (samplesPerBlock * oversampleMultiplier);
    ffCompressor->setOversampleMultiplier (oversampleMultiplier);

    oversampler->reset();
    oversampler->initProcessing (static_cast<size_t> (samplesPerBlock));

    saturator->reset (sampleRate);
    boundedSaturator->reset (sampleRate);
    simpleZOH->setParams (static_cast<float> (sampleRate / detail::kDownSampleRate));

    updateLatencyCompensation (true);

    dryWet.reset (sampleRate, detail::kDryWetRampLength);

    const auto rmsWindow =
        juce::roundToInt (detail::kRmsTime * 0.001f * sampleRate / samplesPerBlock);
    inputMeterSource.resize (getMainBusNumInputChannels(), rmsWindow);

    grMeterSource.resize (1, rmsWindow);
    ffCompressor->setMeterSource (&grMeterSource);

    outputMeterSource.resize (getTotalNumOutputChannels(), rmsWindow);
}

void ValentineAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ValentineAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
    #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

            // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

void ValentineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                            juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto numMainBusChannels = getMainBusNumInputChannels();

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    auto mainInputOutputBuffer = getBusBuffer (buffer, true, 0);
    auto sidechainBuffer = getBusBuffer (buffer, true, 1);

    auto bufferSize = buffer.getNumSamples();
    auto currentSamplesPerBlock = bufferSize;

    if (latencyChanged.get())
    {
        updateLatencyCompensation (false);
        latencyChanged.set (false);
    }

    // We need to prepare the process buffer first. The input buffer is delayed to account for
    // oversampling latency. Copying into the process buffer after this, then, would undo the
    // purpose of the delay: maintaining phase coherence between processed and unprocessed
    // signals.
    processBuffer.setSize (numMainBusChannels, currentSamplesPerBlock, true, true, true);
    for (auto channel = 0; channel < numMainBusChannels; ++channel)
        processBuffer.copyFrom (channel,
                                0,
                                buffer.getReadPointer (channel),
                                buffer.getNumSamples());

    prepareInputBuffer (buffer,
                        numMainBusChannels,
                        totalNumOutputChannels,
                        currentSamplesPerBlock);

    if (bypassOn.get())
    {
        return;
    }

    inputMeterSource.measureBlock (sidechainBuffer);

    // "Downsample" and Bitcrush processing
    if (crushOn.get())
    {
        simpleZOH->processBlock (processBuffer,
                                 currentSamplesPerBlock,
                                 totalNumOutputChannels);

        bitCrush->processBlock (processBuffer,
                                currentSamplesPerBlock,
                                totalNumOutputChannels);
    }

    auto g = compressValue.get();
    for (int i = 0; i < totalNumOutputChannels; ++i)
        processBuffer.applyGainRamp (i, 0, bufferSize, currentGain, g);
    currentGain = g;

    // Upsample then do non-linear processing
    juce::dsp::AudioBlock<float> processBlock (processBuffer);
    juce::dsp::AudioBlock<float> highSampleRateBlock =
        oversampler->processSamplesUp (processBlock);

    ffCompressor->process (highSampleRateBlock);

    if (saturateOn.get())
    {
        // Clear the buffers if saturate just got turned back on
        if (!saturateOnState)
        {
            saturator->clearBuffers();
            saturateOnState = true;
        }
        saturator->processBlock (highSampleRateBlock);
    }

    if (clipOn.get())
    {
        // Clear the buffers if clip just got turned back on
        if (!clipOnState)
        {
            boundedSaturator->clearBuffers();
            clipOnState = true;
        }
        boundedSaturator->processBlock (highSampleRateBlock);
    }

    oversampler->processSamplesDown (processBlock);

    // Delay processed signal to produce a integral delay amount
    const auto blockLength = static_cast<int> (processBlock.getNumSamples());
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        for (int sample = 0; sample < blockLength; ++sample)
        {
            const auto x = processBlock.getSample (channel, sample);
            processedDelayLine.pushSample (channel, x);

            const auto delay =
                processedDelayTime[static_cast<size_t> (channel)].getNextValue();
            const auto y = processedDelayLine.popSample (channel, delay);
            processBlock.setSample (channel, sample, y);
        }
    }

    // Apply Makeup
    auto m = makeupValue.get();
    for (int i = 0; i < totalNumOutputChannels; ++i)
    {
        processBuffer.applyGainRamp (i, 0, bufferSize, currentMakeup, m);
    }
    currentMakeup = m;

    // Get and apply mix
    auto mix = mixValue.get();
    dryWet.setTargetValue (mix);
    for (int j = 0; j < currentSamplesPerBlock; ++j)
    {
        const auto currentMix = dryWet.getNextValue();

        for (int i = 0; i < totalNumOutputChannels; ++i)
        {
            auto processed = processBuffer.getSample (i, j);
            auto unprocessed = buffer.getSample (i, j);

            buffer.setSample (i, j, mix * processed + (1.0f - currentMix) * unprocessed);
        }
    }

    outputMeterSource.measureBlock (buffer);
}

void ValentineAudioProcessor::processBlockBypassed (juce::AudioBuffer<float>& buffer,
                                                    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto currentSamplesPerBlock = buffer.getNumSamples();

    prepareInputBuffer (buffer,
                        totalNumInputChannels,
                        totalNumOutputChannels,
                        currentSamplesPerBlock);
}

void ValentineAudioProcessor::prepareInputBuffer (juce::AudioBuffer<float>& buffer,
                                                  const int numInputChannels,
                                                  const int numOutputChannels,
                                                  const int samplesPerBlock)
{
    buffer.setSize (numOutputChannels, samplesPerBlock, true, true, true);

    for (auto i = numInputChannels; i < numOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    const auto numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < numOutputChannels; ++channel)
    {
        for (int sample = 0; sample < numSamples; ++sample)
        {
            const auto x = buffer.getSample (channel, sample);
            cleanDelayLine.pushSample (channel, x);

            const auto y = cleanDelayLine.popSample (channel, cleanBufferDelay);
            buffer.setSample (channel, sample, y);
        }
    }
}

//==============================================================================
bool ValentineAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ValentineAudioProcessor::createEditor()
{
    return new ValentineAudioProcessorEditor (*this);
}

//==============================================================================

void ValentineAudioProcessor::parameterChanged (const juce::String& parameter,
                                                float newValue)
{
    if (parameter == "Crush")
    {
        const auto bitCrushIndex = static_cast<int> (VParameter::bitCrush);
        bitCrush->setParams (juce::jmap (newValue,
                                         FFCompParameterMin[bitCrushIndex],
                                         FFCompParameterMax[bitCrushIndex],
                                         kMaxBits,
                                         kMinBits));
    }
    else if (parameter == "Saturate")
    {
        const auto saturateIndex = static_cast<int> (VParameter::saturation);
        saturator->setParams (juce::jmap (newValue,
                                          FFCompParameterMin[saturateIndex],
                                          FFCompParameterMax[saturateIndex],
                                          kMinSaturationGain,
                                          kMaxSaturationGain));
    }
    else if (parameter == "AttackTime")
    {
        ffCompressor->setAttack (newValue);
    }
    else if (parameter == "ReleaseTime")
    {
        ffCompressor->setRelease (newValue);
    }
    else if (parameter == "Ratio")
    {
        if (newValue >= FFCompParameterMax[static_cast<size_t> (VParameter::ratio)])
        {
            ffCompressor->setRatio (
                FFCompParameterMax[static_cast<size_t> (VParameter::ratio)]);
            ffCompressor->setKnee (kKneeMax);
            ffCompressor->setThreshold (kThresholdMax);
        }

        ffCompressor->setRatio (newValue);

        auto kneeValue = tote_bag::math::piecewiseRemap (kRatioControlPoints,
                                                         kKneeControlPoints,
                                                         newValue);
        ffCompressor->setKnee (kneeValue);
        auto thresholdValue = tote_bag::math::piecewiseRemap (kRatioControlPoints,
                                                              kThresholdControlPoints,
                                                              newValue);

        ffCompressor->setThreshold (thresholdValue);
    }
    else if (parameter == "Compress")
    {
        compressValue.set (juce::Decibels::decibelsToGain (newValue));
    }
    else if (parameter == "Mix")
    {
        mixValue.set (newValue * .01f);
    }
    else if (parameter == "Makeup")
    {
        makeupValue.set (juce::Decibels::decibelsToGain (newValue));
    }
    else if (parameter == "Bypass")
    {
        bypassOn.set (newValue > 0.5f);
    }
    else if (parameter == "OutputClipEnable")
    {
        clipOn.set (newValue > 0.5f);
        latencyChanged.set (true);
    }
    else if (parameter == "CrushEnable")
    {
        crushOn.set (newValue > 0.5f);
    }
    else if (parameter == "SaturateEnable")
    {
        saturateOn.set (newValue > 0.5f);
        latencyChanged.set (true);
    }
}

//==============================================================================
void ValentineAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = treeState.copyState();

    auto currentPresetName = presetManager.getCurrentPresetName();

    if (currentPresetName.isNotEmpty())
        state.setProperty (juce::Identifier ("PresetName"), currentPresetName, nullptr);

    std::unique_ptr<juce::XmlElement> xml (state.createXml());

    copyXmlToBinary (*xml, destData);
}

void ValentineAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName (treeState.state.getType()))
        {
            auto newTree = juce::ValueTree::fromXml (*xmlState);

            auto presetID = juce::Identifier ("PresetName");
            if (newTree.hasProperty (presetID))
            {
                presetManager.setLastChosenPresetName (
                    newTree.getPropertyAsValue ("PresetName", nullptr).toString());
                newTree.removeProperty (presetID, nullptr);
            }

            presetIsLoading = true;
            treeState.replaceState (newTree);
            presetIsLoading = false;
        }
    }
}

//==============================================================================

void ValentineAudioProcessor::initializeDSP()
{
    ffCompressor = std::make_unique<Compressor> (false);

    saturator =
        std::make_unique<Saturation> (Saturation::Type::inverseHyperbolicSineInterp, .6f);

    boundedSaturator = std::make_unique<Saturation> (Saturation::Type::hyperbolicTangent);
    boundedSaturator->setParams (detail::kNeg6dbGain);

    oversampler =
        std::make_unique<Oversampling> (2,
                                        detail::kOversampleFactor,
                                        Oversampling::filterHalfBandPolyphaseIIR);
    simpleZOH = std::make_unique<SimpleZOH>();
    bitCrush = std::make_unique<Bitcrusher>();
}

//==============================================================================

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ValentineAudioProcessor();
}

//==============================================================================

void ValentineAudioProcessor::updateLatencyCompensation (bool init)
{
    // First order ADAA
    const float maximumSaturatorLatency = 0.5f;
    const float maximumClipperLatency = 0.5f;

    overSamplingLatency = oversampler->getLatencyInSamples();

    const auto maximumDelay =
        overSamplingLatency + maximumSaturatorLatency + maximumClipperLatency;
    cleanBufferDelay = static_cast<int> (std::ceil (maximumDelay));

    const auto numOutputChannels = getTotalNumOutputChannels();
    if (init)
    {
        // We report the maximum possible latency and adjust the delays
        // to maintain it
        setLatencySamples (cleanBufferDelay);

        juce::dsp::ProcessSpec delaySpec {getSampleRate(),
                                          static_cast<juce::uint32> (getBlockSize()),
                                          static_cast<juce::uint32> (numOutputChannels)};

        processedDelayLine.prepare (delaySpec);
        cleanDelayLine.prepare (delaySpec);
    }

    const auto currentClipperLatency = clipOn.get() ? maximumClipperLatency : 0.0f;
    const auto currentSaturatorLatency =
        saturateOn.get() ? maximumSaturatorLatency : 0.0f;
    const auto currentDelay =
        overSamplingLatency + currentSaturatorLatency + currentClipperLatency;
    const auto processBufferDelay = cleanBufferDelay - currentDelay;

    for (size_t channel = 0; channel < static_cast<size_t> (numOutputChannels); ++channel)
    {
        if (init)
        {
            processedDelayTime[channel].reset (getSampleRate(), 1.0f);
            processedDelayTime[channel].setCurrentAndTargetValue (processBufferDelay);
        }
        else
        {
            processedDelayTime[channel].setTargetValue (processBufferDelay);
        }
    }
}
