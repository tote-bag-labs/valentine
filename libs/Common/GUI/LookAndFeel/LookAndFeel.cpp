/*
  ==============================================================================

    LookAndFeel.cpp
    Created: 26 Dec 2019 7:02:26pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#include "LookAndFeel.h"
#include "LookAndFeelConstants.h"
#include "Common/GUI/Components/Widgets/FlatTextButton.h"


namespace
{
const Font getCustomFont()
{
    static auto typeface = Typeface::createSystemTypefaceFor (BinaryData::MontserratMedium_ttf,
                                                              BinaryData::MontserratMedium_ttfSize);
    return Font (typeface);
}
}

namespace tote_bag
{
LookAndFeel::LookAndFeel()
{
    using namespace tote_bag::laf_constants;

    setupDefaultMeterColours();

    setDefaultLookAndFeel(this);

    // slider colours
    setColour(ColourIds::knobColourId, juce::Colours::grey);
    setColour(ColourIds::pointerColourId, juce::Colours::black);

    // slider text box colours
    setColour(juce::Slider::textBoxTextColourId, Colours::black);
    setColour(juce::Slider::textBoxOutlineColourId, vPinkDark);
    setColour(juce::Slider::rotarySliderOutlineColourId, vPinkDark);
    setColour(juce::Slider::rotarySliderFillColourId, Colours::floralwhite);
}

void LookAndFeel::drawSliderMeter(Graphics& g,
                                   const Rectangle<float> bounds,
                                   const float lineWidth,
                                   const float radius,
                                   const float startAngle, float endAngle,
                                   const float toAngle,
                                   Slider& slider)
{
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 radius,
                                 radius,
                                 0.0f,
                                 startAngle,
                                 endAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineWidth, PathStrokeType::curved, PathStrokeType::butt));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                radius,
                                radius,
                                0.0f,
                                startAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineWidth, PathStrokeType::curved, PathStrokeType::butt));
    }
}

void LookAndFeel::drawDrawableKnob(Graphics& g,
                                   const float radius,
                                   const float drawableWidth,
                                   Drawable& sliderImage,
                                   const float toAngle,
                                   const Rectangle<float> bounds)
{
    const auto rw = radius * 2.0f;
    const auto realW = rw / drawableWidth;

    const auto halfDrawableWidth = drawableWidth / 2.0f;

    sliderImage.setTransform(AffineTransform::rotation(toAngle,
                                                  halfDrawableWidth,
                                                  halfDrawableWidth).scaled(realW,
                                                                            realW,
                                                                            halfDrawableWidth,
                                                                            halfDrawableWidth));

    const auto cX = bounds.getCentreX() - halfDrawableWidth;
    const auto cY = bounds.getCentreY() - halfDrawableWidth;
    sliderImage.drawAt(g, cX, cY, 1.0f);
}

void LookAndFeel::drawKnob(Graphics& g,
                            const float radius,
                            const float toAngle,
                            const Rectangle<float> bounds,
                            Slider& slider,
                            const bool withDropShadow)
{
    const auto centreX = bounds.getCentreX();
    const auto centreY = bounds.getCentreY();

    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;

    // Fill main knob area
    auto fillColour = findColour(ColourIds::knobColourId);
    g.setColour (fillColour);
    g.fillEllipse (rx, ry, rw, rw);

    // Get thicknesses for outline rings...
    const auto innerOutlineThickness = jmax((rw * .05f), 1.0f);
    const auto outerOutlineThickness = innerOutlineThickness * .15f;

    // Offset inner outline by its thickness
    auto innerOutlineRadius = radius - (innerOutlineThickness * .5f);
    auto innerOutlineRx = centreX - innerOutlineRadius;
    auto innerOutlineRy = centreY - innerOutlineRadius;
    auto innerOutlineRw = innerOutlineRadius * 2.0f;

    // Draw inner outline
    g.setColour (fillColour.darker(.15f));
    g.drawEllipse (innerOutlineRx, innerOutlineRy, innerOutlineRw, innerOutlineRw, innerOutlineThickness);

    // Offset outer outline by its thickness
    auto outerOutlineRadius = radius - (outerOutlineThickness * .5f);
    auto outerOutlineRx = centreX - outerOutlineRadius;
    auto outerOutlineRy = centreY - outerOutlineRadius;
    auto outerOutlineRw = outerOutlineRadius * 2.0f;

    // Draw outer outline
    if (withDropShadow)
    {

        auto xOffset = jmin(roundToInt(innerOutlineThickness * .25), 1);
        auto yOffset = jmin(roundToInt(innerOutlineThickness * .5), 1);

        auto shadow = DropShadow(fillColour.darker(),
                                 innerOutlineThickness,
                                 {xOffset, yOffset});
        Path shadowPath;
        shadowPath.addEllipse(outerOutlineRx, outerOutlineRy, outerOutlineRw, outerOutlineRw);
        shadow.drawForPath(g, shadowPath);
    }
    else
    {
        g.setColour (fillColour.darker(.85f));
        g.drawEllipse (outerOutlineRx, outerOutlineRy, outerOutlineRw, outerOutlineRw, outerOutlineThickness);
    }

    // Pointer
    juce::Path p;
    auto pointerLength = radius * 0.33f;
    auto pointerThickness = pointerLength * .2f;
    auto cornerSize = pointerThickness * .35f;
    p.addRoundedRectangle(-pointerThickness * 0.5f, -radius + innerOutlineThickness,
                          pointerThickness, pointerLength,
                          cornerSize, cornerSize,
                          true, true,
                          false, false);
    p.applyTransform (juce::AffineTransform::rotation (toAngle).translated (bounds.getCentreX(), bounds.getCentreY()));

    auto pointerColour = findColour(ColourIds::pointerColourId);
    g.setColour (pointerColour);
    g.fillPath (p);
}

void LookAndFeel::drawRotarySlider (Graphics& g,
                                     int x, int y,
                                     int width, int height,
                                     float sliderPos,
                                     const float rotaryStartAngle, const float rotaryEndAngle,
                                     Slider& slider)
{

    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);
    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto lineW = radius * 0.125f;
    auto arcRadius = radius - lineW * 0.5f;
    const auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    drawSliderMeter(g, bounds, lineW, arcRadius, rotaryStartAngle, rotaryEndAngle, toAngle, slider);

    const auto knobRadius = arcRadius * .80f;

    drawKnob(g, knobRadius, toAngle, bounds, slider, true);
}
        
//=============================================================================

Typeface::Ptr LookAndFeel::getTypefaceForFont (const Font& f)
{
    return getCustomFont().getTypefacePtr();
}

Font LookAndFeel::getTextButtonFont (TextButton& b, int buttonHeight)
{
    return  {jmax (7.0f, buttonHeight * 0.8f)};
}
    
Font LookAndFeel::getLabelFont (Label& l)
{
    return { static_cast<float>(l.getHeight()) };
}
    
    void LookAndFeel::drawButtonBackground (Graphics& g,
                                             Button& button,
                                             const Colour& backgroundColour,
                                             bool,
                                             bool isButtonDown)
    {
        auto buttonArea = button.getLocalBounds();
        const auto h = buttonArea.getHeight();

        const auto cornerSize = h * .15;

        g.setColour(backgroundColour);

        g.fillRoundedRectangle(buttonArea.toFloat(), cornerSize);
    }

void LookAndFeel::drawFlatButtonBackground (Graphics& g,
                                             tote_bag::FlatTextButton& button,
                                             const Colour& backgroundColour,
                                             bool shouldDrawButtonAsHighlighted,
                                             bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();

    auto r = jmin(bounds.getWidth(), bounds.getHeight());
    auto cornerSize = r * .5f;

    auto baseColour = backgroundColour.withMultipliedSaturation (button.hasKeyboardFocus (true) ? 1.3f : 0.9f)
                                      .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour (baseColour);

    auto flatOnLeft   = button.isConnectedOnLeft();
    auto flatOnRight  = button.isConnectedOnRight();
    auto flatOnTop    = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! (flatOnLeft  || flatOnTop),
                                  ! (flatOnRight || flatOnTop),
                                  ! (flatOnLeft  || flatOnBottom),
                                  ! (flatOnRight || flatOnBottom));

        g.fillPath (path);
    }
    else
    {
        g.fillRoundedRectangle (bounds, cornerSize);
    }
}

    void LookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool, bool isButtonDown)
    {
        auto font = getTextButtonFont (button, button.getHeight());
        g.setFont (font);
        g.setColour (button.findColour (isButtonDown ? TextButton::textColourOnId
                                                                : TextButton::textColourOffId)
                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

        auto yIndent = jmin (4, button.proportionOfHeight (0.5f));
        auto cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;

        auto fontHeight = roundToInt (font.getHeight() * 0.6f);
        auto leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft()  ? 4 : 2));
        auto rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
        auto textWidth = button.getWidth() - leftIndent - rightIndent;

        //auto edge = 4;
        //auto offset = isButtonDown ? edge / 2 : 0;

        if (textWidth > 0)
            g.drawFittedText (button.getButtonText(),
                              leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                              Justification::centred, 2);
    }

    void LookAndFeel::drawComboBox (Graphics& g, int width, int height, bool,
                                       int, int, int, int, ComboBox& box)
    {
        const auto boxBounds = box.getLocalBounds();

        const auto fontHeight = jmax (7.0f, height * 0.6f);
        g.setFont(Font(fontHeight));

        g.setColour (box.findColour (ComboBox::backgroundColourId));

        const auto h = boxBounds.getHeight();
        const auto cornerSize = h * .15f;
        g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);
    }

    void LookAndFeel::drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                              bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                              const String& text, const String& shortcutKeyText,
                              const Drawable* icon, const Colour* textColour)
      {
          using namespace laf_constants;

          juce::Rectangle<int> r (area);

          Colour fillColour = isHighlighted ? vColour_5 : vColour_4;
          g.setColour(fillColour);
          g.fillRect(r.getX(), r.getY(), r.getWidth(), r.getHeight() - 1 );

          Colour myTextColour = isTicked ? vColour_7 : vColour_1;
          g.setColour(myTextColour);

          auto fHeight = jmax (7.0f, r.getHeight() * 0.6f);
          g.setFont(Font(fHeight));

          r.setLeft(10);
          r.setY(1);
          g.drawFittedText(text, r, Justification::left, 1);

      }
    
    
    
    Slider::SliderLayout LookAndFeel::getSliderLayout (Slider& slider)
    {
        // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints

        int minXSpace = 0;
        int minYSpace = 0;

        auto textBoxPos = slider.getTextBoxPosition();

        if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
            minXSpace = 30;
        else
            minYSpace = 15;

        auto localBounds = slider.getLocalBounds();

        
        auto sDiameter = jmin(localBounds.getWidth(), localBounds.getHeight()) - 4.0f;
        auto textBoxWidth  = sDiameter * .66f;
        auto textBoxHeight = sDiameter * .17f;
      
        Slider::SliderLayout layout;

        // 2. set the textBox bounds

        if (textBoxPos != Slider::NoTextBox)
        {
            if (slider.isBar())
            {
                layout.textBoxBounds = localBounds;
            }
            else
            {
                layout.textBoxBounds.setWidth (textBoxWidth);
                layout.textBoxBounds.setHeight (textBoxHeight);

                if (textBoxPos == Slider::TextBoxLeft)           layout.textBoxBounds.setX (0);
                else if (textBoxPos == Slider::TextBoxRight)     layout.textBoxBounds.setX (localBounds.getWidth() - textBoxWidth);
                else /* above or below -> centre horizontally */ layout.textBoxBounds.setX ((localBounds.getWidth() - textBoxWidth) / 2);

                if (textBoxPos == Slider::TextBoxAbove)          layout.textBoxBounds.setY (0);
                else if (textBoxPos == Slider::TextBoxBelow)     layout.textBoxBounds.setY (localBounds.getHeight() - textBoxHeight);
                else /* left or right -> centre vertically */    layout.textBoxBounds.setY ((localBounds.getHeight() - textBoxHeight) / 2);
            }
        }

        // 3. set the slider bounds

        layout.sliderBounds = localBounds;

        if (slider.isBar())
        {
            layout.sliderBounds.reduce (1, 1);   // bar border
        }
        else
        {
            if (textBoxPos == Slider::TextBoxLeft)       layout.sliderBounds.removeFromLeft (textBoxWidth);
            else if (textBoxPos == Slider::TextBoxRight) layout.sliderBounds.removeFromRight (textBoxWidth);
            else if (textBoxPos == Slider::TextBoxAbove) layout.sliderBounds.removeFromTop (textBoxHeight);
            else if (textBoxPos == Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom (textBoxHeight);

            const int thumbIndent = getSliderThumbRadius (slider);

            if (slider.isHorizontal())    layout.sliderBounds.reduce (thumbIndent, 0);
            else if (slider.isVertical()) layout.sliderBounds.reduce (0, thumbIndent);
        }

        return layout;
    }
} // namespace tote_bag

    
    
    
    

