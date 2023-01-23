/*
  ==============================================================================

    LookAndFeel.h
    Created: 26 Dec 2019 7:02:26pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

namespace tote_bag
{
class FlatTextButton;

class LookAndFeel : public LookAndFeel_V4,
                     public FFAU::LevelMeter::LookAndFeelMethods
{
    
public:
    LookAndFeel();

    void drawSliderMeter(Graphics& g,
                         const Rectangle<float> bounds,
                         float lineWidth,
                         float arcRadius,
                         float rotaryStartAngle, float rotaryEndAngle,
                         float toAngle,
                         Slider& slider);
    
    /** Draws a drawable knob. Originally used for the knobs in Valentine, this is been refactored
        to not rely on member variables. This, along with the fact that this function isn't called
        anywhere, effectively orphans it. This is intentional: the way that the drawable was defined
        and used didn't make sense for a look and feel class that is supposed to be reuseable.

        TODO: Refactor slider drawing to effectively use this function if desired
     */
    void drawDrawableKnob(Graphics& g,
                          const float radius,
                          const float drawableWidth,
                          Drawable& sliderImage,
                          const float toAngle,
                          const Rectangle<float> bounds);

    void drawKnob(Graphics& g,
                  const float radius,
                  const float toAngle,
                  const Rectangle<float> bounds,
                  Slider& slider,
                  const bool withDropShadow);

    void drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, Slider&) override;

    Typeface::Ptr getTypefaceForFont (const Font& f) override;

    Font getTextButtonFont (TextButton&, int buttonHeight) override;
    Font getLabelFont (Label& l) override;

    void drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
                          bool, bool isButtonDown) override;
    void drawFlatButtonBackground (Graphics& g, tote_bag::FlatTextButton& button, const Colour& backgroundColour,
                          bool, bool isButtonDown);
    void drawButtonText (Graphics& g, TextButton& button, bool, bool isButtonDown) override;
    
    void drawComboBox (Graphics& g, int width, int height, bool, int, int, int, int, ComboBox& box) override;
    
    void drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                  bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                                  const String& text, const String& shortcutKeyText,
                            const Drawable* icon, const Colour* textColour) override;
    
    
    
    Slider::SliderLayout getSliderLayout (Slider& slider) override;
    
    enum ColourIds
    {
      knobColourId = 0x1001800,
      pointerColourId = 0x1001801
    };

#include "MeterLookAndFeelMethods.h"
};
} // namespace tote_bag
