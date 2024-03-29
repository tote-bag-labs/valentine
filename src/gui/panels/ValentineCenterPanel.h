// 2023 Tote Bag Labs

#pragma once

#include "ValentineBottomRowPanel.h"
#include "ValentineTopRowPanel.h"
#include "tote_bag/juce_gui/components/widgets/LabelSlider.h"
#include "tote_bag/juce_gui/components/widgets/tbl_ToggleButton.h"

#include <juce_gui_basics/juce_gui_basics.h>

class ValentineAudioProcessor;

namespace tote_bag
{
namespace valentine
{
class CenterPanel : public juce::Component
{
public:
    CenterPanel (ValentineAudioProcessor& processor);
    ~CenterPanel() override;

    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    juce::Rectangle<int> topRowBorder;
    juce::Rectangle<int> bottomRowBorder;
    float borderLineThickness;
    float borderCornerSize;

    TopRowPanel topRow;
    BottomRowPanel bottomRow;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CenterPanel)
};
} // namespace valentine
} // namespace tote_bag
