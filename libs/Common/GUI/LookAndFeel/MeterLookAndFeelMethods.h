/*
  ==============================================================================

    MeterLookAndFeelMethods.h
    Created: 15 Jan 2020 3:49:16pm
    Author:  José Díaz Rohena

  ==============================================================================
*/

#pragma once

void setupDefaultMeterColours () override
{
    setColour (FFAU::LevelMeter::lmTextColour,             juce::Colours::black);
    setColour (FFAU::LevelMeter::lmTextClipColour,         juce::Colours::white);
    setColour (FFAU::LevelMeter::lmTextDeactiveColour,     juce::Colours::darkgrey);
    setColour (FFAU::LevelMeter::lmTicksColour,            juce::Colours::orange);
    setColour (FFAU::LevelMeter::lmOutlineColour,          juce::Colours::orange);
    setColour (FFAU::LevelMeter::lmBackgroundColour,       juce::Colours::lightpink);
    setColour (FFAU::LevelMeter::lmBackgroundClipColour,   juce::Colours::red);
    setColour (FFAU::LevelMeter::lmMeterForegroundColour,  juce::Colours::green);
    setColour (FFAU::LevelMeter::lmMeterOutlineColour,     juce::Colours::lightgrey);
    setColour (FFAU::LevelMeter::lmMeterBackgroundColour,  juce::Colours::darkgrey);
    setColour (FFAU::LevelMeter::lmMeterMaxNormalColour,   juce::Colours::lightgrey);
    setColour (FFAU::LevelMeter::lmMeterMaxWarnColour,     juce::Colours::orange);
    setColour (FFAU::LevelMeter::lmMeterMaxOverColour,     juce::Colours::darkred);
    setColour (FFAU::LevelMeter::lmMeterGradientLowColour, juce::Colours::green);
    setColour (FFAU::LevelMeter::lmMeterGradientMidColour, juce::Colours::lightgoldenrodyellow);
    setColour (FFAU::LevelMeter::lmMeterGradientMaxColour, juce::Colours::red);
    setColour (FFAU::LevelMeter::lmMeterReductionColour,   juce::Colours::orange);
}

void updateMeterGradients () override
{
    horizontalGradient.clearColours();
    verticalGradient.clearColours();
}

juce::Rectangle<float> getMeterInnerBounds (const juce::Rectangle<float> bounds,
                                            const FFAU::LevelMeter::MeterFlags meterType) const override
{
    if (meterType & FFAU::LevelMeter::HasBorder) {
        const float corner = std::min (bounds.getWidth(), bounds.getHeight()) * 0.01f;
        return bounds.reduced (3 + corner);
    }
    return bounds;
}

juce::Rectangle<float> getMeterBounds (const juce::Rectangle<float> bounds,
                                       const FFAU::LevelMeter::MeterFlags meterType,
                                       const int numChannels,
                                       const int channel) const override
{
    if (meterType & FFAU::LevelMeter::SingleChannel) {
        return bounds;
    }
    else {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            const float h = bounds.getHeight() / numChannels;
            return bounds.withHeight (h).withY (bounds.getY() + channel * h);
        }
        else {
            const float w = bounds.getWidth() / numChannels;
            return bounds.withWidth (w).withX (bounds.getX() + channel  );
        }
    }
    return juce::Rectangle<float> ();
}

