/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "GUI/Panels/ValentineMainPanel.h"


//==============================================================================

class ValentineAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    ValentineAudioProcessorEditor (ValentineAudioProcessor&);
    ~ValentineAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ValentineAudioProcessor& processor;

    VMainPanel mainPanel {processor};

    // in figma ~ 5053 x 1741. ratio = 2.9 ish width to height
    static constexpr float ratio = 2.5f;
    static constexpr int minimumWidth = 700;
    static constexpr int startingWidth = 1000;
    static constexpr int maximumWidth = 2000;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ValentineAudioProcessorEditor)
};
