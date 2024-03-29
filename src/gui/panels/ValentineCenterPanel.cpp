// 2023 Tote Bag Labs

#include "ValentineCenterPanel.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

#include "ValentineParameters.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"
#include "tote_bag/juce_gui/utilities/GraphicsUtilities.h"

namespace tote_bag
{
namespace valentine
{

CenterPanel::CenterPanel (ValentineAudioProcessor& processor)
    : borderLineThickness (0.0f)
    , borderCornerSize (0.0f)
    , topRow (processor)
    , bottomRow (processor)
{
    addAndMakeVisible (topRow);
    addAndMakeVisible (bottomRow);
}

CenterPanel::~CenterPanel()
{
}

void CenterPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::black);

    g.drawRoundedRectangle (topRowBorder.toFloat(),
                            borderCornerSize,
                            borderLineThickness);

    g.drawRoundedRectangle (bottomRowBorder.toFloat(),
                            borderCornerSize,
                            borderLineThickness);
}

void CenterPanel::resized()
{
    auto localBounds = getLocalBounds();
    const auto margin = localBounds.getHeight() * .0375f;
    localBounds.reduce (juce::roundToInt (margin * 1.6f),
                        juce::roundToInt (margin * 1.3f));

    topRowBorder =
        localBounds.removeFromTop (juce::roundToInt (localBounds.getHeight() * .50f));

    borderLineThickness = topRowBorder.getHeight() * .01f;
    borderCornerSize = topRowBorder.getHeight() * .060f;

    topRow.setBounds (topRowBorder.reduced (juce::roundToInt (margin)));

    localBounds.removeFromTop (juce::roundToInt (margin * 2));

    bottomRowBorder = localBounds;

    bottomRow.setBounds (bottomRowBorder.reduced (juce::roundToInt (margin)));
}
} // namespace tote_bag
} // namespace valentine