/** Override this callback to define the placement of the actual meter bar. */
juce::Rectangle<float> getMeterBarBounds (const juce::Rectangle<float> bounds,
                                          const FFAU::LevelMeter::MeterFlags meterType) const override
{
    
    if (meterType & FFAU::LevelMeter::Minimal
        || meterType & FFAU::LevelMeter::Reduction) {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            const float margin = bounds.getHeight() * 0.05f;
            const float h      = bounds.getHeight() - 2.0f * margin;
            const float left   = bounds.getX() + margin;
            const float right  = bounds.getRight() - (4.0f * margin + h);
            return juce::Rectangle<float>(bounds.getX() + margin,
                                          bounds.getY() + margin,
                                          right - left,
                                          h);
        }
        else {
            const float margin = bounds.getHeight() * 0.001f;
            const float top    = bounds.getY() + 2.0f * margin;
            const float bottom = (meterType & FFAU::LevelMeter::MaxNumber) ?
                                  bounds.getBottom() - (3.0f * margin + ((bounds.getHeight() * .1) - margin * 2.0f))
                                  : bounds.getBottom() - margin;
            return juce::Rectangle<float>(bounds.getX() + margin, top,
                                          bounds.getWidth() - margin * 2.0f, bottom - top);
        }
    }
    else if (meterType & FFAU::LevelMeter::Vintage) {
        return bounds;
    }
    else {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            const float margin = bounds.getHeight() * 0.05f;
            const float h      = bounds.getHeight() * 0.5f - 2.0f * margin;
            const float left   = 60.0f + 3.0f * margin;
            const float right  = bounds.getRight() - (4.0f * margin + h * 0.5f);
            return juce::Rectangle<float>(bounds.getX() + left,
                                          bounds.getY() + margin,
                                          right - left,
                                          h);
        }
        else {
            const float margin = bounds.getWidth() * 0.05f;
            const float w      = bounds.getWidth() * 0.45f;
            const float top    = bounds.getY() + 2.0f * margin + w * 0.5f;
            const float bottom = bounds.getBottom() - (2.0f * margin + 25.0f);
            return juce::Rectangle<float>(bounds.getX() + margin, top, w, bottom - top);
        }
    }
    return juce::Rectangle<float> ();
}

/** Override this callback to define the placement of the tickmarks.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> getMeterTickmarksBounds (const juce::Rectangle<float> bounds,
                                                const FFAU::LevelMeter::MeterFlags meterType) const override
{
    return juce::Rectangle<float> ();
}

/** Override this callback to define the placement of the clip indicator light.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> getMeterClipIndicatorBounds (const juce::Rectangle<float> bounds,
                                                    const FFAU::LevelMeter::MeterFlags meterType) const override
{
    return juce::Rectangle<float> ();
}

/** Override this callback to define the placement of the max level.
 To disable this feature return an empty rectangle. */
juce::Rectangle<float> getMeterMaxNumberBounds (const juce::Rectangle<float> bounds,
                                                const FFAU::LevelMeter::MeterFlags meterType) const override
{
    if (meterType & FFAU::LevelMeter::Minimal
        || meterType & FFAU::LevelMeter::Reduction) {
        if (meterType & FFAU::LevelMeter::MaxNumber) {
            if (meterType & FFAU::LevelMeter::Horizontal) {
                const float margin = bounds.getHeight() * 0.05f;
                const float h      = bounds.getHeight() - 2.0f * margin;
                return juce::Rectangle<float>(bounds.getRight() - (margin + h),
                                              bounds.getY() + margin,
                                              h, h);
            }
            else {
                const float margin = bounds.getWidth() * 0.02f;
                const float w      = bounds.getWidth() - margin * 2.0f;
                const float h      = bounds.getHeight() * 0.085f;
                return juce::Rectangle<float>(bounds.getX() + margin,
                                              bounds.getBottom() - (margin + (h * 1.5f)),
                                              w, h);
            }
        }
        else {
            return juce::Rectangle<float> ();
        }
    }
    return juce::Rectangle<float>();
}

juce::Rectangle<float> drawBackground (juce::Graphics& g,
                                       const FFAU::LevelMeter::MeterFlags meterType,
                                       const juce::Rectangle<float> bounds) override
{
    g.setColour (findColour (FFAU::LevelMeter::lmBackgroundColour));
    if (meterType & FFAU::LevelMeter::HasBorder) {
        const float corner = std::min (bounds.getWidth(), bounds.getHeight()) * 0.01f;
        g.fillRoundedRectangle (bounds, corner);
        g.setColour (findColour (FFAU::LevelMeter::lmOutlineColour));
        g.drawRoundedRectangle (bounds.reduced (3), corner, 2);
        return bounds.reduced (3 + corner);
    }
    else {
        g.fillRect (bounds);
        return bounds;
    }
}

