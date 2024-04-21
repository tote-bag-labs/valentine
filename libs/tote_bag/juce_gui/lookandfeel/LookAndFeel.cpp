/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 26 Dec 2019 7:02:26pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "LookAndFeelConstants.h"

#include "tote_bag/juce_gui/components/widgets/FlatTextButton.h"
#include "tote_bag/juce_gui/components/widgets/tbl_ToggleButton.h"

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR
    #include <juce_gui_extra/juce_gui_extra.h>
#endif // JUCE_ENABLE_LIVE_CONSTANT_EDITOR

namespace tote_bag
{

LookAndFeel::LookAndFeel()
{
    using namespace tote_bag::colours;

    // Set up some default colours. These may be overriden
    // in client code.

    setupDefaultMeterColours();

    // slider colours
    setColour (ColourIds::knobColourId, sliderGrey);
    setColour (ColourIds::pointerColourId, juce::Colours::black);

    // slider text box colours
    setColour (juce::Slider::backgroundColourId, juce::Colour (0xff2f2f2f));
    setColour (juce::Slider::thumbColourId, juce::Colour (0xffe7e7e7));
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour (juce::Slider::textBoxOutlineColourId, valentinePink);
    setColour (juce::Slider::rotarySliderOutlineColourId, valentinePinkDark);
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::floralwhite);

    // A transparent-ish grey
    setColour (ToggleButton::defaultFillColourId, colours::transparentGrey);

    // A bright, not totally solid green
    setColour (ToggleButton::activeFillColourId, juce::Colour (0xc749ff1b));

    // A solid grey
    setColour (ToggleButton::strokeColourId, juce::Colour (0xff7f7f7f));

    // so we don't get background painting on drawable buttons
    setColour (juce::DrawableButton::backgroundOnColourId,
               juce::Colours::transparentWhite);

    setColour (juce::ComboBox::ColourIds::backgroundColourId, slateGrey);
    setColour (juce::ComboBox::ColourIds::textColourId, plainWhite);
}

void LookAndFeel::drawDrawableKnob (juce::Graphics& g,
                                    const float radius,
                                    const float drawableWidth,
                                    juce::Drawable& sliderImage,
                                    const float toAngle,
                                    const juce::Rectangle<float> bounds)
{
    const auto rw = radius * 2.0f;
    const auto realW = rw / drawableWidth;

    const auto halfDrawableWidth = drawableWidth / 2.0f;

    sliderImage.setTransform (
        juce::AffineTransform::rotation (toAngle, halfDrawableWidth, halfDrawableWidth)
            .scaled (realW, realW, halfDrawableWidth, halfDrawableWidth));

    const auto cX = bounds.getCentreX() - halfDrawableWidth;
    const auto cY = bounds.getCentreY() - halfDrawableWidth;
    sliderImage.drawAt (g, cX, cY, 1.0f);
}

void LookAndFeel::drawRotarySliderMeter (juce::Graphics& g,
                                         const juce::Rectangle<float> bounds,
                                         const float lineWidth,
                                         const float radius,
                                         const float startAngle,
                                         float endAngle,
                                         const float toAngle,
                                         juce::Slider& slider)
{
    auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour (juce::Slider::rotarySliderFillColourId);

    juce::Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 radius,
                                 radius,
                                 0.0f,
                                 startAngle,
                                 endAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc,
                  juce::PathStrokeType (lineWidth,
                                        juce::PathStrokeType::curved,
                                        juce::PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        juce::Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                radius,
                                radius,
                                0.0f,
                                startAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc,
                      juce::PathStrokeType (lineWidth,
                                            juce::PathStrokeType::curved,
                                            juce::PathStrokeType::rounded));
    }
}

void LookAndFeel::drawRotarySliderBase (juce::Graphics& g,
                                        const float radius,
                                        const float toAngle,
                                        const juce::Rectangle<float> bounds,
                                        juce::Slider&)
{
    const auto centreX = bounds.getCentreX();
    const auto centreY = bounds.getCentreY();
    const auto rx = centreX - radius;
    const auto ry = centreY - radius;
    const auto rw = radius * 2.0f;

    g.setColour (findColour (juce::Slider::backgroundColourId));
    g.fillEllipse (rx, ry, rw, rw);

    juce::Path p;
    const auto pointerLength = radius * 0.41f;
    const auto pointerThickness = radius * .085f;
    const auto pointerX = -pointerThickness * 0.5f;
    const auto initialPointerY = -radius;

    // Increasing this value will cause the pointer to be drawn closer
    // to the center of the slider.
    const auto sliderEdgeOffset = juce::roundToInt (juce::jmax ((radius * .12f), 1.0f));
    const auto pointerY = initialPointerY + sliderEdgeOffset;

    const auto cornerSize = pointerThickness * .5f;

    p.addRoundedRectangle (pointerX,
                           pointerY,
                           pointerThickness,
                           pointerLength,
                           cornerSize,
                           cornerSize,
                           true,
                           true,
                           true,
                           true);
    p.applyTransform (
        juce::AffineTransform::rotation (toAngle).translated (bounds.getCentreX(),
                                                              bounds.getCentreY()));

    g.setColour (findColour (juce::Slider::thumbColourId));
    g.fillPath (p);
}

