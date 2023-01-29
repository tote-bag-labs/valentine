/*
  ==============================================================================

    GraphicsUtilities.h
    Created: 23 Jul 2022 2:29:49pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace gui_utils
{

void drawRoundedRect (juce::Graphics& g,
                      juce::Rectangle<float> bounds,
                      juce::Colour colour);

} // namespace gui_utils