void drawMeterBars (juce::Graphics& g,
                    const FFAU::LevelMeter::MeterFlags meterType,
                    const juce::Rectangle<float> bounds,
                    const FFAU::LevelMeterSource* source,
                    const int fixedNumChannels=-1,
                    const int selectedChannel=-1) override
{
    const juce::Rectangle<float> innerBounds = getMeterInnerBounds (bounds, meterType);
    if (source) {
        const int numChannels = source->getNumChannels();
        if (meterType & FFAU::LevelMeter::Minimal) {
            if (meterType & FFAU::LevelMeter::Horizontal) {
                const float height = innerBounds.getHeight() / (2 * numChannels - 1);
                juce::Rectangle<float> meter = innerBounds.withHeight (height);
                for (int channel=0; channel < numChannels; ++channel) {
                    meter.setY (height * channel * 2);
                    {
                        juce::Rectangle<float> meterBarBounds = getMeterBarBounds (meter, meterType);
                        drawMeterBar (g, meterType, meterBarBounds,
                                      source->getRMSLevel (channel),
                                      source->getMaxLevel (channel));
                        const float reduction = source->getReductionLevel (channel);
                        if (reduction < 1.0)
                            drawMeterReduction (g, meterType,
                                                meterBarBounds.withBottom (meterBarBounds.getCentreY()),
                                                reduction);
                    }
                    juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                    if (! clip.isEmpty())
                        drawClipIndicator (g, meterType, clip, source->getClipFlag (channel));
                    juce::Rectangle<float> maxNum = getMeterMaxNumberBounds (meter, meterType);
                    if (! maxNum.isEmpty())
                        drawMaxNumber(g, meterType, maxNum, source->getMaxOverallLevel (channel));
                    if (channel < numChannels-1) {
                        meter.setY (height * (channel * 2 + 1));
                        juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                        if (! ticks.isEmpty())
                            drawTickMarks (g, meterType, ticks);
                    }
                }
            }
            else {
                const float width = innerBounds.getWidth() / numChannels ;
                for (int channel=0; channel < numChannels; ++channel) {
                    juce::Rectangle<float> meter = innerBounds.withWidth(width);
                    meter.setX (width * channel);
                    juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                    if (! clip.isEmpty())
                    {
                        meter.removeFromTop(clip.getHeight());
                        drawClipIndicator (g, meterType, clip, source->getClipFlag (channel));
                    }
                    
                    {
                        juce::Rectangle<float> meterBarBounds = getMeterBarBounds (meter, meterType);
                        drawMeterBar (g, meterType,
                                      meterBarBounds,
                                      source->getRMSLevel (channel),
                                      source->getMaxLevel (channel));
                        const float reduction = source->getReductionLevel (channel);
                        if (reduction < 1.0)
                            drawMeterReduction (g, meterType,
                                                meterBarBounds.withLeft (meterBarBounds.getCentreX()),
                                                reduction);
                    }
                    juce::Rectangle<float> maxNum = getMeterMaxNumberBounds (innerBounds.withWidth (innerBounds.getWidth() / numChannels).withX (innerBounds.getX() + channel * (innerBounds.getWidth() / numChannels)), meterType);
                    if (! maxNum.isEmpty()) {
                        g.setColour(juce::Colours::cornflowerblue);
                        g.fillRect(maxNum);
                        drawMaxNumber(g, meterType, maxNum, source->getMaxLevel(channel));
                    }
                    if (channel < numChannels-1) {
                        meter.setX (width * (channel * 2 + 1));
                        juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                        if (! ticks.isEmpty())
                            drawTickMarks (g, meterType, ticks);
                    }
                }
            }
        }
        else if (meterType & FFAU::LevelMeter::Reduction) {
        
            const float width = innerBounds.getWidth() / numChannels;
                   
            juce::Rectangle<float> meter = innerBounds.withWidth(width);
                
            for (int channel=0; channel < numChannels; ++channel) {
                drawMeterBar (g, meterType,
                              getMeterBarBounds (meter, meterType),
                              source->getReductionLevel(channel),
                              0.0f);
                
                juce::Rectangle<float> maxes = getMeterMaxNumberBounds (bounds, meterType);
                if (! maxes.isEmpty())
                {
                    drawMaxNumber (g, meterType, maxes, source->getReductionLevel (channel));
                }
            }
        }
    }
}

