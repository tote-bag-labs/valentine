/*
  ==============================================================================

    VMainPanel.h
    Created: 3 Apr 2022 8:53:03pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "ValentineCenterPanel.h"

#include "Common/GUI/Components/Panels/PresetPanel.h"
#include "Common/GUI/Components/Panels/VerticalMeterPanel.h"
#include "Common/GUI/LookAndFeel/LookAndFeel.h"

#include <juce_gui_basics/juce_gui_basics.h>

class ValentineAudioProcessor;

class VMainPanel : public juce::Component
{
public:
    VMainPanel (ValentineAudioProcessor& inProcessor);
    ~VMainPanel();

    void paint (juce::Graphics& g) override;

    void resized() override;

private:
    tote_bag::LookAndFeel lookAndFeel;

    PresetPanel presetPanel;
    VerticalMeterPanel inputMeterPanel;
    VerticalMeterPanel outputMeterPanel;
    CenterPanel centerPanel;
};
