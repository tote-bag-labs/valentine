/*
  ==============================================================================

    CenterPanel.h
    Created: 24 Jul 2022 7:14:26pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/components/widgets/DrawableParameterButton.h"
#include "tote_bag/juce_gui/components/widgets/LabelSlider.h"
#include "tote_bag/juce_gui/components/widgets/ParameterTextButton.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/*
*/

class ValentineAudioProcessor;

class CenterPanel : public juce::Component
{
public:
    CenterPanel (ValentineAudioProcessor& processor);
    ~CenterPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::Rectangle<int> topLeftRowBorderBounds;
    LabelSlider inputSlider;
    LabelSlider crushSlider;
    LabelSlider saturateSlider;

    juce::Rectangle<int> bottomLeftRowBorderBounds;
    LabelSlider ratioSlider;
    LabelSlider attackSlider;
    LabelSlider releaseSlider;

    juce::Rectangle<int> topRightRowBorderBounds;
    LabelSlider mixSlider;
    LabelSlider outputSlider;

    tote_bag::DrawableParameterButton outputClipButton;

    tote_bag::DrawableParameterButton crushEnableButton;

    juce::Label versionLabel;
    std::unique_ptr<juce::Drawable> vLogo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterPanel)
};
