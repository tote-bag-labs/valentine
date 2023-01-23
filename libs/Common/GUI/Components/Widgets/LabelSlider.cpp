/*
  ==============================================================================

    LabelSlider.cpp
    Created: 18 Apr 2022 9:12:40am
    Author:  Jose Diaz

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LabelSlider.h"

//==============================================================================

LabelSlider::LabelSlider(const juce::String& parameterId,
                         juce::AudioProcessorValueTreeState& stateToControl) :
slider(parameterId, stateToControl)
{
    label.setText(stateToControl.getParameter(parameterId)->name, dontSendNotification);
    label.setColour(Label::textColourId, Colours::black);
    label.setJustificationType(Justification::centredTop);

    addAndMakeVisible(label);
    addAndMakeVisible(slider);
}

LabelSlider::~LabelSlider()
{
}

void LabelSlider::paint (juce::Graphics& g)
{
}

void LabelSlider::resized()
{
    const auto mainArea = getLocalBounds();
    const auto margin = juce::roundToInt(mainArea.getHeight() * .01f);
    auto sliderArea = mainArea.reduced(margin);

    const auto labelHeight = juce::roundToInt(sliderArea.getHeight() * .140);
    const auto labelArea = sliderArea.removeFromTop(labelHeight);

    slider.setBounds(sliderArea);
    label.setBounds(labelArea);
}
