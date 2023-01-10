#pragma once

#include "PluginProcessor.h"

//==============================================================================
class ValentineAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    explicit ValentineAudioProcessorEditor (ValentineAudioProcessor&);
    ~ValentineAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ValentineAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValentineAudioProcessorEditor)
};