void drawMeterBarsBackground (juce::Graphics& g,
                              const FFAU::LevelMeter::MeterFlags meterType,
                              const juce::Rectangle<float> bounds,
                              const int numChannels,
                              const int fixedNumChannels) override
{
    const juce::Rectangle<float> innerBounds = getMeterInnerBounds (bounds, meterType);
    if (meterType & FFAU::LevelMeter::Minimal) {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            const float height = innerBounds.getHeight() / (2 * numChannels - 1);
            juce::Rectangle<float> meter = innerBounds.withHeight (height);
            for (int channel=0; channel < numChannels; ++channel) {
                meter.setY (height * channel * 2);
                drawMeterBarBackground (g, meterType, getMeterBarBounds (meter, meterType));
                juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                if (! clip.isEmpty())
                    drawClipIndicatorBackground (g, meterType, clip);
                if (channel < numChannels-1) {
                    meter.setY (height * (channel * 2 + 1));
                    juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                    if (! ticks.isEmpty())
                        drawTickMarks (g, meterType, ticks);
                }
            }
        }
        else {
            for (int channel=0; channel < numChannels; ++channel) {
                const float width = innerBounds.getWidth() / numChannels;
                juce::Rectangle<float> meter = innerBounds.withWidth(width);
                meter.setX (width * channel);
                
                const juce::Rectangle<float> clip = getMeterClipIndicatorBounds (meter, meterType);
                
                if (! clip.isEmpty())
                {
                    drawClipIndicatorBackground (g, meterType, clip);
                    meter.removeFromTop(clip.getHeight());
                }
                
                drawMeterBarBackground (g, meterType, getMeterBarBounds (meter, meterType));
                
                if (channel < numChannels-1) {
                    meter.setX (width * (channel * 2 + 1));
                    juce::Rectangle<float> ticks = getMeterTickmarksBounds (meter, meterType);
                    if (! ticks.isEmpty())
                        drawTickMarks (g, meterType, ticks);
                }
            }
        }
    }
    else if (meterType & FFAU::LevelMeter::SingleChannel) {
        drawMeterChannelBackground (g, meterType, innerBounds);
    }
    else if (meterType & FFAU::LevelMeter::Reduction) {
    
    const float width = innerBounds.getWidth() / numChannels;
           
    juce::Rectangle<float> meter = innerBounds.withWidth(width);
    meter.removeFromTop (getMeterClipIndicatorBounds (meter, FFAU::LevelMeter::MeterFlags::Minimal).getHeight());
        
    drawMeterBarBackground (g, meterType, getMeterBarBounds (meter, meterType));
        
    }
    else {
        for (int channel=0; channel < numChannels; ++channel) {
            drawMeterChannelBackground (g, meterType,
                                        getMeterBounds (innerBounds, meterType,
                                                        fixedNumChannels < 0 ? numChannels : fixedNumChannels,
                                                        channel));
        }
    }
}


void drawMeterChannel (juce::Graphics& g,
                       const FFAU::LevelMeter::MeterFlags meterType,
                       const juce::Rectangle<float> bounds,
                       const FFAU::LevelMeterSource* source,
                       const int selectedChannel) override
{
}

void drawMeterChannelBackground (juce::Graphics& g,
                                 const FFAU::LevelMeter::MeterFlags meterType,
                                 const juce::Rectangle<float> bounds) override
{
    juce::Rectangle<float> meter = getMeterBarBounds (bounds, meterType);
    if (! meter.isEmpty()) {
        drawMeterBarBackground (g, meterType, meter);
    }
    juce::Rectangle<float> clip = getMeterClipIndicatorBounds (bounds, meterType);
    if (! clip.isEmpty())
        drawClipIndicatorBackground (g, meterType, clip);
    juce::Rectangle<float> ticks = getMeterTickmarksBounds (bounds, meterType);
    if (! ticks.isEmpty())
        drawTickMarks (g, meterType, ticks);
    juce::Rectangle<float> maxes = getMeterMaxNumberBounds (bounds, meterType);
    if (! maxes.isEmpty()) {
        drawMaxNumberBackground (g, meterType, maxes);
    }
}

