/*
  ==============================================================================

    FlatButton.cpp
    Created: 6 Nov 2022 1:47:36pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "FlatTextButton.h"
#include "Common/GUI/LookAndFeel/LookAndFeel.h"

namespace tote_bag
{

FlatTextButton::FlatTextButton (juce::String name)
    : TextButton (name)
{
}

void FlatTextButton::paintButton (juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    if (auto* lnf = dynamic_cast<LookAndFeel*> (&getLookAndFeel()))
    {
        lnf->drawFlatButtonBackground (g, *this, findColour (getToggleState() ? buttonOnColourId : buttonColourId), shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        lnf->drawButtonText (g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    }
    else
    {
        // This button can only be used with our custom look and feel!
        // Make sure it is being set by the top level GUI component.
        jassertfalse;
    }
}
} // namespace totebag
