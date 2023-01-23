/*
  ==============================================================================

    CenterPanel.h
    Created: 24 Jul 2022 7:14:26pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Common/GUI/Components/Widgets/LabelSlider.h"
#include "Common/GUI/LookAndFeel/LookAndFeelConstants.h"
#include "Common/GUI/Components/Widgets/FlatTextChooser.h"


//==============================================================================
/*
*/

class ValentineAudioProcessor;

class CenterPanel  : public juce::Component
{
public:
    CenterPanel(ValentineAudioProcessor& processor);
    ~CenterPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    Rectangle<int> topLeftRowBorderBounds;
    LabelSlider inputSlider;
    LabelSlider crushSlider;
    LabelSlider saturateSlider;

    Rectangle<int> bottomLeftRowBorderBounds;
    LabelSlider ratioSlider;
    LabelSlider attackSlider;
    LabelSlider releaseSlider;

    Rectangle<int> topRightRowBorderBounds;
    LabelSlider mixSlider;
    LabelSlider outputSlider;

    juce::Label versionLabel;
    std::unique_ptr<juce::Drawable> vLogo;

    std::unique_ptr<tote_bag::FlatTextChooser> mRatioBox;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterPanel)
};
