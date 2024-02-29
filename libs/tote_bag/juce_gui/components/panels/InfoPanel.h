// 2024 Tote Bag Labs

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

namespace tote_bag
{

class InfoPanel : public juce::Component
{
public:
    InfoPanel (std::function<void()> mouseUpCallback);

    ~InfoPanel() override;

    void paint (juce::Graphics& g) override;

    void mouseUp (const juce::MouseEvent& e) override;

private:
    std::function<void()> onMouseUp;

    juce::Rectangle<int> urlBounds;
    juce::URL githubURL;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoPanel)
};

} // namespace tote_bag
