// 2024 Tote Bag Labs

#include "InfoPanel.h"

#include "generated/version.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeel.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

namespace tote_bag
{

namespace detail
{
constexpr int kNumInfoTextLines = 5;
}

InfoPanel::InfoPanel (std::function<void()> mouseUpCallback)
    : onMouseUp (mouseUpCallback)
    , githubURL ("https://github.com/tote-bag-labs/valentine")
{
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::paint (juce::Graphics& g)
{
    g.setColour (colours::plainWhite);
    g.fillRect (getLocalBounds());

    drawInfoText (g);
}

void InfoPanel::mouseUp (const juce::MouseEvent& e)
{
    if (urlBounds.contains (e.getPosition()))
    {
        githubURL.launchInDefaultBrowser();
    }
    else
    {
        onMouseUp();
    }
}

void InfoPanel::drawInfoText (juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const auto boundsWidth = bounds.getWidth();
    const auto boundsHeight = bounds.getHeight();

    const auto textLineHeight = juce::roundToInt (boundsHeight / 32.0f);
    const auto textAreaHeight = textLineHeight * detail::kNumInfoTextLines;
    const auto textAreaY = bounds.getCentreY() - textAreaHeight / 2.0;

    auto textBounds = bounds.withY (textAreaY).withHeight (textAreaHeight);

    g.setColour (colours::plainBlack);

    if (auto* lnf = dynamic_cast<tote_bag::LookAndFeel*> (&getLookAndFeel()))
    {
        g.setFont (lnf->getInfoTextFont().withHeight (textLineHeight));
    }
    else
    {
        // This needs a tote bag look and feel to work correctly
        jassertfalse;
    }

    const auto placeText = [&] (const juce::String& text) {
        const auto bounds = textBounds.removeFromTop (textLineHeight);

        g.drawFittedText (text, bounds, juce::Justification::centredBottom, 1);

        return bounds;
    };

    placeText ("Valentine");
    placeText ("Tote Bag Labs");
    placeText (CURRENT_VERSION);
    placeText (juce::String ("Build: " + juce::String (BUILD_ID)));

    g.setColour (colours::valentinePink);
    urlBounds = placeText ("Github");
}

} // namespace tote_bag
