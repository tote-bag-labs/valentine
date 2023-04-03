/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ValentineAudioProcessor::ValentineAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                          )
    , treeState (*this, nullptr, "PARAMETER", createParameterLayout())
    , presetManager (this)
#endif
{
    initializeDSP();

    // sign up parameter listeners
    for (auto param : FFCompParameterID())
    {
        treeState.addParameterListener (param, this);
    }

    //initialize processor params
    const auto defaultRatioIndex = static_cast<size_t> (FFCompParameterDefaults[getParameterIndex (VParameter::ratio)]);
    ffCompressor->setRatio (ratioValues[defaultRatioIndex]);
    ffCompressor->setKnee (kneeValues[defaultRatioIndex]);
    ffCompressor->setThreshold (thresholdValues[defaultRatioIndex]);
    bitCrush->setParams (17.0);
    saturator->setParams (.001);
    boundedSaturator->setParams (boundedSatGain);
}

ValentineAudioProcessor::~ValentineAudioProcessor()
{
    for (auto param : FFCompParameterID())
    {
        treeState.removeParameterListener (param, this);
    }

    testManager->deleteInstance();
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout
    ValentineAudioProcessor::createParameterLayout()
{
    testManager = juce::MessageManager::getInstance();
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    for (size_t i = 0; i < numParams; ++i)
    {
        const auto paramType = static_cast<VParameter> (i);
        if (paramType == VParameter::ratio)
        {
            juce::StringArray ratioChoices;
            for (const auto ratio : ratioValues)
            {
                ratioChoices.add (juce::String (ratio));
            }

            const auto ratioParameterIndex = getParameterIndex (VParameter::ratio);
            params.push_back (std::make_unique<juce::AudioParameterChoice> (
                juce::ParameterID { FFCompParameterID()[ratioParameterIndex], ValentineParameterVersion },
                FFCompParameterLabel()[ratioParameterIndex],
                ratioChoices,
                FFCompParameterDefaults[ratioParameterIndex]));
        }
        else if (paramType == VParameter::nice || paramType == VParameter::bypass)
        {
            params.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { FFCompParameterID()[i], ValentineParameterVersion },
                                                                          FFCompParameterLabel()[i],
                                                                          false));
        }

        else
        {
            auto rangeToUse = juce::NormalisableRange<float> (FFCompParameterMin[i],
                                                              FFCompParameterMax[i],
                                                              FFCompParameterIncrement[i]);
            rangeToUse.setSkewForCentre (FFCompParamCenter[i]);
            std::function<juce::String (float value, int maximumStringLength)> stringFromValue = nullptr;

            if ((paramType == VParameter::attack) || (paramType == VParameter::release) || (paramType == VParameter::inputGain) || (paramType == VParameter::makeupGain))
            {
                stringFromValue =
                    [] (float value, int) {
                        const auto absValue = std::abs (value);

                        if (absValue < 10.0f)
                        {
                            return juce::String (value, 2);
                        }
                        if (absValue < 100.0f)
                        {
                            return juce::String (value, 1);
                        }
                        return juce::String (static_cast<int> (value));
                    };
            }
            else
            {
                stringFromValue =
                    [] (int value, int) {
                        return juce::String (value);
                    };
            }
            params.push_back (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { FFCompParameterID()[i], ValentineParameterVersion },
                                                                           FFCompParameterLabel()[i],
                                                                           rangeToUse,
                                                                           FFCompParameterDefaults[i],
                                                                           VParameterUnit()[i],
                                                                           juce::AudioProcessorParameter::genericParameter,
                                                                           stringFromValue,
                                                                           nullptr));
        }
    }

    return { params.begin(), params.end() };
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
    const auto oversampleMultiplier = static_cast<int> (pow (2, oversampleFactor));

    processBuffer.setSize (2, samplesPerBlock);
    processBuffer.clear();

    ffCompressor->setAttack (*treeState.getRawParameterValue (FFCompParameterID()[getParameterIndex (VParameter::attack)]));
    ffCompressor->setRelease (*treeState.getRawParameterValue (FFCompParameterID()[getParameterIndex (VParameter::release)]));
    ffCompressor->setSampleRate (sampleRate * oversampleMultiplier);
    ffCompressor->reset (samplesPerBlock * oversampleMultiplier);
    ffCompressor->setOversampleMultiplier (oversampleMultiplier);

    oversampler->reset();
    oversampler->initProcessing (static_cast<size_t> (samplesPerBlock));

    saturator->reset (sampleRate);
    boundedSaturator->reset (sampleRate);
    simpleZOH->setParams (static_cast<float> (sampleRate / downSampleRate));

    // Calculate delay, round up. That's the delay reported to host. subtract
    // the original delay from that and you have the fractional delay
    // for processed data.
    // .5 for the the interpolated tanh() latency,
    // .5 for the interp inverse sine latency
    const auto overSamplingDelay = oversampler->getLatencyInSamples() + 1.0f;
    const auto reportedDelay = static_cast<int> (std::ceil (overSamplingDelay));
    const auto fracDelay = reportedDelay - overSamplingDelay;
    setLatencySamples (reportedDelay);
    circBuffDelay = reportedDelay;

    for (auto& filter : fracDelayFilters)
    {
        filter->reset();
        filter->prepare (fracDelay);
    }

    for (auto& buffer : unProcBuffers)
    {
        buffer->setSize (samplesPerBlock);
        buffer->reset();
    }

    dryWet.reset (sampleRate, dryWetRampLength);

    const auto rmsWindow = juce::roundToInt (RMStime * 0.001f * sampleRate / samplesPerBlock);
    inputMeterSource.resize (getTotalNumInputChannels(), rmsWindow);

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

void ValentineAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    auto bufferSize = buffer.getNumSamples();
    auto currentSamplesPerBlock = bufferSize;

    // We need to prepare the process buffer first. The input buffer is delayed to account for
    // oversampling latency. Copying into the process buffer after this, then, would undo the
    // purpose of the delay: maintaining phase coherence between processed and unprocessed
    // signals.
    processBuffer.setSize (totalNumOutputChannels, currentSamplesPerBlock, true, true, true);
    for (auto channel = 0; channel < totalNumInputChannels; ++channel)
        processBuffer.copyFrom (channel, 0, buffer.getReadPointer (channel), buffer.getNumSamples());

    prepareInputBuffer (buffer,
                        totalNumInputChannels,
                        totalNumOutputChannels,
                        currentSamplesPerBlock);

    if (bypassOn.get())
    {
        return;
    }

    inputMeterSource.measureBlock (buffer);

    // "Downsample" and Bitcrush processing
    if (crushOn.get())
    {
        simpleZOH->processBlock (processBuffer, currentSamplesPerBlock, totalNumOutputChannels);

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
    juce::dsp::AudioBlock<float> highSampleRateBlock = oversampler->processSamplesUp (processBlock);

    ffCompressor->process (highSampleRateBlock);
    saturator->processBlock (highSampleRateBlock);
    boundedSaturator->processBlock (highSampleRateBlock);
    oversampler->processSamplesDown (processBlock);

    // Delay processed signal to produce a integral delay amount
    const auto numOutputChannels = static_cast<size_t> (totalNumOutputChannels);
    for (size_t channel = 0; channel < numOutputChannels; ++channel)
    {
        auto channelPointer = processBlock.getChannelPointer (channel);
        const auto blockLength = processBlock.getNumSamples();

        fracDelayFilters[channel]->process (channelPointer, blockLength);
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

void ValentineAudioProcessor::processBlockBypassed (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
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

    const auto numChannels = static_cast<size_t> (numOutputChannels);
    const auto numSamples = static_cast<size_t> (buffer.getNumSamples());
    for (size_t channel = 0; channel < numChannels; ++channel)
    {
        auto channelData = buffer.getWritePointer (static_cast<int> (channel));
        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            unProcBuffers[channel]->writeBuffer (channelData[sample]);
            channelData[sample] = unProcBuffers[channel]->readBuffer (circBuffDelay, false);
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

void ValentineAudioProcessor::parameterChanged (const juce::String& parameter, float newValue)
{
    if (parameter == "Crush")
    {
        const auto bitCrushIndex = static_cast<int> (VParameter::bitCrush);
        bitCrush->setParams (juce::jmap (newValue,
                                         FFCompParameterMin[bitCrushIndex],
                                         FFCompParameterMax[bitCrushIndex],
                                         17.0f,
                                         3.0f));
        if (newValue >= 1.0001f)
            crushOn.set (true);
        else
            crushOn.set (false);
    }
    else if (parameter == "Saturation")
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
        ratioIndex = static_cast<size_t> (newValue);
        ffCompressor->setRatio (ratioValues[ratioIndex]);
        ffCompressor->setKnee (kneeValues[ratioIndex]);

        if (niceModeOn)
        {
            ffCompressor->setThreshold (thresholdValues[ratioIndex] + kNiceOffset);
        }
        else
        {
            ffCompressor->setThreshold (thresholdValues[ratioIndex]);
        }
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
    else if (parameter == "Nice")
    {
        if (newValue > 0.5)
        {
            niceModeOn = true;
            ffCompressor->setThreshold (thresholdValues[ratioIndex] + kNiceOffset);
        }
        else
        {
            niceModeOn = false;
            ffCompressor->setThreshold (thresholdValues[ratioIndex]);
        }
    }
    else if (parameter == "Bypass")
    {
        bypassOn.set (newValue > 0.5f);
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
                presetManager.setLastChosenPresetName (newTree.getPropertyAsValue ("PresetName", nullptr).toString());
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

    saturator = std::make_unique<Saturation> (Saturation::Type::inverseHyperbolicSineInterp, .6f);

    boundedSaturator = std::make_unique<Saturation> (Saturation::Type::interpolatedHyperbolicTangent, -.4f);

    oversampler = std::make_unique<Oversampling> (2,
                                                  oversampleFactor,
                                                  Oversampling::filterHalfBandPolyphaseIIR);
    simpleZOH = std::make_unique<SimpleZOH>();
    bitCrush = std::make_unique<Bitcrusher>();

    for (auto& filter : fracDelayFilters)
        filter.reset (new FirstOrderThiranAllpass<float>);

    for (auto& buffer : unProcBuffers)
    {
        buffer.reset (new CircularBuffer<float>);
    }
}

//==============================================================================

// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ValentineAudioProcessor();
}

//==============================================================================
