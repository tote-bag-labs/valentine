/*
  ==============================================================================

    VerticalMeterPanel.cpp
    Created: 27 Apr 2022 9:47:04pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "VerticalMeterPanel.h"

#include "Common/GUI/LookAndFeel/LookAndFeelConstants.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
VerticalMeterPanel::VerticalMeterPanel(const juce::String& label,
                                       ReductionMeterPlacement grMeterPlacement,
                                       foleys::LevelMeterSource* levelMeterSource,
                                       foleys::LevelMeterSource* grMeterSource) :
grMeterPlacement(grMeterPlacement)
{
    using namespace tote_bag::laf_constants;

    metersLookAndFeel.setColour(FFAU::LevelMeter::lmBackgroundColour, vPink);
    metersLookAndFeel.setColour(FFAU::LevelMeter::lmMeterGradientLowColour, (vGreen1));
    metersLookAndFeel.setColour(FFAU::LevelMeter::lmMeterGradientMidColour, vGreen2);
    metersLookAndFeel.setColour(FFAU::LevelMeter::lmMeterMaxOverColour, vRed);
    
    levelMeter.setLookAndFeel(&metersLookAndFeel);
    levelMeter.setMeterSource(levelMeterSource);
    addAndMakeVisible(levelMeter);

    if(grMeterSource)
    {
        gainReductionMeter = std::make_unique<FFAU::LevelMeter>(FFAU::LevelMeter::MeterFlags::Reduction);
        gainReductionMeter->setMeterSource(grMeterSource);
        gainReductionMeter->setLookAndFeel(&metersLookAndFeel);
        addAndMakeVisible(gainReductionMeter.get());
    }
}

VerticalMeterPanel::~VerticalMeterPanel()
{
}

void VerticalMeterPanel::resized()
{
    auto area = getLocalBounds();

    // remove amount for gain reduction no matter what, that way meter will be
    // the same size whether or not a gani reduction bar is specified
    const auto areaWidth = area.getWidth();

    // get gr and meters width
    const auto grMeterWidth = juce::roundToInt(areaWidth * .25f);


    const auto grMeterBounds = (grMeterPlacement & ReductionMeterPlacement::Left) ?
                               area.removeFromLeft(grMeterWidth) : area.removeFromRight(grMeterWidth);

    if(gainReductionMeter.get())
    {
        gainReductionMeter->setBounds(grMeterBounds);
    }
    levelMeter.setBounds(area);

}
