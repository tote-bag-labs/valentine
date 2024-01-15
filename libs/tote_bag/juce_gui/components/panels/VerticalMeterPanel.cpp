/*
  ==============================================================================

    VerticalMeterPanel.cpp
    Created: 27 Apr 2022 9:47:04pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "VerticalMeterPanel.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
VerticalMeterPanel::VerticalMeterPanel (ReductionMeterPlacement reductionMeterPlacement,
                                        foleys::LevelMeterSource* levelMeterSource,
                                        foleys::LevelMeterSource* grMeterSource)
    : grMeterPlacement (reductionMeterPlacement)
{
    using namespace tote_bag::laf_constants;

    levelMeter.setMeterSource (levelMeterSource);
    addAndMakeVisible (levelMeter);

    if (grMeterSource)
    {
        gainReductionMeter =
            std::make_unique<FFAU::LevelMeter> (FFAU::LevelMeter::MeterFlags::Reduction);
        gainReductionMeter->setMeterSource (grMeterSource);
        addAndMakeVisible (gainReductionMeter.get());
    }
}

VerticalMeterPanel::~VerticalMeterPanel()
{
}

void VerticalMeterPanel::lookAndFeelChanged()
{
    using namespace tote_bag::laf_constants;

    auto& lookAndFeel = getLookAndFeel();

    lookAndFeel.setColour (FFAU::LevelMeter::lmBackgroundColour, valentinePink);
    lookAndFeel.setColour (FFAU::LevelMeter::lmMeterGradientLowColour, (grassGreen));
    lookAndFeel.setColour (FFAU::LevelMeter::lmMeterGradientMidColour, avocadoGreen);
    lookAndFeel.setColour (FFAU::LevelMeter::lmMeterMaxOverColour, racecarRed);

    levelMeter.setLookAndFeel (&lookAndFeel);

    if (gainReductionMeter.get())
    {
        gainReductionMeter->setLookAndFeel (&lookAndFeel);
    }
}

void VerticalMeterPanel::resized()
{
    auto area = getLocalBounds();

    // remove amount for gain reduction no matter what, that way meter will be
    // the same size whether or not a gani reduction bar is specified
    const auto areaWidth = area.getWidth();

    // get gr and meters width
    const auto grMeterWidth = juce::roundToInt (areaWidth * .25f);

    const auto grMeterBounds = (grMeterPlacement & ReductionMeterPlacement::Left)
                                   ? area.removeFromLeft (grMeterWidth)
                                   : area.removeFromRight (grMeterWidth);

    if (gainReductionMeter.get())
    {
        gainReductionMeter->setBounds (grMeterBounds);
    }
    levelMeter.setBounds (area);
}
