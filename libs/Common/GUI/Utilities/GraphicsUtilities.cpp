/*
  ==============================================================================

    GraphicsUtilities.cpp
    Created: 23 Jul 2022 2:29:49pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "GraphicsUtilities.h"

namespace gui_utils
{

void drawRoundedRect (juce::Graphics& g,
                      juce::Rectangle<float> bounds,
                      juce::Colour colour)
{
    auto margin = bounds.getHeight() * .025f;
    auto croppedBounds = bounds.reduced (margin);

    auto h = croppedBounds.getHeight();
    auto lineThickness = h * .025f;
    auto cornerSize = h * .25;

    g.setColour (colour.darker());

    g.drawRoundedRectangle (croppedBounds, cornerSize, lineThickness);

    g.setColour (colour);

    juce::Path p;
    p.addRoundedRectangle (croppedBounds, cornerSize);
    g.fillPath (p);
}

} // namespace gui_utils
