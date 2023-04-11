/*
  ==============================================================================

    FlatChooser.cpp
    Created: 7 Nov 2022 8:43:30pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "FlatTextChooser.h"

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================

namespace tote_bag
{

FlatTextChooser::FlatTextChooser (const juce::String& labelText,
                                  const std::vector<std::string>& choices,
                                  int paramGroupId,
                                  juce::AudioParameterChoice* param = nullptr,
                                  bool alignWithParameterSliders = false)
    : mLabel (labelText + " Chooser", labelText)
    , mAlignWithParameterSliders (alignWithParameterSliders)
{
    mLabel.setColour (juce::Label::textColourId, juce::Colours::black);
    mLabel.setJustificationType (juce::Justification::centredTop);

    addAndMakeVisible (mLabel);

    for (const auto& choiceLabelText : choices)
    {
        auto button = mButtons.add (std::make_unique<FlatTextButton> (choiceLabelText));

        int edgesFlag = 0;
        if (choiceLabelText == choices.front())
        {
            edgesFlag = juce::Button::ConnectedEdgeFlags::ConnectedOnBottom;
        }
        else if (choiceLabelText == choices.back())
        {
            edgesFlag = juce::Button::ConnectedEdgeFlags::ConnectedOnTop;
        }
        else
        {
            edgesFlag = juce::Button::ConnectedEdgeFlags::ConnectedOnTop
                        | juce::Button::ConnectedEdgeFlags::ConnectedOnBottom;
        }

        button->setConnectedEdges (edgesFlag);
        addAndMakeVisible (button);
    }

    if (param)
    {
        mButtonState = std::make_unique<RadioButtonGroupManager> (*param, paramGroupId);

        for (const auto button : mButtons)
        {
            mButtonState->attach (button);
        }
    }
}

FlatTextChooser::~FlatTextChooser()
{
}

void FlatTextChooser::resized()
{
    const auto mainArea = getLocalBounds();
    const auto margin = juce::roundToInt (mainArea.getHeight() * .01f);
    auto buttonArea = getLocalBounds().reduced (margin);

    const auto labelHeight = juce::roundToInt (buttonArea.getHeight() * .140);
    const auto labelArea = buttonArea.removeFromTop (labelHeight);

    mLabel.setBounds (labelArea);

    // Make some more space between label and buttons
    buttonArea.removeFromTop (labelHeight);

    if (mAlignWithParameterSliders)
    {
        buttonArea.removeFromBottom (static_cast<int>(buttonArea.getHeight() * .15f));
    }

    // The buttons themselves shouldn't take up all of the horizontal space
    // given to this component
    const auto sideMargin = juce::roundToInt (buttonArea.getHeight() * .6f);
    buttonArea.reduce (sideMargin, 0);

    const auto numButtons = mButtons.size();
    const auto buttonWidth = buttonArea.getWidth();
    const auto buttonHeight = juce::roundToInt (buttonArea.getHeight() / static_cast<float> (numButtons));
    const auto x = buttonArea.getX();
    auto y = buttonArea.getY();

    for (auto* button : mButtons)
    {
        button->setBounds (x, y, buttonWidth, buttonHeight);

        y += buttonHeight;
    }
}

} // namespace tote_bag
