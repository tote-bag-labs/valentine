/*
  ==============================================================================

    LabelSlider.cpp
    Created: 18 Apr 2022 9:12:40am
    Author:  Jose Diaz

  ==============================================================================
*/

#include "LabelSlider.h"

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================

LabelSlider::LabelSlider (const juce::String& labelText,
                          const juce::String& parameterId,
                          juce::AudioProcessorValueTreeState& stateToControl)
    : slider (parameterId, stateToControl)
{
    label.setText (labelText, juce::dontSendNotification);
    label.setColour (juce::Label::textColourId, juce::Colours::black);
    label.setJustificationType (juce::Justification::centredTop);

    addAndMakeVisible (label);
    addAndMakeVisible (slider);
}

LabelSlider::~LabelSlider()
{
}

void LabelSlider::resized()
{
    const auto mainArea = getLocalBounds();
    const auto margin = juce::roundToInt (mainArea.getHeight() * .01f);
    auto sliderArea = mainArea.reduced (margin);

    const auto labelHeight = juce::roundToInt (sliderArea.getHeight() * .140);
    const auto labelArea = sliderArea.removeFromTop (labelHeight);

    slider.setBounds (sliderArea);
    label.setBounds (labelArea);
}
