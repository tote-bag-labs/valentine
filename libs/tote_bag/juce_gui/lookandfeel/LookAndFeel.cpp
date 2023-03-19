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

#include "BinaryData.h"

namespace
{
const juce::Font getCustomFont()
{
    static auto typeface = juce::Typeface::createSystemTypefaceFor (BinaryData::MontserratMedium_ttf,
                                                                    BinaryData::MontserratMedium_ttfSize);
    return juce::Font (typeface);
}
}

namespace tote_bag
{
LookAndFeel::LookAndFeel()
{
    using namespace tote_bag::laf_constants;

    setupDefaultMeterColours();

    setDefaultLookAndFeel (this);

    // slider colours
    setColour (ColourIds::knobColourId, juce::Colours::grey);
    setColour (ColourIds::pointerColourId, juce::Colours::black);

    // slider text box colours
    setColour (juce::Slider::textBoxTextColourId, juce::Colours::black);
    setColour (juce::Slider::textBoxOutlineColourId, vPinkDark);
    setColour (juce::Slider::rotarySliderOutlineColourId, vPinkDark);
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colours::floralwhite);
}

void LookAndFeel::drawSliderMeter (juce::Graphics& g,
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
                                        juce::PathStrokeType::butt));

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
                                            juce::PathStrokeType::butt));
    }
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

    sliderImage.setTransform (juce::AffineTransform::rotation (toAngle,
                                                               halfDrawableWidth,
                                                               halfDrawableWidth)
                                  .scaled (realW,
                                           realW,
                                           halfDrawableWidth,
                                           halfDrawableWidth));

    const auto cX = bounds.getCentreX() - halfDrawableWidth;
    const auto cY = bounds.getCentreY() - halfDrawableWidth;
    sliderImage.drawAt (g, cX, cY, 1.0f);
}

void LookAndFeel::drawKnob (juce::Graphics& g,
                            const float radius,
                            const float toAngle,
                            const juce::Rectangle<float> bounds,
                            juce::Slider&,
                            const bool withDropShadow)
{
    const auto centreX = bounds.getCentreX();
    const auto centreY = bounds.getCentreY();

    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;

    // Fill main knob area
    auto fillColour = findColour (ColourIds::knobColourId);
    g.setColour (fillColour);
    g.fillEllipse (rx, ry, rw, rw);

    // Get thicknesses for outline rings...
    const auto innerOutlineThickness = juce::jmax ((rw * .05f), 1.0f);
    const auto outerOutlineThickness = innerOutlineThickness * .15f;

    // Offset inner outline by its thickness
    auto innerOutlineRadius = radius - (innerOutlineThickness * .5f);
    auto innerOutlineRx = centreX - innerOutlineRadius;
    auto innerOutlineRy = centreY - innerOutlineRadius;
    auto innerOutlineRw = innerOutlineRadius * 2.0f;

    // Draw inner outline
    g.setColour (fillColour.darker (.15f));
    g.drawEllipse (innerOutlineRx, innerOutlineRy, innerOutlineRw, innerOutlineRw, innerOutlineThickness);

    // Offset outer outline by its thickness
    auto outerOutlineRadius = radius - (outerOutlineThickness * .5f);
    auto outerOutlineRx = centreX - outerOutlineRadius;
    auto outerOutlineRy = centreY - outerOutlineRadius;
    auto outerOutlineRw = outerOutlineRadius * 2.0f;

    // Draw outer outline
    if (withDropShadow)
    {
        auto xOffset = juce::jmin (juce::roundToInt (innerOutlineThickness * .25), 1);
        auto yOffset = juce::jmin (juce::roundToInt (innerOutlineThickness * .5), 1);

        auto shadow = juce::DropShadow (fillColour.darker(),
                                        innerOutlineThickness,
                                        { xOffset, yOffset });
        juce::Path shadowPath;
        shadowPath.addEllipse (outerOutlineRx, outerOutlineRy, outerOutlineRw, outerOutlineRw);
        shadow.drawForPath (g, shadowPath);
    }
    else
    {
        g.setColour (fillColour.darker (.85f));
        g.drawEllipse (outerOutlineRx, outerOutlineRy, outerOutlineRw, outerOutlineRw, outerOutlineThickness);
    }

    // Pointer
    juce::Path p;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = pointerLength * .2f;
    auto cornerSize = pointerThickness * .35f;
    p.addRoundedRectangle (-pointerThickness * 0.5f, -radius + innerOutlineThickness, pointerThickness, pointerLength, cornerSize, cornerSize, true, true, false, false);
    p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (bounds.getCentreX(), bounds.getCentreY()));

    auto pointerColour = findColour (ColourIds::pointerColourId);
    g.setColour (pointerColour);
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
    auto bounds = juce::Rectangle<int> (x, y, width, height).toFloat().reduced (10);
    auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = radius * 0.125f;
    auto arcRadius = radius - lineW * 0.5f;
    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    drawSliderMeter (g, bounds, lineW, arcRadius, rotaryStartAngle, rotaryEndAngle, toAngle, slider);

    const auto knobRadius = arcRadius * .80f;

    drawKnob (g, knobRadius, toAngle, bounds, slider, true);
}