void drawMeterBar (juce::Graphics& g,
                   const FFAU::LevelMeter::MeterFlags meterType,
                   const juce::Rectangle<float> bounds,
                   const float rms, const float peak) override
{
    const float infinity = meterType & FFAU::LevelMeter::Reduction ? -30.0f :  -80.0f;
    const float rmsDb  = juce::Decibels::gainToDecibels (rms,  infinity);
    const float peakDb = juce::Decibels::gainToDecibels (peak, infinity);

    const juce::Rectangle<float> floored (ceilf (bounds.getX()) + 1.0f, ceilf (bounds.getY()) + 1.0f,
                                          floorf (bounds.getRight()) - (ceilf (bounds.getX() + 2.0f)),
                                          floorf (bounds.getBottom()) - (ceilf (bounds.getY()) + 2.0f));

    if (meterType & FFAU::LevelMeter::Vintage) {
        // Vintage has not been implemented yet!!!
        jassertfalse;
    }
    else if (meterType & FFAU::LevelMeter::Reduction) {
        const float limitDb = juce::Decibels::gainToDecibels (rms, infinity);
        g.setColour (findColour (FFAU::LevelMeter::lmMeterReductionColour));
        if (meterType & FFAU::LevelMeter::Horizontal) {
            g.fillRect (floored.withLeft (floored.getX() + limitDb * floored.getWidth() / infinity));
        }
        else {
            g.fillRect (floored.withBottom (floored.getY() + limitDb * floored.getHeight() / infinity));
        }
    }
    else {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            if (horizontalGradient.getNumColours() < 2) {
                horizontalGradient = juce::ColourGradient (findColour (FFAU::LevelMeter::lmMeterGradientLowColour),
                                                           floored.getX(), floored.getY(),
                                                           findColour (FFAU::LevelMeter::lmMeterGradientMaxColour),
                                                           floored.getRight(), floored.getY(), false);
                horizontalGradient.addColour (0.5, findColour (FFAU::LevelMeter::lmMeterGradientLowColour));
                horizontalGradient.addColour (0.75, findColour (FFAU::LevelMeter::lmMeterGradientMidColour));
            }
            g.setGradientFill (horizontalGradient);
            g.fillRect (floored.withRight (floored.getRight() - rmsDb * floored.getWidth() / infinity));

            if (peakDb > -49.0) {
                g.setColour (findColour ((peakDb > -0.3f) ? FFAU::LevelMeter::lmMeterMaxOverColour :
                                         ((peakDb > -5.0) ? FFAU::LevelMeter::lmMeterMaxWarnColour :
                                          FFAU::LevelMeter::lmMeterMaxNormalColour)));
                g.drawVerticalLine (juce::roundToInt(floored.getRight() - juce::jmax (peakDb * floored.getWidth() / infinity, 0.0f)),
                                    floored.getY(),
                                    floored.getBottom());
            }
        }
        else {
            // vertical
            if (verticalGradient.getNumColours() < 2) {
                verticalGradient = juce::ColourGradient (findColour (FFAU::LevelMeter::lmMeterGradientLowColour),
                                                         floored.getX(), floored.getBottom(),
                                                         findColour (FFAU::LevelMeter::lmMeterGradientMaxColour),
                                                         floored.getX(), floored.getY(), false);
                verticalGradient.addColour (0.5, findColour (FFAU::LevelMeter::lmMeterGradientLowColour));
                verticalGradient.addColour (0.75, findColour (FFAU::LevelMeter::lmMeterGradientMidColour));
            }
            g.setGradientFill (verticalGradient);
            g.fillRect (floored.withTop (floored.getY() + rmsDb * floored.getHeight() / infinity));

            if (peakDb > -49.0) {
                g.setColour (findColour ((peakDb > -0.3f) ? FFAU::LevelMeter::lmMeterMaxOverColour :
                                         ((peakDb > -5.0) ? FFAU::LevelMeter::lmMeterMaxWarnColour :
                                          FFAU::LevelMeter::lmMeterMaxNormalColour)));
                g.drawHorizontalLine (juce::roundToInt(floored.getY() + juce::jmax (peakDb * floored.getHeight() / infinity, 0.0f)),
                                      floored.getX(),
                                      floored.getRight());
            }
        }
    }
}

