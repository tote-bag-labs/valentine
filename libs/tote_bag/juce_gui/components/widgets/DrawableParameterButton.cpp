// 2023 Tote Bag Labs

#include "DrawableParameterButton.h"

namespace tote_bag
{
DrawableParameterButton::DrawableParameterButton (
    const juce::Drawable* buttonOnImage,
    const juce::Drawable* buttonOffImage,
    const juce::String& parameterId,
    juce::AudioProcessorValueTreeState& stateToControl)
    : juce::DrawableButton (parameterId, ButtonStyle::ImageStretched)
    , buttonValue (stateToControl, parameterId, *this)
{
    setClickingTogglesState (true);
    setImages (buttonOffImage,
               nullptr,
               nullptr,
               nullptr,
               buttonOnImage,
               nullptr,
               nullptr,
               nullptr);
}
} // namespace tote_bag
