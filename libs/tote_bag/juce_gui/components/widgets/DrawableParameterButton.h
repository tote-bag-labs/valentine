// 2023 Tote Bag Labs

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace tote_bag
{
class DrawableParameterButton : public juce::DrawableButton
{
public:
    DrawableParameterButton (const juce::String& buttonText,
                             const juce::Drawable* buttonOnImage,
                             const juce::Drawable* buttonOffImage,
                             const juce::String& parameterId,
                             juce::AudioProcessorValueTreeState& stateToControl);

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment buttonValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DrawableParameterButton)
};
} // namespace tote_bag