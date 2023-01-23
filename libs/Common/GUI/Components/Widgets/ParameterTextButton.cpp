/*
  ==============================================================================

    ParameterTextButton.cpp
    Created: 23 Jan 2020 9:34:06pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "ParameterTextButton.h"

ParameterTextButton::ParameterTextButton (const juce::String& buttonText,
                                          const juce::String& parameterId,
                                          AudioProcessorValueTreeState& stateToControl) :
buttonValue(stateToControl, parameterId, *this)
{
    setButtonText(buttonText);
    changeWidthToFitText();
    setClickingTogglesState (true);
}
