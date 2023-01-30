/*
  ==============================================================================

    LabelSlider.h
    Created: 18 Apr 2022 9:12:40am
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/components/widgets/ParameterSlider.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/*
*/
namespace juce
{
class AudioProcessorValueTreeState;
} // namespace juce

class LabelSlider : public juce::Component
{
public:
    LabelSlider (const juce::String& parameterId,
                 juce::AudioProcessorValueTreeState& stateToControl);

    ~LabelSlider() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Label label;
    ParameterSlider slider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LabelSlider)
};
