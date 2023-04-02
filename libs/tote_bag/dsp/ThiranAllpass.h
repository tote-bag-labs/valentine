/*
  ==============================================================================

    ThiranAllpass.h
    Created: 14 Oct 2019 11:10:18am
    Author:  dev

  ==============================================================================
*/

#pragma once

#include <cassert>

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
        assert (inDelay > 0.0);

        if (delay != inDelay)
        {
            delay = inDelay;
            updateCoefficients();
        }
    }

    /** Processes a buffer of samples. This should be called after prepare().
     *  @param buffer The buffer to process.
     *  @param numSamples The number of samples in the buffer.
     */
    void process (T* buffer, size_t numSamples)
    {
        for (size_t sample = 0; sample < numSamples; ++sample)
        {
            const auto x = buffer[sample];
            const auto y = a1 * x + accumulator;

            buffer[sample] = y;
            accumulator = x - (a1 * y);
        }
    }

private:
    /** Calculates the filter coefficients. This is called automatically by prepare().
     * see Splitting the Unit Delay: Tools for Fractional Delay Filter Design
     * T.I. Laakso; V. Valimaki; M. Karjalainen; U.K. Laine; et al.
     */
    void updateCoefficients()
    {
        a1 = (1.0f - delay) / (1.0f + delay);
    }

    T delay = 0.0;
    T a1 = 0.0;
    T accumulator = 0.0;
};
