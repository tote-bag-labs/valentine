/*
  ==============================================================================

    ParameterTextButton.h
    Created: 23 Jan 2020 9:34:06pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ParameterTextButton : public juce::TextButton
{
public:
    ParameterTextButton (const juce::String& buttonText,
                         const juce::String& parameterId,
                         juce::AudioProcessorValueTreeState& stateToControl);

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment buttonValue;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParameterTextButton)
};
