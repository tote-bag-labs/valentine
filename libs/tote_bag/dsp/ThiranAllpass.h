/*
  ==============================================================================

    ThiranAllpass.h
    Created: 14 Oct 2019 11:10:18am
    Author:  dev

  ==============================================================================
*/

#pragma once

#include <cassert>

/** A first order Thiran allpass filter. This is a simple allpass filter that
 *  has a single coefficient, a1, and a single state variable, accumulator.
 *  This filter will sound best for delay times around 1 sample.
 */
template <typename T>
class FirstOrderThiranAllpass
{
public:
    /** Resets the filter state to zero. This should be called before processing.
     */
    void reset()
    {
        accumulator = 0.0;
    }

    /** Prepares the filter for processing. This should be called before processing.
     *  @param inDelay The delay time in samples. This should be between 0 and 2.
     */
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
