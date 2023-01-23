/*
  ==============================================================================

    CenterPanel.cpp
    Created: 24 Jul 2022 7:14:26pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include <JuceHeader.h>

#include "ValentineCenterPanel.h"
#include "PluginProcessor.h"
#include "ValentineParameters.h"

#include "Common/GUI/Utilities/GraphicsUtilities.h"

//==============================================================================

CenterPanel::CenterPanel(ValentineAudioProcessor& processor) :
 inputSlider(FFCompParameterID()[VParameter::inputGain], processor.treeState)
,crushSlider(FFCompParameterID()[VParameter::bitCrush], processor.treeState)
,saturateSlider(FFCompParameterID()[VParameter::saturation], processor.treeState)
,ratioSlider(FFCompParameterID()[VParameter::ratio], processor.treeState)
,attackSlider(FFCompParameterID()[VParameter::attack], processor.treeState)
,releaseSlider(FFCompParameterID()[VParameter::release], processor.treeState)
,mixSlider(FFCompParameterID()[VParameter::dryWet], processor.treeState)
,outputSlider(FFCompParameterID()[VParameter::makeupGain], processor.treeState)
{
    // Top left sliders
    addAndMakeVisible(inputSlider);
    addAndMakeVisible(crushSlider);
    addAndMakeVisible(saturateSlider);

    // Bottom left sliders
    addAndMakeVisible(ratioSlider);
    addAndMakeVisible(attackSlider);
    addAndMakeVisible(releaseSlider);

    // Top right sliders
    addAndMakeVisible(mixSlider);
    addAndMakeVisible(outputSlider);

    // Logo
    vLogo = Drawable::createFromImageData(BinaryData::logo_218x40_svg,
                                          BinaryData::logo_218x40_svgSize); 
    addAndMakeVisible(vLogo.get());

    // Version label
    versionLabel.setText(JucePlugin_VersionString, dontSendNotification);
    versionLabel.setColour(juce::Label::ColourIds::textColourId,
                           tote_bag::laf_constants::vPinkDark);
    versionLabel.setJustificationType(Justification::centredTop);
    addAndMakeVisible(versionLabel);

    // Ratio box
    if (auto ratioParam = dynamic_cast<AudioParameterChoice*>(processor.treeState.getParameter(FFCompParameterID()[VParameter::ratio])))
    {
        mRatioBox = std::make_unique<tote_bag::FlatTextChooser>(FFCompParameterLabel()[VParameter::ratio],
                                                                std::vector<std::string>{
                                                                k4_1RatioLabel.data(),
                                                                k8_1RatioLabel.data(),
                                                                k12_1RatioLabel.data(),
                                                                k20_1RatioLabel.data(),
                                                                k1000_1RatioLabel.data()},
                                                                8001,
                                                                ratioParam,
                                                                true);
    }

    addAndMakeVisible(mRatioBox.get());
}

CenterPanel::~CenterPanel()
{
}

void CenterPanel::paint (juce::Graphics& g)
{
    using namespace tote_bag::laf_constants;

    gui_utils::drawRoundedRect(g, topLeftRowBorderBounds.toFloat(), vPinkDark);
    gui_utils::drawRoundedRect(g, bottomLeftRowBorderBounds.toFloat(), vPinkDark);
    gui_utils::drawRoundedRect(g, topRightRowBorderBounds.toFloat(), vPinkDark);
}

void CenterPanel::resized()
{
    auto workingArea = getLocalBounds();

    auto areaFunc = [](juce::Rectangle<int>& area, int numSliders, int sliderCount) -> juce::Rectangle<int> {
        return area.removeFromLeft (area.getWidth() / (numSliders - sliderCount));
    };

    const auto paramWidth = juce::roundToInt(workingArea.getWidth() / 5.0f);

    // Center slider rows vertically
    auto betweenRowMargin = paramWidth * .1f;

    // 1.65 to reflect that the bottom half is .65 the height of the top row
    auto totalParamHeight = (paramWidth * 1.65f) + betweenRowMargin;
    auto verticalAlignmentSpacer = juce::roundToInt((workingArea.getHeight() - totalParamHeight) * .5f);
    workingArea.removeFromTop(verticalAlignmentSpacer);
    workingArea.removeFromBottom(verticalAlignmentSpacer);

    const auto borderMargin = paramWidth * .05f;

    // Left side
    const auto numLeftColumns = 3;
    auto leftSideBounds = workingArea.removeFromLeft(paramWidth * numLeftColumns);

    // Top
    topLeftRowBorderBounds = leftSideBounds.removeFromTop(paramWidth);

    auto topLeftRowBounds = topLeftRowBorderBounds.reduced(borderMargin);

    std::array<LabelSlider*, numLeftColumns> topLeftRowComponents = {
        &inputSlider,
        &crushSlider,
        &saturateSlider
    };

    for (int i = 0; i < numLeftColumns; ++i)
    {
        topLeftRowComponents[i]->setBounds(areaFunc(topLeftRowBounds, numLeftColumns, i));
    }

    // Margin
    leftSideBounds.removeFromTop(betweenRowMargin);

    // Bottom
    auto bottomRowParamHeight = juce::roundToInt(paramWidth * .65f);
    const auto smallborderMargin = bottomRowParamHeight * .05f;
    bottomLeftRowBorderBounds = leftSideBounds.removeFromTop(bottomRowParamHeight);

    auto bottomLeftRowBounds = bottomLeftRowBorderBounds.reduced(smallborderMargin);

    auto ratioBounds = bottomLeftRowBounds.removeFromLeft(bottomLeftRowBounds.getWidth() / 3.0f);
    mRatioBox->setBounds(ratioBounds);

    const auto numBottomLeftColumns = numLeftColumns - 1;

    std::array<Component*, numBottomLeftColumns> bottomLeftRowComponents = {
        &attackSlider,
        &releaseSlider
    };

    for (int i = 0; i < numBottomLeftColumns; ++i)
    {
        bottomLeftRowComponents[i]->setBounds(areaFunc(bottomLeftRowBounds, numBottomLeftColumns, i));
    }

    // Vertical margin
    workingArea.removeFromLeft(betweenRowMargin);

    const auto numRightColumns = 2;
    auto rightSideBounds = workingArea.removeFromLeft(paramWidth * numRightColumns);

    // Top right
    topRightRowBorderBounds = rightSideBounds.removeFromTop(paramWidth);

    auto topRightRowBounds = topRightRowBorderBounds.reduced(borderMargin);

    std::array<LabelSlider*, numRightColumns> topRightRowComponents = {
        &outputSlider,
        &mixSlider
    };

    for (int i = 0; i < numRightColumns; ++i)
    {
        topRightRowComponents[i]->setBounds(areaFunc(topRightRowBounds, numRightColumns, i));
    }

    // Margin
    rightSideBounds.removeFromTop(betweenRowMargin);

    // Bottom right
    auto logoHeight = juce::roundToInt(paramWidth * .5f);
    auto versionHeight = juce::roundToInt(logoHeight * .25);

    auto logoBounds = rightSideBounds.removeFromTop(logoHeight).removeFromLeft(paramWidth * numRightColumns).reduced(borderMargin);
    auto versionBounds = logoBounds.removeFromBottom(versionHeight);

    vLogo->setTransformToFit(logoBounds.toFloat(), RectanglePlacement(RectanglePlacement::xMid |
                                                                    RectanglePlacement::yTop));
    versionLabel.setBounds(versionBounds);
}
