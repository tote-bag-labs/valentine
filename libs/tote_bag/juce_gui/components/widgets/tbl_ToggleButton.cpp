// 2023 Tote Bag Labs

#include "tbl_ToggleButton.h"

namespace tote_bag
{
ToggleButton::ToggleButton (const juce::String& parameterId,
                            juce::AudioProcessorValueTreeState& stateToControl)
    : juce::ToggleButton (parameterId)
    , buttonValue (stateToControl, parameterId, *this)
{
    setClickingTogglesState (true);
}
} // namespace tote_bag
