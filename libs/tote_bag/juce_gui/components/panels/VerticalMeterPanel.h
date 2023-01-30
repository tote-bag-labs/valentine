/*
  ==============================================================================

    VerticalMeterPanel.h
    Created: 27 Apr 2022 9:47:04pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/lookandfeel/LookAndFeel.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================

enum ReductionMeterPlacement {
    Left = 1,
    Right = 1 << 1,
    None = 1 << 2
};

class ValentineAudioProcessor;

class VerticalMeterPanel : public juce::Component
{
public:
    VerticalMeterPanel (const juce::String& label,
                        ReductionMeterPlacement grMeterPlacement,
                        foleys::LevelMeterSource* levelMeterSource,
                        foleys::LevelMeterSource* grMeterSource = nullptr);

    ~VerticalMeterPanel() override;

    void resized() override;

private:
    tote_bag::LookAndFeel metersLookAndFeel;

    FFAU::LevelMeter levelMeter { FFAU::LevelMeter::MeterFlags::Minimal };

    std::unique_ptr<FFAU::LevelMeter> gainReductionMeter;

    ReductionMeterPlacement grMeterPlacement;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VerticalMeterPanel)
};
