// 2024 Tote Bag Labs

#include "ValentineTopRowPanel.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

#include "ValentineParameters.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"
#include "tote_bag/juce_gui/utilities/GraphicsUtilities.h"

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR
    #include <juce_gui_extra/juce_gui_extra.h>
#endif // JUCE_ENABLE_LIVE_CONSTANT_EDITOR

namespace tote_bag
{
namespace valentine
{
namespace detail
{
inline const juce::String kCrushSliderText = "CRUSH";
inline const juce::String kCompressSliderText = "COMPRESS";
inline const juce::String kSaturateSliderText = "SATURATE";
} // namespace detail

TopRowPanel::TopRowPanel (ValentineAudioProcessor& processor)
    : crushEnableButton (parameterID (VParameter::crushEnable), processor.treeState)
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
{
    addAndMakeVisible (crushEnableButton);
    addAndMakeVisible (crushSlider);
    addAndMakeVisible (compressSlider);
    addAndMakeVisible (saturateEnableButton);
    addAndMakeVisible (saturateSlider);
    addAndMakeVisible (valentineTblLogo.get());
}

TopRowPanel::~TopRowPanel()
{
}

void TopRowPanel::resized()
{
    auto bounds = getLocalBounds();

    auto sliders = bounds.removeFromLeft (juce::roundToInt (bounds.getWidth() * .65f));
    const auto buttonWidth = juce::roundToInt (sliders.getWidth() * .033f);
    const auto adjustedComponentWidth = sliders.getWidth() - (buttonWidth * 2.0f);
    const auto sliderInitialWidth = juce::roundToInt (adjustedComponentWidth / 3.0f);
    const auto buttonNudge = juce::roundToInt (buttonWidth / 5.0f);

    const auto setButtonAndSliderBounds = [&] (auto& button, auto& slider) {
        const auto initialButtonX = sliders.getX();
        const auto buttonX = initialButtonX + buttonNudge;
        const auto buttonY = sliders.getCentreY() - buttonWidth / 2;

        button.setBounds (sliders.removeFromLeft (buttonWidth)
                              .withX (buttonX)
                              .withY (buttonY)
                              .withHeight (buttonWidth));

        const auto sliderArea = sliders.removeFromLeft (sliderInitialWidth);
        const auto sliderWidth = sliderInitialWidth - buttonNudge;
        const auto sliderX = sliderArea.getCentreX() - sliderWidth / 2;

        slider.setBounds (sliderArea.withX (sliderX).withWidth (sliderWidth));
    };

    setButtonAndSliderBounds (crushEnableButton, crushSlider);
    compressSlider.setBounds (sliders.removeFromLeft (sliderInitialWidth));
    setButtonAndSliderBounds (saturateEnableButton, saturateSlider);

    const auto logoHeight = bounds.getHeight() * .25f;
    const auto logoWidth = bounds.getWidth() * .75f;

    const auto logoVerticalSpacer = (bounds.getHeight() - logoHeight) / 2.0f;
    bounds.removeFromTop (juce::roundToInt (logoVerticalSpacer));

    const auto logoHorizontalSpacer = (bounds.getWidth() - logoWidth) / 2.0f;

    // logoHorizontalSpacer is the amount we hypothetically should remove from left
    // in order to have the log centred. However, the spacing is fudged here to account
    // for the fact that our sliders don't take up all of the horizontal space given
    // to them.
    const auto horizontalKludgeQuotient = .8f;
    bounds.removeFromLeft (
        juce::roundToInt (logoHorizontalSpacer * horizontalKludgeQuotient));

    const auto valentineLogoBounds = bounds.removeFromLeft (juce::roundToInt (logoWidth))
                                         .removeFromTop (juce::roundToInt (logoHeight));

    valentineTblLogo->setTransformToFit (
        valentineLogoBounds.toFloat(),
        juce::RectanglePlacement (juce::RectanglePlacement::centred
                                  | juce::RectanglePlacement::fillDestination));
}

} // namespace valentine
} // namespace tote_bag
