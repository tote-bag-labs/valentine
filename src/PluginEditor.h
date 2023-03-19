/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

#include "gui/panels/ValentineMainPanel.h"

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================

class ValentineAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ValentineAudioProcessorEditor (ValentineAudioProcessor&);
    ~ValentineAudioProcessorEditor() override;

    //==============================================================================
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ValentineAudioProcessor& audioProcessor;

    VMainPanel mainPanel;

    // in figma ~ 5053 x 1741. ratio = 2.9 ish width to height
    static constexpr float ratio = 2.5f;
    static constexpr int minimumWidth = 700;
    static constexpr int startingWidth = 1000;
    static constexpr int maximumWidth = 2000;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValentineAudioProcessorEditor)
};
