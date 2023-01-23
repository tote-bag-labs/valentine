/*
  ==============================================================================

    LabelSlider.h
    Created: 18 Apr 2022 9:12:40am
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Common/GUI/Components/Widgets/ParameterSlider.h"

//==============================================================================
/*
*/
class LabelSlider  : public juce::Component
{
public:

    LabelSlider(const juce::String& parameterId,
                juce::AudioProcessorValueTreeState& stateToControl);

    ~LabelSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label label;
    ParameterSlider slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelSlider)
};
