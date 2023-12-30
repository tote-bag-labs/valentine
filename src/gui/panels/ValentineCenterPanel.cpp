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
namespace detail
{
inline const juce::String kCrushSliderText = "CRUSH";
inline const juce::String kCompressSliderText = "COMPRESS";
inline const juce::String kSaturateSliderText = "SATURATE";
inline const juce::String kRatioSliderText = "RATIO";
inline const juce::String kAttackSliderText = "ATTACK";
inline const juce::String kReleaseSliderText = "RELEASE";
inline const juce::String kOutputSliderText = "OUTPUT";
inline const juce::String kMixSliderText = "MIX";
} // namespace detail

CenterPanel::CenterPanel (ValentineAudioProcessor& processor)
    : borderLineThickness (0.0f)
    , borderCornerSize (0.0f)
    , crushEnableButton (parameterID (VParameter::crushEnable), processor.treeState)
    , crushSlider (detail::kCrushSliderText,
                   parameterID (VParameter::bitCrush),
                   processor.treeState)
    , compressSlider (detail::kCompressSliderText,
                      parameterID (VParameter::inputGain),
                      processor.treeState)
    , saturateEnableButton (parameterID (VParameter::saturateEnable), processor.treeState)
    , saturateSlider (detail::kSaturateSliderText,
                      parameterID (VParameter::saturation),
                      processor.treeState)
    , valentineTblLogo (
          juce::Drawable::createFromImageData (BinaryData::val_totebag_logo_svg,
                                               BinaryData::val_totebag_logo_svgSize))
    , ratioSlider (detail::kRatioSliderText,
                   parameterID (VParameter::ratio),
                   processor.treeState)
    , attackSlider (detail::kAttackSliderText,
                    parameterID (VParameter::attack),
                    processor.treeState)
    , releaseSlider (detail::kReleaseSliderText,
                     parameterID (VParameter::release),
                     processor.treeState)
    , clipEnableButton (parameterID (VParameter::outputClipEnable), processor.treeState)
    , outputSlider (detail::kOutputSliderText,
                    parameterID (VParameter::makeupGain),
                    processor.treeState)
    , mixSlider (detail::kMixSliderText,
                 parameterID (VParameter::dryWet),
                 processor.treeState)
{
    addAndMakeVisible (crushEnableButton);
    addAndMakeVisible (crushSlider);
    addAndMakeVisible (compressSlider);
    addAndMakeVisible (saturateEnableButton);
    addAndMakeVisible (saturateSlider);
    addAndMakeVisible (valentineTblLogo.get());
    addAndMakeVisible (ratioSlider);
    addAndMakeVisible (attackSlider);
    addAndMakeVisible (releaseSlider);
    addAndMakeVisible (clipEnableButton);
    addAndMakeVisible (outputSlider);
    addAndMakeVisible (mixSlider);
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

    g.fillRect (bottomRowDivider);
}