//=============================================================================

juce::Typeface::Ptr LookAndFeel::getTypefaceForFont (const juce::Font&)
{
    return getCustomFont().getTypefacePtr();
}

juce::Font LookAndFeel::getTextButtonFont (juce::TextButton&, int buttonHeight)
{
    return { juce::jmax (7.0f, buttonHeight * 0.8f) };
}

juce::Font LookAndFeel::getLabelFont (juce::Label& l)
{
    return { static_cast<float> (l.getHeight()) };
}

void LookAndFeel::drawButtonBackground (juce::Graphics& g,
                                        juce::Button& button,
                                        const juce::Colour& backgroundColour,
                                        bool,
                                        bool)
{
    auto buttonArea = button.getLocalBounds();
    const auto h = buttonArea.getHeight();

    const auto cornerSize = h * .15;

    g.setColour (backgroundColour);

    g.fillRoundedRectangle (buttonArea.toFloat(), cornerSize);
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

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
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
        path.addRoundedRectangle (bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), cornerSize, cornerSize, !(flatOnLeft || flatOnTop), !(flatOnRight || flatOnTop), !(flatOnLeft || flatOnBottom), !(flatOnRight || flatOnBottom));

        g.fillPath (path);
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
    }
}

void LookAndFeel::drawButtonText (juce::Graphics& g,
                                  juce::TextButton& button,
                                  bool,
                                  bool isButtonDown)
{
    auto font = getTextButtonFont (button, button.getHeight());
    g.setFont (font);
    g.setColour (button.findColour (isButtonDown ? juce::TextButton::textColourOnId
                                                 : juce::TextButton::textColourOffId)
                     .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    auto yIndent = juce::jmin (4, button.proportionOfHeight (0.5f));
    auto cornerSize = juce::jmin (button.getHeight(), button.getWidth()) / 2;

    auto fontHeight = juce::roundToInt (font.getHeight() * 0.6f);
    auto leftIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    auto rightIndent = juce::jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
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

void LookAndFeel::drawComboBox (juce::Graphics& g,
                                int,
                                int height,
                                bool,
                                int,
                                int,
                                int,
                                int,
                                juce::ComboBox& box)
{
    const auto boxBounds = box.getLocalBounds();

    const auto fontHeight = juce::jmax (7.0f, height * 0.6f);
    g.setFont (juce::Font (fontHeight));

    g.setColour (box.findColour (juce::ComboBox::backgroundColourId));

    const auto h = boxBounds.getHeight();
    const auto cornerSize = h * .15f;
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
    using namespace laf_constants;

    juce::Rectangle<int> r (area);

    juce::Colour fillColour = isHighlighted ? vColour_5 : vColour_4;
    g.setColour (fillColour);
    g.fillRect (r.getX(), r.getY(), r.getWidth(), r.getHeight() - 1);

    juce::Colour myTextColour = isTicked ? vColour_7 : vColour_1;
    g.setColour (myTextColour);

    auto fHeight = juce::jmax (7.0f, r.getHeight() * 0.6f);
    g.setFont (juce::Font (fHeight));

    r.setLeft (10);
    r.setY (1);
    g.drawFittedText (text, r, juce::Justification::left, 1);
}

juce::Slider::SliderLayout LookAndFeel::getSliderLayout (juce::Slider& slider)
{
    // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

    auto textBoxPos = slider.getTextBoxPosition();

    auto localBounds = slider.getLocalBounds();

    auto sDiameter = juce::jmin (localBounds.getWidth(), localBounds.getHeight()) - 4.0f;
    auto textBoxWidth = sDiameter * .66f;
    auto textBoxHeight = sDiameter * .17f;

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
} // namespace tote_bag
