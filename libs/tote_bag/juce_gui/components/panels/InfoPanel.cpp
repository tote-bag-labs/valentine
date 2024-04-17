// 2024 Tote Bag Labs

#include "InfoPanel.h"

#include "generated/version.h"

#include "tote_bag/juce_gui/lookandfeel/LookAndFeel.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR
    #include <juce_gui_extra/juce_gui_extra.h>
#endif // JUCE_ENABLE_LIVE_CONSTANT_EDITOR

namespace tote_bag
{

namespace detail
{
constexpr int kNumInfoTextLines = 5;
constexpr float kTotieWidth = 2098.55f;
constexpr float kTotieHeight = 2094.79f;
constexpr float kTotieWidthHeightRatio = kTotieWidth / kTotieHeight;
}

InfoPanel::InfoPanel (std::function<void()> mouseUpCallback)
    : onMouseUp (mouseUpCallback)
    , githubURL ("https://github.com/tote-bag-labs/valentine")
{
    totieWatermark =
        juce::Drawable::createFromImageData (BinaryData::totie_watermark_svg,
                                             BinaryData::totie_watermark_svgSize);
}

InfoPanel::~InfoPanel()
{
}

void InfoPanel::paint (juce::Graphics& g)
{
    g.setColour (colours::plainWhite);
    g.fillRect (getLocalBounds());

    drawInfoText (g);
    drawWatermark (g);
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
    const auto boundsHeight = bounds.getHeight();

    const auto textLineHeight = juce::roundToInt (boundsHeight / 32.0f);
    const auto textAreaHeight =
        juce::roundToInt (textLineHeight * detail::kNumInfoTextLines);
    const auto textAreaY = juce::roundToInt (bounds.getCentreY() - textAreaHeight / 2.0);

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
        const auto placedBounds = textBounds.removeFromTop (textLineHeight);

        g.drawFittedText (text, placedBounds, juce::Justification::centredBottom, 1);

        return placedBounds;
    };

    placeText ("Valentine");
    placeText ("Tote Bag Labs");
    placeText (CURRENT_VERSION);
    placeText (juce::String ("Build: " + juce::String (BUILD_ID)));

    g.setColour (colours::valentinePink);
    urlBounds = placeText ("Github");
}

void InfoPanel::drawWatermark (juce::Graphics& g)
{
    if (totieWatermark)
    {
        const auto bounds = getLocalBounds();
        const auto boundsWidth = bounds.getWidth();
        const auto boundsHeight = bounds.getHeight();

        const auto totieX = juce::roundToInt (bounds.getX() + (boundsWidth * .28f));
        const auto totieY = juce::roundToInt (bounds.getY() - (boundsHeight * .08f));
        const auto totieHeight = juce::roundToInt (boundsHeight * 1.22f);
        const auto totieBounds = bounds.withX (totieX)
                                     .withY (totieY)
                                     .withHeight (totieHeight)
                                     .withWidth (juce::roundToInt (
                                         totieHeight * detail::kTotieWidthHeightRatio));

        totieWatermark->drawWithin (g,
                                    totieBounds.toFloat(),
                                    juce::RectanglePlacement::stretchToFit,
                                    1.0f);
    }
}

} // namespace tote_bag
