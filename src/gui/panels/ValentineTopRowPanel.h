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
class TopRowPanel : public juce::Component
{
public:
    TopRowPanel (ValentineAudioProcessor& processor);
    ~TopRowPanel() override;

    void resized() override;

private:
    ToggleButton crushEnableButton;
    LabelSlider crushSlider;
    LabelSlider compressSlider;
    ToggleButton saturateEnableButton;
    LabelSlider saturateSlider;

    std::unique_ptr<juce::Drawable> valentineTblLogo;
};
} // namespace valentine
} // namespace tote_bag
