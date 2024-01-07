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
    auto sliderArea = getLocalBounds();

    const auto labelHeight = juce::roundToInt (sliderArea.getHeight() * .120);
    label.setBounds (sliderArea.removeFromTop (labelHeight));

    const auto margin = juce::roundToInt (sliderArea.getHeight() * .03f);
    sliderArea.removeFromTop (margin);

    slider.setBounds (sliderArea);
}