void LookAndFeel::drawRotarySlider (juce::Graphics& g,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    float sliderPos,
                                    const float rotaryStartAngle,
                                    const float rotaryEndAngle,
                                    juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat();
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = radius * 0.090f;
    auto arcRadius = radius - lineW;
    const auto toAngle =
        rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    drawRotarySliderMeter (g,
                           bounds,
                           lineW,
                           arcRadius,
                           rotaryStartAngle,
                           rotaryEndAngle,
                           toAngle,
                           slider);

    const auto knobRadius = arcRadius * .87f;

    drawRotarySliderBase (g, knobRadius, toAngle, bounds, slider);
}

juce::Font LookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    const auto fontHeight = juce::jmax (7.0f, buttonHeight * 0.40f);
    return fontHolder.getFont ("RobotoRegular_ttf").withHeight (fontHeight);
}

juce::Font LookAndFeel::getLabelFont (juce::Label& l)
{
    const auto fontHeight = static_cast<float> (l.getHeight());
    // Slider value box font
    if (dynamic_cast<juce::Slider*> (l.getParentComponent()))
    {
        return fontHolder.getFont ("RobotoMonoRegular_ttf").withHeight (fontHeight);
    }
    return fontHolder.getFont ("RobotoRegular_ttf")
        .withHeight (fontHeight)
        .withExtraKerningFactor (.05f);
}

void LookAndFeel::drawButtonBackground (juce::Graphics& g,
                                        juce::Button& button,
                                        const juce::Colour& backgroundColour,
                                        bool,
                                        bool shouldDrawButtonAsDown)
{
    auto buttonArea =
        button.getLocalBounds().reduced (juce::roundToInt (button.getHeight() * .15));
    const auto h = buttonArea.getHeight();

    const auto cornerSize = juce::roundToInt (h * .5f);

    g.setColour (shouldDrawButtonAsDown
                     ? button.findColour (juce::TextButton::buttonOnColourId)
                     : backgroundColour);

    g.fillRoundedRectangle (buttonArea.toFloat(), cornerSize);

    g.setColour (tote_bag::colours::plainBlack);
    g.drawRoundedRectangle (buttonArea.toFloat(), cornerSize, 1);
}

void LookAndFeel::drawFlatButtonBackground (juce::Graphics& g,
                                            tote_bag::FlatTextButton& button,
                                            const juce::Colour& backgroundColour,
                                            bool shouldDrawButtonAsHighlighted,
                                            bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();

    auto r = juce::jmin (bounds.getWidth(), bounds.getHeight());
    auto cornerSize = r * .5f;

    auto baseColour =
        backgroundColour
            .withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
            .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour (baseColour);

    auto flatOnLeft = button.isConnectedOnLeft();
    auto flatOnRight = button.isConnectedOnRight();
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        juce::Path path;
        path.addRoundedRectangle (bounds.getX(),
                                  bounds.getY(),
                                  bounds.getWidth(),
                                  bounds.getHeight(),
                                  cornerSize,
                                  cornerSize,
                                  !(flatOnLeft || flatOnTop),
                                  !(flatOnRight || flatOnTop),
                                  !(flatOnLeft || flatOnBottom),
                                  !(flatOnRight || flatOnBottom));

        g.fillPath (path);
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
    }
}

void LookAndFeel::drawButtonText (juce::Graphics& g, juce::TextButton& button, bool, bool)
{
    auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button
                     .findColour (button.getToggleState()
                                      ? juce::TextButton::textColourOnId
                                      : juce::TextButton::textColourOffId)
                     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    auto yIndent = juce::jmin (4, button.proportionOfHeight (0.5f));
    auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

    auto fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
    auto leftIndent =
        juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    auto rightIndent =
        juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    auto textWidth = button.getWidth() - leftIndent - rightIndent;

    //auto edge = 4;
    //auto offset = isButtonDown ? edge / 2 : 0;

    if (textWidth > 0)
        g.drawFittedText (button.getButtonText(),
                          leftIndent,
                          yIndent,
                          textWidth,
                          button.getHeight() - yIndent * 2,
                          juce::Justification::centred,
                          2);
}

juce::Font LookAndFeel::getComboBoxFont (juce::ComboBox&)
{
    // Font height is determined by the label that is used to
    // draw text.
    return fontHolder.getFont ("RobotoRegular_ttf");
}

void LookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    const auto w = juce::roundToInt (box.getWidth() * .8f);
    const auto x = juce::roundToInt (w * .06);
    const auto h = juce::roundToInt (box.getHeight() * .4f);
    const auto yOffset = juce::roundToInt (h * .1f);
    const auto y = ((box.getHeight() - h) / 2) + yOffset;

    label.setBounds (x, y, w, h);

    label.setFont (getComboBoxFont (box));
}

