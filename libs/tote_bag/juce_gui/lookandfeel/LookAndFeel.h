/*
  ==============================================================================

    LookAndFeel.h
    Created: 26 Dec 2019 7:02:26pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/utilities/tbl_font.h"

#include <ff_meters/ff_meters.h>

namespace tote_bag
{

class FlatTextButton;

class LookAndFeel : public juce::LookAndFeel_V4,
                    public FFAU::LevelMeter::LookAndFeelMethods
{
public:
    LookAndFeel();

    void drawSliderMeter (juce::Graphics& g,
                          const juce::Rectangle<float> bounds,
                          float lineWidth,
                          float arcRadius,
                          float rotaryStartAngle,
                          float rotaryEndAngle,
                          float toAngle,
                          juce::Slider& slider);

    /** Draws a drawable knob. Originally used for the knobs in Valentine, this is been refactored
        to not rely on member variables. This, along with the fact that this function isn't called
        anywhere, effectively orphans it. This is intentional: the way that the drawable was defined
        and used didn't make sense for a look and feel class that is supposed to be reuseable.

        TODO: Refactor slider drawing to effectively use this function if desired
     */
    void drawDrawableKnob (juce::Graphics& g,
                           const float radius,
                           const float drawableWidth,
                           juce::Drawable& sliderImage,
                           const float toAngle,
                           const juce::Rectangle<float> bounds);

    void drawKnob (juce::Graphics& g,
                   const float radius,
                   const float toAngle,
                   const juce::Rectangle<float> bounds,
                   juce::Slider&,
                   const bool withDropShadow);

    void drawRotarySlider (juce::Graphics& g,
                           int x,
                           int y,
                           int width,
                           int height,
                           float sliderPos,
                           const float rotaryStartAngle,
                           const float rotaryEndAngle,
                           juce::Slider&) override;

    juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override;

    juce::Font getLabelFont (juce::Label& l) override;

    void drawButtonBackground (juce::Graphics& g,
                               juce::Button& button,
                               const juce::Colour& backgroundColour,
                               bool,
                               bool) override;

    void drawFlatButtonBackground (juce::Graphics& g,
                                   tote_bag::FlatTextButton& button,
                                   const juce::Colour& backgroundColour,
                                   bool,
                                   bool isButtonDown);

    void drawButtonText (juce::Graphics& g,
                         juce::TextButton& button,
                         bool,
                         bool isButtonDown) override;

    void drawComboBox (juce::Graphics& g,
                       int,
                       int height,
                       bool,
                       int,
                       int,
                       int,
                       int,
                       juce::ComboBox& box) override;

    void drawPopupMenuItem (juce::Graphics& g,
                            const juce::Rectangle<int>& area,
                            bool,
                            bool,
                            bool isHighlighted,
                            bool isTicked,
                            bool,
                            const juce::String& text,
                            const juce::String&,
                            const juce::Drawable*,
                            const juce::Colour*) override;

    juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider) override;

    enum ColourIds { knobColourId = 0x1001800, pointerColourId = 0x1001801 };

private:
    FontHolder fontHolder;

#include "MeterLookAndFeelMethods.h"
};
} // namespace tote_bag
