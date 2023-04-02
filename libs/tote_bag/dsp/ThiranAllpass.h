/*
  ==============================================================================

    ThiranAllpass.h
    Created: 14 Oct 2019 11:10:18am
    Author:  dev

  ==============================================================================
*/

#pragma once

template <typename T>
class FirstOrderThiranAllpass
{
public:
    void reset()
    {
        accumulator = 0.0;
    }

    void prepare (T inDelay)
    {
        if (delay != inDelay)
        {
            delay = inDelay;
            makeCoefficients();
        }
    }

    /** Calculates the filter coefficients. This is called automatically by prepare().
     *  see Splitting the Unit Delay: Tools for Fractional Delay Filter Design
     *  T.I. Laakso; V. Valimaki; M. Karjalainen; U.K. Laine; et al.
     */
    void makeCoefficients()
    {
        a1 = (1.0f - delay) / (1.0f + delay);
    }

    /** Take a mono AudioBlock and processes it
     */
    void process (juce::dsp::AudioBlock<float>& inBlock)
    {
        juce::ScopedNoDenormals noDenormals;

        auto blockLen = inBlock.getNumSamples();
        auto inAudio = inBlock.getChannelPointer (0);

        for (size_t sample = 0; sample < blockLen; ++sample)
        {
            const T x = inAudio[sample];

            const T y = a1 * x + accumulator;

            inAudio[sample] = y;
            accumulator = x - (a1 * y);
        }
    }

private:
    T delay = 0.0;
    T a1 = 0.0;
    T accumulator = 0.0;
};
