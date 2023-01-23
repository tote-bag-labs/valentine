/*
  ==============================================================================

    ParameterTextButton.h
    Created: 23 Jan 2020 9:34:06pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

class ParameterTextButton : public TextButton
{
public:
    ParameterTextButton (const juce::String& buttonText,
                         const juce::String& parameterId,
                         AudioProcessorValueTreeState& stateToControl);

private:
    AudioProcessorValueTreeState::ButtonAttachment buttonValue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterTextButton)
};
