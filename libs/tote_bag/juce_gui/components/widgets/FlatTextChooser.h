/*
  ==============================================================================

    FlatChooser.h
    Created: 7 Nov 2022 8:43:30pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/components/widgets/FlatTextButton.h"
#include "tote_bag/juce_gui/managers/RadioButtonGroupManager.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================

namespace juce
{
class AudioParameterChoice;
} // namespace juce

namespace tote_bag
{
/*
*/
class FlatTextChooser : public juce::Component
{
public:
    FlatTextChooser (const juce::String&,
                     const std::vector<std::string>&,
                     int,
                     juce::AudioParameterChoice*,
                     bool);
    ~FlatTextChooser() override;

    void resized() override;

private:
    juce::Label mLabel;

    juce::OwnedArray<FlatTextButton> mButtons;

    // Depends on mButtons. Must be destroyed first.
    std::unique_ptr<RadioButtonGroupManager> mButtonState;

    // TODO: remove this awful kludge
    // If true, the bottom of the chooser will be adjusted
    // to line up with the top of values displayed by parameter
    // sliders. This looks better when a chooser box and
    // parameter sliders are in the same row.
    // This is not really ideal, and is here because the label
    // is part of this component as well as other parameter
    // widgets. Lesson: labels need to be separate (TODO).
    bool mAlignWithParameterSliders;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FlatTextChooser)
};
}