void CenterPanel::resized()
{
    auto localBounds = getLocalBounds();
    const auto margin = localBounds.getHeight() * .0375f;
    localBounds.reduce (juce::roundToInt (margin * 1.6f),
                        juce::roundToInt (margin * 1.3f));

    const auto topRowHeight = localBounds.getHeight() * .50f;

    topRowBorder = localBounds.removeFromTop (juce::roundToInt (topRowHeight));

    const auto topRowBorderHeight = topRowBorder.getHeight();
    borderLineThickness = topRowBorderHeight * .01f;
    borderCornerSize = topRowBorderHeight * .060f;

    auto topRowComponents = topRowBorder.reduced (juce::roundToInt (margin));

    auto topRowSliders = topRowComponents.removeFromLeft (
        juce::roundToInt (topRowComponents.getWidth() * .65f));
    const auto topRowButtonWidth = juce::roundToInt (topRowSliders.getWidth() * .033f);
    const auto adjustedTopRowComponentsWidth =
        topRowSliders.getWidth() - (topRowButtonWidth * 2.0f);
    const auto topRowSliderWidth =
        juce::roundToInt (adjustedTopRowComponentsWidth / 3.0f);

    const auto topRowButtonSpacer =
        juce::roundToInt ((topRowSliders.getHeight() - topRowButtonWidth) * .5f);

    // See below note about horizontal LabelSlider dimensions and button placement.
    const auto topRowButtonNudge = juce::roundToInt (topRowButtonWidth / 1.5f);

    const auto initialCrushButtonX = topRowSliders.getX();
    const auto crushEnableButtonBounds =
        topRowSliders.removeFromLeft (topRowButtonWidth)
            .reduced (0, topRowButtonSpacer)
            .withX (initialCrushButtonX + topRowButtonNudge);

    crushEnableButton.setBounds (crushEnableButtonBounds);
    crushSlider.setBounds (
        topRowSliders.removeFromLeft (topRowSliderWidth).reduced (topRowButtonNudge, 0));
    compressSlider.setBounds (topRowSliders.removeFromLeft (topRowSliderWidth));

    const auto initialSaturateButtonX = topRowSliders.getX();
    const auto saturateEnableButtonBounds =
        topRowSliders.removeFromLeft (topRowButtonWidth)
            .reduced (0, topRowButtonSpacer)
            .withX (initialSaturateButtonX + topRowButtonNudge);

    saturateEnableButton.setBounds (saturateEnableButtonBounds);
    saturateSlider.setBounds (
        topRowSliders.removeFromLeft (topRowSliderWidth).reduced (topRowButtonNudge, 0));

    const auto logoHeight = topRowComponents.getHeight() * .25f;
    const auto logoWidth = topRowComponents.getWidth() * .75f;

    const auto logoVerticalSpacer = (topRowComponents.getHeight() - logoHeight) / 2.0f;
    topRowComponents.removeFromTop (juce::roundToInt (logoVerticalSpacer));

    const auto logoHorizontalSpacer = (topRowComponents.getWidth() - logoWidth) / 2.0f;

    // logoHorizontalSpacer is the amount we hypothetically should remove from left
    // in order to have the log centred. However, the spacing is fudged here to account
    // for the fact that our sliders don't take up all of the horizontal space given
    // to them.
    const auto horizontalKludgeQuotient = .8f;
    topRowComponents.removeFromLeft (
        juce::roundToInt (logoHorizontalSpacer * horizontalKludgeQuotient));

    const auto valentineLogoBounds =
        topRowComponents.removeFromLeft (juce::roundToInt (logoWidth))
            .removeFromTop (juce::roundToInt (logoHeight));

    valentineTblLogo->setTransformToFit (
        valentineLogoBounds.toFloat(),
        juce::RectanglePlacement (juce::RectanglePlacement::centred
                                  | juce::RectanglePlacement::fillDestination));

    localBounds.removeFromTop (juce::roundToInt (margin * 2));
    bottomRowBorder = localBounds;

    auto bottomRowComponents = bottomRowBorder.reduced (juce::roundToInt (margin));

    const auto bottomRowButtonWidth =
        juce::roundToInt (bottomRowComponents.getWidth() / 60.0f);
    const auto adjustedBottomRowComponentsWidth =
        bottomRowComponents.getWidth() - bottomRowButtonWidth;
    const auto bottomRowSliderWidth =
        juce::roundToInt (adjustedBottomRowComponentsWidth / 6.0f);

    ratioSlider.setBounds (bottomRowComponents.removeFromLeft (bottomRowSliderWidth));
    attackSlider.setBounds (bottomRowComponents.removeFromLeft (bottomRowSliderWidth));
    releaseSlider.setBounds (bottomRowComponents.removeFromLeft (bottomRowSliderWidth));

    const auto dividerBounds = bottomRowComponents.removeFromLeft (bottomRowSliderWidth);

    bottomRowDivider =
        juce::Rectangle<int> (juce::roundToInt (dividerBounds.getCentreX()),
                              dividerBounds.getY(),
                              juce::roundToInt (borderLineThickness),
                              dividerBounds.getHeight());

    const auto bottomRowButtonSpacer =
        juce::roundToInt ((bottomRowComponents.getHeight() - bottomRowButtonWidth) * .5f);

    const auto bottomRowButtonNudge = juce::roundToInt (bottomRowButtonWidth / 1.5f);

    const auto clipButtonInitialX = bottomRowComponents.getX();
    const auto clipButtonBounds =
        bottomRowComponents.removeFromLeft (bottomRowButtonWidth)
            .reduced (0, bottomRowButtonSpacer)
            .withX (clipButtonInitialX + bottomRowButtonNudge);

    clipEnableButton.setBounds (clipButtonBounds);
    outputSlider.setBounds (bottomRowComponents.removeFromLeft (bottomRowSliderWidth)
                                .reduced (bottomRowButtonNudge, 0));
    mixSlider.setBounds (bottomRowComponents.removeFromLeft (bottomRowSliderWidth));
}
} // namespace tote_bag
} // namespace valentine
