/*
  ==============================================================================

    ThiranAllpass.h
    Created: 14 Oct 2019 11:10:18am
    Author:  dev

  ==============================================================================
*/

#pragma once

#include "tote_bag/dsp/AudioHelpers.h"
#include "tote_bag/dsp/CircularBuffer.h"

#include <juce_dsp/juce_dsp.h>

#include <array>

struct ThiranScalars
{
    /*
       precalculated for a Nth order filters,
       will calculate exact delay in prepare()
       [0] is always 1. Keeping here for easier indexing
     
       -1^k * (binomial coefficient of N k)
     
       N       N!
         =  ----------
       k    k!(N - k)!
     
       Should compute a lookup table to avoid calculating factorials
       for a more flexible implementation
    
    */

    const std::array<int, 2> firstOrder = { 1, -1 };
    const std::array<int, 5> fourthOrder = { 1, -4, 6, -4, 1 };
    const std::array<int, 6> sixthOrder = { 1, -5, 10, -10, 5, -1 };
};

/** An allpass filter desined for fractional delay. Currently supports delay amounts
    < 1 sample, but theoretically could handle more (as shown by the above coefficient arrays).
 */
template <typename T>
class ThiranAllPass
{
public:
    void reset()
    {
        xBuf.reset();
        yBuf.reset();
    }

    void prepare (T inDelay)
    {
        // get the desired delay and figure out filter order.
        if (delay != inDelay)
        {
            delay = inDelay;
            N = 1;
            bufSize = N;

            xBuf.setSize (bufSize);
            yBuf.setSize (bufSize);

            makeCoefficients();
        }
    }

    void makeCoefficients()
    {
        // from https://ccrma.stanford.edu/~jos/pasp/Thiran_Allpass_Interpolation_Matlab.html

        if (aCoeffs.size() != bufSize)
            aCoeffs.resize (bufSize);
        if (bCoeffs.size() != bufSize)
            bCoeffs.resize (bufSize);

        //an attempt at ameliorating the level boost
        aCoeffs[0] = 0.7943282347242815;

        for (int i = 1; i < N + 1; i++)
        {
            auto product = 1.0;

            for (int n = 0; n < N + 1; n++)
            {
                product = product * (delay - N + n) / (delay - N + i + n);

                aCoeffs[i] = std::pow (-1, i) * scalars.firstOrder[i] * product;
            }
        }

        int k = N;
        for (auto coeff : aCoeffs)
            bCoeffs[k--] = coeff;
    }

    /** Take a mono AudioBlock and processes it
        Direct Form I since the order / delay is integral to the filtering
     */
    void process (juce::dsp::AudioBlock<float>& inBlock)
    {
        juce::ScopedNoDenormals noDenormals;

        auto blockLen = inBlock.getNumSamples();
        auto inAudio = inBlock.getChannelPointer (0);

        for (int sample = 0; sample < blockLen; ++sample)
        {
            auto v = bCoeffs[0] * inAudio[sample];

            for (int i = 1; i < bCoeffs.size(); ++i)
                v += bCoeffs[i] * xBuf.readBuffer (i, false);

            xBuf.writeBuffer (inAudio[sample]);

            for (int i = 1; i < aCoeffs.size(); ++i)
                v -= aCoeffs[i] * yBuf.readBuffer (i, false);

            inAudio[sample] = v * aCoeffs[0];

            yBuf.writeBuffer (inAudio[sample]);
        }
    }

private:
    std::vector<T> aCoeffs, bCoeffs;
    CircularBuffer<T> xBuf, yBuf;
    T delay;
    int N, bufSize;
    ThiranScalars scalars;
};
