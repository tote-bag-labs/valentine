/*
  ==============================================================================

    CenterPanel.cpp
    Created: 24 Jul 2022 7:14:26pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "ValentineCenterPanel.h"
#include "BinaryData.h"
#include "PluginProcessor.h"
#include "ValentineParameters.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeel.h"
#include "tote_bag/juce_gui/utilities/GraphicsUtilities.h"

#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================

namespace detail
{
// Get ratio of svg dimensions so we can correctly resize it.
inline constexpr auto kCrushOnHeight = 108.1f;
inline constexpr auto kCrushOnWidth = 201.84f;
inline constexpr auto kCrushOnRatio = kCrushOnHeight / kCrushOnWidth;
} // namespace detail

CenterPanel::CenterPanel (ValentineAudioProcessor& processor)
    : inputSlider (FFCompParameterID()[getParameterIndex (VParameter::inputGain)],
                   processor.treeState)
    , crushSlider (FFCompParameterID()[getParameterIndex (VParameter::bitCrush)],
                   processor.treeState)
    , saturateSlider (FFCompParameterID()[getParameterIndex (VParameter::saturation)],
                      processor.treeState)
    , ratioSlider (FFCompParameterID()[getParameterIndex (VParameter::ratio)],
                   processor.treeState)
    , attackSlider (FFCompParameterID()[getParameterIndex (VParameter::attack)],
                    processor.treeState)
    , releaseSlider (FFCompParameterID()[getParameterIndex (VParameter::release)],
                     processor.treeState)
    , mixSlider (FFCompParameterID()[getParameterIndex (VParameter::dryWet)],
                 processor.treeState)
    , outputSlider (FFCompParameterID()[getParameterIndex (VParameter::makeupGain)],
                    processor.treeState)
    , outputClipButton (
          FFCompParameterLabel()[static_cast<size_t> (VParameter::outputClipEnable)],
          FFCompParameterID()[static_cast<size_t> (VParameter::outputClipEnable)],
          processor.treeState)
    // clang-format off
    , crushEnableButton (
          "On",
          juce::Drawable::createFromImageData (BinaryData::crush_on_svg,
                                               BinaryData::crush_on_svgSize).get(),
          juce::Drawable::createFromImageData (BinaryData::crush_off_svg,
                                               BinaryData ::crush_off_svgSize).get(),
          FFCompParameterID()[static_cast<size_t> (VParameter::crushEnable)],
          processor.treeState)
// clang-format on
{
    // Top left sliders
    addAndMakeVisible (inputSlider);
    addAndMakeVisible (crushSlider);
    addAndMakeVisible (crushEnableButton);
    addAndMakeVisible (saturateSlider);

    // Bottom left sliders
    addAndMakeVisible (ratioSlider);
    addAndMakeVisible (attackSlider);
    addAndMakeVisible (releaseSlider);

    // Top right sliders
    addAndMakeVisible (mixSlider);
    addAndMakeVisible (outputClipButton);
    addAndMakeVisible (outputSlider);

    // Logo
    vLogo = juce::Drawable::createFromImageData (BinaryData::logo_218x40_svg,
                                                 BinaryData::logo_218x40_svgSize);
    addAndMakeVisible (vLogo.get());

    // Version label
    versionLabel.setText (JucePlugin_VersionString, juce::dontSendNotification);
    versionLabel.setColour (juce::Label::ColourIds::textColourId,
                            tote_bag::laf_constants::vPinkDark);
    versionLabel.setJustificationType (juce::Justification::centredTop);
    addAndMakeVisible (versionLabel);
}

CenterPanel::~CenterPanel()
{
}

void CenterPanel::paint (juce::Graphics& g)
{
    using namespace tote_bag::laf_constants;

    gui_utils::drawRoundedRect (g, topLeftRowBorderBounds.toFloat(), vPinkDark);
    gui_utils::drawRoundedRect (g, bottomLeftRowBorderBounds.toFloat(), vPinkDark);
    gui_utils::drawRoundedRect (g, topRightRowBorderBounds.toFloat(), vPinkDark);
}

void CenterPanel::resized()
{
    auto workingArea = getLocalBounds();

    auto areaFunc = [] (juce::Rectangle<int>& area,
                        size_t numSliders,
                        size_t sliderCount) -> juce::Rectangle<int> {
        int amountToRemove =
            juce::roundToInt (static_cast<float> (area.getWidth())
                              / static_cast<float> (numSliders - sliderCount));
        return area.removeFromLeft (amountToRemove);
    };

    const auto paramWidth = juce::roundToInt (workingArea.getWidth() / 5.0f);

    // Center slider rows vertically
    auto betweenRowMargin = paramWidth * .1f;
    const auto roundedRowMargin = juce::roundToInt (betweenRowMargin);

    // 1.65 to reflect that the bottom half is .65 the height of the top row
    auto totalParamHeight = (paramWidth * 1.75f) + betweenRowMargin;
    auto verticalAlignmentSpacer =
        juce::roundToInt ((workingArea.getHeight() - totalParamHeight) * .5f);
    workingArea.removeFromTop (verticalAlignmentSpacer);
    workingArea.removeFromBottom (verticalAlignmentSpacer);

    const auto borderMargin = static_cast<int> (paramWidth * .05f);

    // Left side
    const auto numLeftColumns = 3;
    auto leftSideBounds = workingArea.removeFromLeft (paramWidth * numLeftColumns);

    // Top
    topLeftRowBorderBounds = leftSideBounds.removeFromTop (paramWidth);

    auto topLeftRowBounds = topLeftRowBorderBounds.reduced (borderMargin);

    // Crush enable button
    const auto sliderWidth = juce::roundToInt (topLeftRowBounds.getWidth() * .333f);

    // This works for now but will be confusing if svgs of different sizes or
    // differently sized buttons are desired. Not dealing with it now since
    // this will all be rewritten in next UI iteraction. aka "Good Enough".
    const auto topLeftButtonsWidth = juce::roundToInt (sliderWidth * .2f);
    const auto topLeftButtonsHeight =
        juce::roundToInt (topLeftButtonsWidth * detail::kCrushOnRatio);

    // Buttons corresponding to top left sliders will be placed within these bounds
    auto topLeftButtonRowBounds =
        topLeftRowBounds.removeFromBottom (topLeftButtonsHeight);

    const auto crushEnableButtonBounds =
        topLeftButtonRowBounds.removeFromLeft (sliderWidth)
            .withSizeKeepingCentre (topLeftButtonsWidth, topLeftButtonsHeight);

    crushEnableButton.setBounds (crushEnableButtonBounds);

    const auto buttonSliderMargin = juce::roundToInt (roundedRowMargin * .33f);
    topLeftRowBounds.removeFromBottom (buttonSliderMargin);

    // Now, the sliders
    std::array<LabelSlider*, numLeftColumns> topLeftRowComponents = {
        &crushSlider,
        &inputSlider,
        &saturateSlider,
    };

    for (size_t i = 0; i < numLeftColumns; ++i)
    {
        topLeftRowComponents[i]->setBounds (
            areaFunc (topLeftRowBounds, numLeftColumns, i));
    }

    // Margin
    leftSideBounds.removeFromTop (roundedRowMargin);

    // Bottom
    auto bottomRowParamHeight = juce::roundToInt (paramWidth * .75f);
    const auto smallborderMargin = juce::roundToInt (bottomRowParamHeight * .05f);
    bottomLeftRowBorderBounds = leftSideBounds.removeFromTop (bottomRowParamHeight);

    auto bottomLeftRowBounds = bottomLeftRowBorderBounds.reduced (smallborderMargin);

    std::array<Component*, numLeftColumns> bottomLeftRowComponents = {
        &ratioSlider,
        &attackSlider,
        &releaseSlider,
    };

    for (size_t i = 0; i < numLeftColumns; ++i)
    {
        bottomLeftRowComponents[i]->setBounds (
            areaFunc (bottomLeftRowBounds, numLeftColumns, i));
    }

    // Vertical margin
    workingArea.removeFromLeft (roundedRowMargin);

    const auto numRightColumns = 2;
    auto rightSideBounds = workingArea.removeFromLeft (paramWidth * numRightColumns);

    // Top right
    topRightRowBorderBounds = rightSideBounds.removeFromTop (paramWidth);

    auto topRightRowBounds = topRightRowBorderBounds.reduced (borderMargin);

    // Clip button
    const auto clipButtonWidth = juce::roundToInt (topRightRowBounds.getWidth() * .5f);
    auto clipButtonBounds =
        topRightRowBounds.removeFromBottom (juce::roundToInt (clipButtonWidth * .09f))
            .removeFromLeft (clipButtonWidth)
            .reduced (juce::roundToInt (clipButtonWidth * .35f), 0);
    outputClipButton.setBounds (clipButtonBounds);

    topRightRowBounds.removeFromBottom (buttonSliderMargin);

    std::array<LabelSlider*, numRightColumns> topRightRowComponents = {&outputSlider,
                                                                       &mixSlider};

    for (size_t i = 0; i < numRightColumns; ++i)
    {
        topRightRowComponents[i]->setBounds (
            areaFunc (topRightRowBounds, numRightColumns, i));
    }

    // Margin
    rightSideBounds.removeFromTop (roundedRowMargin);

    // Bottom right
    auto logoHeight = juce::roundToInt (paramWidth * .5f);
    auto versionHeight = juce::roundToInt (logoHeight * .25);

    auto logoBounds = rightSideBounds.removeFromTop (logoHeight)
                          .removeFromLeft (paramWidth * numRightColumns)
                          .reduced (borderMargin);
    auto versionBounds = logoBounds.removeFromBottom (versionHeight);

    vLogo->setTransformToFit (
        logoBounds.toFloat(),
        juce::RectanglePlacement (juce::RectanglePlacement::xMid
                                  | juce::RectanglePlacement::yTop));
    versionLabel.setBounds (versionBounds);
}