void LookAndFeel::drawComboBox (juce::Graphics& g,
                                int,
                                int,
                                bool,
                                int,
                                int,
                                int,
                                int,
                                juce::ComboBox& box)
{
    const auto boxBounds =
        box.getLocalBounds().reduced (juce::roundToInt (box.getHeight() * .1f));

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));

    const auto h = boxBounds.getHeight();
    const auto cornerSize = h * .075f;
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);
}

void LookAndFeel::drawPopupMenuItem (juce::Graphics& g,
                                     const juce::Rectangle<int>& area,
                                     bool,
                                     bool,
                                     bool isHighlighted,
                                     bool isTicked,
                                     bool,
                                     const juce::String& text,
                                     const juce::String&,
                                     const juce::Drawable*,
                                     const juce::Colour*)
{
    using namespace colours;

    juce::Rectangle<int> r (area);

    const auto comboBoxColour =
        findColour (juce::ComboBox::ColourIds::backgroundColourId);

    juce::Colour fillColour = isHighlighted ? comboBoxColour.brighter() : comboBoxColour;
    g.setColour (fillColour);
    g.fillRect (r.getX(), r.getY(), r.getWidth(), r.getHeight() - 1);

    const auto comboBoxTextColour = findColour (juce::ComboBox::ColourIds::textColourId);
    juce::Colour myTextColour =
        isTicked ? comboBoxTextColour : comboBoxTextColour.darker();
    g.setColour (myTextColour);

    auto fHeight = juce::jmax (7.0f, r.getHeight() * 0.6f);
    g.setFont (fontHolder.getFont ("RobotoRegular_ttf").withHeight (fHeight));

    r.setLeft (10);
    r.setY (1);
    g.drawFittedText (text, r, juce::Justification::left, 1);
}

void LookAndFeel::drawToggleButton (juce::Graphics& g,
                                    juce::ToggleButton& button,
                                    bool,
                                    bool)
{
    auto bounds = button.getLocalBounds();
    const auto margin = juce::roundToInt (bounds.getHeight() * .1f);
    bounds.reduce (margin, margin);

    const auto buttonIsActive = button.getToggleState();
    const auto fillColour = buttonIsActive
                                ? findColour (ToggleButton::activeFillColourId)
                                : findColour (ToggleButton::defaultFillColourId);

    g.setColour (fillColour);
    g.fillEllipse (bounds.toFloat());

    const auto outlineThickness = bounds.getHeight() * .10f;

    g.setColour (findColour (ToggleButton::strokeColourId));
    g.drawEllipse (bounds.toFloat(), outlineThickness);
}

juce::Slider::SliderLayout LookAndFeel::getSliderLayout (juce::Slider& slider)
{
    // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

    auto textBoxPos = slider.getTextBoxPosition();

    auto localBounds = slider.getLocalBounds();

    auto sDiameter = juce::jmin (localBounds.getWidth(), localBounds.getHeight());
    auto textBoxWidth = juce::roundToInt (sDiameter * .66f);
    auto textBoxHeight = juce::roundToInt (sDiameter * .15);

    juce::Slider::SliderLayout layout;

    // 2. set the textBox bounds

    if (textBoxPos != juce::Slider::NoTextBox)
    {
        if (slider.isBar())
        {
            layout.textBoxBounds = localBounds;
        }
        else
        {
            layout.textBoxBounds.setWidth (textBoxWidth);
            layout.textBoxBounds.setHeight (textBoxHeight);

            if (textBoxPos == juce::Slider::TextBoxLeft)
                layout.textBoxBounds.setX (0);
            else if (textBoxPos == juce::Slider::TextBoxRight)
                layout.textBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
            else /* above or below -> centre horizontally */
                layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);

            if (textBoxPos == juce::Slider::TextBoxAbove)
                layout.textBoxBounds.setY (0);
            else if (textBoxPos == juce::Slider::TextBoxBelow)
                layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight);
            else /* left or right -> centre vertically */
                layout.textBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
        }
    }

    // 3. set the slider bounds

    layout.sliderBounds = localBounds;

    if (slider.isBar())
    {
        layout.sliderBounds.reduce (1, 1); // bar border
    }
    else
    {
        if (textBoxPos == juce::Slider::TextBoxLeft)
            layout.sliderBounds.removeFromLeft (textBoxWidth);
        else if (textBoxPos == juce::Slider::TextBoxRight)
            layout.sliderBounds.removeFromRight (textBoxWidth);
        else if (textBoxPos == juce::Slider::TextBoxAbove)
            layout.sliderBounds.removeFromTop (textBoxHeight);
        else if (textBoxPos == juce::Slider::TextBoxBelow)
            layout.sliderBounds.removeFromBottom (textBoxHeight);

        const int thumbIndent = getSliderThumbRadius (slider);

        if (slider.isHorizontal())
            layout.sliderBounds.reduce (thumbIndent, 0);
        else if (slider.isVertical())
            layout.sliderBounds.reduce (0, thumbIndent);
    }

    return layout;
}

juce::Font LookAndFeel::getInfoTextFont()
{
    return fontHolder.getFont ("RobotoMonoMedium_ttf").withExtraKerningFactor (.05f);
}

} // namespace tote_bag
