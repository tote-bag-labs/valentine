// 2023 Tote Bag Labs

#include "DrawableParameterButton.h"

namespace tote_bag
{
DrawableParameterButton::DrawableParameterButton (
    const juce::String& buttonText,
    const juce::Drawable* buttonOnImage,
    const juce::Drawable* buttonOffImage,
    const juce::String& parameterId,
    juce::AudioProcessorValueTreeState& stateToControl)
    : juce::DrawableButton (buttonText, ButtonStyle::ImageStretched)
    , buttonValue (stateToControl, parameterId, *this)
{
    setButtonText (buttonText);
    setClickingTogglesState (true);
    setImages (buttonOffImage,
               buttonOnImage,
               nullptr,
               nullptr,
               buttonOnImage,
               nullptr,
               nullptr,
               nullptr);
}
} // namespace tote_bag
