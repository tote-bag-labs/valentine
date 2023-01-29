/*
  ==============================================================================

    FlatButton.h
    Created: 6 Nov 2022 1:47:36pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace tote_bag
{

class FlatTextButton : public juce::TextButton
{
public:
    FlatTextButton (juce::String name);

protected:
    void paintButton (juce::Graphics& g,
                      bool shouldDrawButtonAsHighlighted,
                      bool shouldDrawButtonAsDown) override;
};
} // namespace totebag
