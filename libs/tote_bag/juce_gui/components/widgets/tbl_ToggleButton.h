
// 2023 Tote Bag Labs

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

namespace tote_bag
{
class ToggleButton : public juce::ToggleButton
{
public:
    ToggleButton (const juce::String& parameterId,
                  juce::AudioProcessorValueTreeState& stateToControl);

    enum ColourIds
    {
        defaultFillColourId = 0x1006504,
        activeFillColourId = 0x1006505,
        strokeColourId = 0x1006506,
    };

private:
    juce::AudioProcessorValueTreeState::ButtonAttachment buttonValue;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToggleButton)
};
} // namespace tote_bag