void drawMeterReduction (juce::Graphics& g,
                         const FFAU::LevelMeter::MeterFlags meterType,
                         const juce::Rectangle<float> bounds,
                         const float reduction) override
{
    const float infinity = -30.0f;
    
    const juce::Rectangle<float> floored (ceilf (bounds.getX()) + 1.0f, ceilf (bounds.getY()) + 1.0f,
                                          floorf (bounds.getRight()) - (ceilf (bounds.getX() + 2.0f)),
                                          floorf (bounds.getBottom()) - (ceilf (bounds.getY()) + 2.0f));

    const float limitDb = juce::Decibels::gainToDecibels (reduction, infinity);
    g.setColour (findColour (FFAU::LevelMeter::lmMeterReductionColour));
    if (meterType & FFAU::LevelMeter::Horizontal) {
        g.fillRect (floored.withLeft (floored.getX() + limitDb * floored.getWidth() / infinity));
    }
    else {
        g.fillRect (floored.withBottom (floored.getY() + limitDb * floored.getHeight() / infinity));
    }
}

void drawMeterBarBackground (juce::Graphics& g,
                             [[maybe_unused]] const FFAU::LevelMeter::MeterFlags meterType,
                             const juce::Rectangle<float> bounds) override
{
    g.setColour (findColour (FFAU::LevelMeter::lmMeterBackgroundColour));
    g.fillRect  (bounds);
}

void drawTickMarks (juce::Graphics& g,
                    const FFAU::LevelMeter::MeterFlags meterType,
                    const juce::Rectangle<float> bounds) override
{
    const float infinity = meterType & FFAU::LevelMeter::Reduction ? -30.0f :  -80.0f;

    g.setColour (findColour (FFAU::LevelMeter::lmTicksColour));
    if (meterType & FFAU::LevelMeter::Minimal) {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            for (int i=0; i<11; ++i)
                g.drawVerticalLine (juce::roundToInt(bounds.getX() + i * 0.1f * bounds.getWidth()),
                                    bounds.getY() + 4,
                                    bounds.getBottom() - 4);
        }
        else {
            const float h = (bounds.getHeight() - 2.0f) * 0.1f;
            for (int i=0; i<11; ++i) {
                g.drawHorizontalLine (juce::roundToInt(bounds.getY() + i * h + 1),
                                      bounds.getX() + 4,
                                      bounds.getRight());
            }
            if (h > 10 && bounds.getWidth() > 20) {
                // don't print tiny numbers
                g.setFont (h * 0.5f);
                for (int i=0; i<10; ++i) {
                    g.drawFittedText (juce::String (i * 0.1 * infinity),
                                      juce::roundToInt(bounds.getX()),
                                      juce::roundToInt(bounds.getY() + (float) i * h + 2.0f),
                                      juce::roundToInt(bounds.getWidth()),
                                      juce::roundToInt((float) h * 0.6f),
                                      juce::Justification::centredTop, 1);
                }
            }
        }
    }
    else if (meterType & FFAU::LevelMeter::Vintage) {
        // TODO
    }
    else {
        if (meterType & FFAU::LevelMeter::Horizontal) {
            for (int i=0; i<11; ++i)
                g.drawVerticalLine (juce::roundToInt(bounds.getX() + i * 0.1f * bounds.getWidth()),
                                    bounds.getY() + 4,
                                    bounds.getBottom() - 4);
        }
        else {
            const float h = (bounds.getHeight() - 2.0f) * 0.05f;
            g.setFont (h * 0.8f);
            for (int i=0; i<21; ++i) {
                const float y = bounds.getY() + i * h;
                if (i % 2 == 0) {
                    g.drawHorizontalLine (juce::roundToInt(y + 1),
                                          bounds.getX() + 4,
                                          bounds.getRight());
                    if (i < 20) {
                        g.drawFittedText (juce::String (i * 0.05f * infinity),
                                          juce::roundToInt(bounds.getX()),
                                          juce::roundToInt(y + 4),
                                          juce::roundToInt(bounds.getWidth()),
                                          juce::roundToInt(h * 0.6f),
                                          juce::Justification::topRight, 1);
                    }
                }
                else {
                  g.drawHorizontalLine (juce::roundToInt(y + 2),
                                        bounds.getX() + 4,
                                        bounds.getCentreX());
                }
            }
        }
    }
}

