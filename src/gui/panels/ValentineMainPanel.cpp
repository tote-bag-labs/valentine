/*
  ==============================================================================

    VMainPanel.cpp
    Created: 3 Apr 2022 8:53:03pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "ValentineMainPanel.h"
#include "PluginProcessor.h"
#include "ValentineParameters.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

VMainPanel::VMainPanel (ValentineAudioProcessor& processor)
    : presetPanel (processor.getPresetManager(),
                   FFCompParameterLabel()[getParameterIndex (VParameter::bypass)],
                   FFCompParameterID()[getParameterIndex (VParameter::bypass)],
                   processor.treeState)
    , inputMeterPanel (ReductionMeterPlacement::Right, &processor.getInputMeterSource())
    , outputMeterPanel (ReductionMeterPlacement::Left,
                        &processor.getOutputMeterSource(),
                        &processor.getGrMeterSource())
    , centerPanel (processor)
{
    addAndMakeVisible (presetPanel);
    addAndMakeVisible (centerPanel);
    addAndMakeVisible (inputMeterPanel);
    addAndMakeVisible (outputMeterPanel);

    setLookAndFeel (&lookAndFeel);
}

VMainPanel::~VMainPanel()
{
    setLookAndFeel (nullptr);
}

void VMainPanel::paint (juce::Graphics& g)
{
    g.fillAll (tote_bag::colours::valentinePink);
}

void VMainPanel::resized()
{
    auto panelBounds = getLocalBounds();

    const auto presetBounds =
        panelBounds.removeFromTop (juce::roundToInt (panelBounds.getHeight() * .075f));
    presetPanel.setBounds (presetBounds);

    const auto resizerMargin = juce::roundToInt (panelBounds.getHeight() * .03f);
    panelBounds.removeFromLeft (resizerMargin);
    panelBounds.removeFromRight (resizerMargin);
    panelBounds.removeFromBottom (resizerMargin);
    panelBounds.removeFromTop (juce::roundToInt (resizerMargin * .5));

    const auto meterWidth = juce::roundToInt (panelBounds.getWidth() * .05f);
    const auto outMeterBounds = panelBounds.removeFromRight (meterWidth);
    const auto inMeterBounds = panelBounds.removeFromLeft (meterWidth);

    // Shift working area left to account for wider output meter
    panelBounds.removeFromRight (juce::roundToInt (meterWidth / 4.f));

    inputMeterPanel.setBounds (inMeterBounds);
    outputMeterPanel.setBounds (outMeterBounds);
    centerPanel.setBounds (panelBounds);
}
