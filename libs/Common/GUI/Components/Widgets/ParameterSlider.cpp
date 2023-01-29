/*
  ==============================================================================

    ParameterSlider.cpp
    Created: 4 Aug 2019 4:36:23pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#include "ParameterSlider.h"

ParameterSlider::ParameterSlider (const juce::String& parameterId,
                                  juce::AudioProcessorValueTreeState& stateToControl)
    : sliderValue (std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (stateToControl,
                                                                                           parameterId,
                                                                                           *this))
{
    setSliderStyle (SliderStyle::RotaryHorizontalVerticalDrag);
    setTextValueSuffix (stateToControl.getParameter (parameterId)->label);
    setTextBoxStyle (Slider::TextEntryBoxPosition::TextBoxBelow, false, 0, 0);
}
