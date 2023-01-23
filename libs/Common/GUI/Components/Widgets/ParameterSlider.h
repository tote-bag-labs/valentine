/*
  ==============================================================================

    ParameterSlider.h
    Created: 4 Aug 2019 4:36:23pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class ParameterSlider : public Slider
{
public:

    ParameterSlider (const juce::String& parameterId,
                     juce::AudioProcessorValueTreeState& stateToControl);

private:

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterSlider);
};

