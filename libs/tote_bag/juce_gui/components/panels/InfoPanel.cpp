// 2024 Tote Bag Labs

#include "InfoPanel.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

namespace tote_bag
{

InfoPanel::InfoPanel (std::function<void()> mouseUpCallback)
    : onMouseUp (mouseUpCallback)
{
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::paint (juce::Graphics& g)
{
    g.setColour (colours::plainWhite);
    g.fillRect (getLocalBounds());
}

void InfoPanel::mouseUp (const juce::MouseEvent& e)
{
    onMouseUp();
}

} // namespace tote_bag