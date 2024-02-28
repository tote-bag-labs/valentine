// 2024 Tote Bag Labs

#include "InfoPanel.h"

#include "generated/version.h"

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
    const auto bounds = getLocalBounds();

    g.setColour (colours::plainWhite);
    g.fillRect (bounds);

    const auto boundsWidth = bounds.getWidth();
    const auto boundsHeight = bounds.getHeight();

    auto textArea = bounds.reduced (juce::roundToInt (boundsWidth / 4.0f),
                                    juce::roundToInt (boundsHeight / 4.0f));

    const auto textAreaHeight = textArea.getHeight();
    const auto textHeight = juce::roundToInt (textAreaHeight / 8.0f);
    const auto textMargin = juce::roundToInt (textAreaHeight / 64.0f);

    g.setColour (colours::plainBlack);

    const auto placeText = [&] (const juce::String& text) {
        g.drawFittedText (text,
                          textArea.removeFromTop (textHeight),
                          juce::Justification::centredBottom,
                          1);
        textArea.removeFromTop (textMargin);
    };

    placeText ("Valentine");
    placeText ("Tote Bag Labs");
    placeText (CURRENT_VERSION);
    placeText (juce::String ("Build: " + juce::String (BUILD_ID)));
}

void InfoPanel::mouseUp (const juce::MouseEvent& e)
{
    onMouseUp();
}

} // namespace tote_bag