void drawClipIndicator (juce::Graphics& g,
                        [[maybe_unused]] const FFAU::LevelMeter::MeterFlags meterType,
                        const juce::Rectangle<float> bounds,
                        const bool hasClipped) override
{
    g.setColour (findColour (hasClipped ? FFAU::LevelMeter::lmBackgroundClipColour : FFAU::LevelMeter::lmMeterBackgroundColour));
    g.fillRect (bounds);
}

void drawClipIndicatorBackground (juce::Graphics& g,
                                  [[maybe_unused]] const FFAU::LevelMeter::MeterFlags meterType,
                                  const juce::Rectangle<float> bounds) override
{
    g.setColour (findColour (FFAU::LevelMeter::lmMeterBackgroundColour));
    g.fillRect (bounds);
    g.setColour (findColour (FFAU::LevelMeter::lmMeterOutlineColour));
    g.drawRect (bounds, 1.0);
}

void drawMaxNumber (juce::Graphics& g,
                    [[maybe_unused]] const FFAU::LevelMeter::MeterFlags meterType,
                    const juce::Rectangle<float> bounds,
                    const float maxGain) override
{

    const float maxDb = juce::Decibels::gainToDecibels (maxGain, -80.0f);
    g.setColour (findColour (maxDb > 0.0 ? FFAU::LevelMeter::lmTextClipColour : FFAU::LevelMeter::lmTextColour));
    g.setFont (bounds.getHeight() * 0.5f);
    
    g.drawFittedText (juce::String (maxDb, 1),
                      bounds.toNearestInt(),
                      juce::Justification::centred, 1);
}

void drawMaxNumberBackground (juce::Graphics& g,
                              [[maybe_unused]] const FFAU::LevelMeter::MeterFlags meterType,
                              const juce::Rectangle<float> bounds) override
{
}

int hitTestClipIndicator (const juce::Point<int> position,
                          const FFAU::LevelMeter::MeterFlags meterType,
                          const juce::Rectangle<float> bounds,
                          const FFAU::LevelMeterSource* source) const override
{
    if (source) {
        const int numChannels = source->getNumChannels();
        for (int i=0; i < numChannels; ++i) {
            if (getMeterClipIndicatorBounds (getMeterBounds
                                             (bounds, meterType, source->getNumChannels(), i), meterType)
                .contains (position.toFloat())) {
                return i;
            }
        }
    }
    return -1;
}

int hitTestMaxNumber (const juce::Point<int> position,
                      const FFAU::LevelMeter::MeterFlags meterType,
                      const juce::Rectangle<float> bounds,
                      const FFAU::LevelMeterSource* source) const override
{
    if (source) {
        const int numChannels = source->getNumChannels();
        for (int i=0; i < numChannels; ++i) {
            if (getMeterMaxNumberBounds (getMeterBounds
                                         (bounds, meterType, source->getNumChannels(), i), meterType)
                .contains (position.toFloat())) {
                return i;
            }
        }
    }
    return -1;
}

private:

juce::ColourGradient horizontalGradient;
juce::ColourGradient verticalGradient;


