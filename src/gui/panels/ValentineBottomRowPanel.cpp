// 2024 Tote Bag Labs

#include "ValentineBottomRowPanel.h"
#include "BinaryData.h"
#include "PluginProcessor.h"

#include "ValentineParameters.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"
#include "tote_bag/juce_gui/utilities/GraphicsUtilities.h"

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR
    #include <juce_gui_extra / juce_gui_extra.h>
#endif // JUCE_ENABLE_LIVE_CONSTANT_EDITOR

namespace tote_bag
{
namespace valentine
{
namespace detail
{
inline const juce::String kRatioSliderText = "RATIO";
inline const juce::String kAttackSliderText = "ATTACK";
inline const juce::String kReleaseSliderText = "RELEASE";
inline const juce::String kOutputSliderText = "OUTPUT";
inline const juce::String kMixSliderText = "MIX";
} // namespace detail

BottomRowPanel::BottomRowPanel (ValentineAudioProcessor& processor)
    : ratioSlider (detail::kRatioSliderText,
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
    addAndMakeVisible (ratioSlider);
    addAndMakeVisible (attackSlider);
    addAndMakeVisible (releaseSlider);
    addAndMakeVisible (clipEnableButton);
    addAndMakeVisible (outputSlider);
    addAndMakeVisible (mixSlider);
}

BottomRowPanel::~BottomRowPanel()
{
}

void BottomRowPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::black);

    g.fillRect (divider);
}

void BottomRowPanel::resized()
{
    auto bounds = getLocalBounds();

    const auto clipButtonWidth = juce::roundToInt (bounds.getWidth() / 60.0f);
    const auto adjustedComponentWidth = bounds.getWidth() - clipButtonWidth;
    const auto sliderWidth = juce::roundToInt (adjustedComponentWidth / 6.0f);

    ratioSlider.setBounds (bounds.removeFromLeft (sliderWidth));
    attackSlider.setBounds (bounds.removeFromLeft (sliderWidth));
    releaseSlider.setBounds (bounds.removeFromLeft (sliderWidth));

    const auto dividerCentreX = bounds.getX() + juce::roundToInt (sliderWidth / 2.0f);
    const auto dividerThickness = juce::roundToInt (bounds.getHeight() * .015f);

    divider = bounds.removeFromLeft (sliderWidth)
                  .withX (dividerCentreX)
                  .withWidth (dividerThickness);

    const auto buttonSpacer =
        juce::roundToInt ((bounds.getHeight() - clipButtonWidth) * .5f);

    const auto buttonNudge = juce::roundToInt (clipButtonWidth / 1.5f);

    const auto clipButtonInitialX = bounds.getX();
    const auto clipButtonBounds = bounds.removeFromLeft (clipButtonWidth)
                                      .reduced (0, buttonSpacer)
                                      .withX (clipButtonInitialX + buttonNudge);

    clipEnableButton.setBounds (clipButtonBounds);
    outputSlider.setBounds (bounds.removeFromLeft (sliderWidth).reduced (buttonNudge, 0));
    mixSlider.setBounds (bounds.removeFromLeft (sliderWidth));
}

} // namespace valentine
} // namespace tote_bag
