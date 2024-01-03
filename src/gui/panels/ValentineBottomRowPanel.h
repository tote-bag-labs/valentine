// 2024 Tote Bag Labs

#pragma once

#include "tote_bag/juce_gui/components/widgets/LabelSlider.h"
#include "tote_bag/juce_gui/components/widgets/tbl_ToggleButton.h"

#include <juce_gui_basics/juce_gui_basics.h>

class ValentineAudioProcessor;

namespace tote_bag
{
namespace valentine
{
class BottomRowPanel : public juce::Component
{
public:
    BottomRowPanel (ValentineAudioProcessor& processor);
    ~BottomRowPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Rectangle<int> divider;

    LabelSlider ratioSlider;
    LabelSlider attackSlider;
    LabelSlider releaseSlider;

    ToggleButton clipEnableButton;
    LabelSlider outputSlider;
    LabelSlider mixSlider;
};
} // namespace valentine
} // namespace tote_bag
