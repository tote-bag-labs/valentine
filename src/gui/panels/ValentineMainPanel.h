/*
  ==============================================================================

    VMainPanel.h
    Created: 3 Apr 2022 8:53:03pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "ValentineCenterPanel.h"

#include "tote_bag/juce_gui/components/panels/InfoPanel.h"
#include "tote_bag/juce_gui/components/panels/PresetPanel.h"
#include "tote_bag/juce_gui/components/panels/VerticalMeterPanel.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeel.h"

#include <juce_gui_basics/juce_gui_basics.h>

class ValentineAudioProcessor;

class VMainPanel : public juce::Component
{
public:
    VMainPanel (ValentineAudioProcessor& inProcessor);

    ~VMainPanel() override;

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    tote_bag::LookAndFeel lookAndFeel;

    PresetPanel presetPanel;
    VerticalMeterPanel inputMeterPanel;
    VerticalMeterPanel outputMeterPanel;
    tote_bag::valentine::CenterPanel centerPanel;
    tote_bag::InfoPanel infoPanel;
};
