//
//  AudioHelpersTests.cpp
//  Valentine
//
//  Created by Jose Diaz on 19.03.23.
//

#include "tote_bag/dsp/AudioHelpers.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <cmath>

TEST_CASE ("Next power of two", "[Audio Helpers]")
{
    // Finds the next power of two for given input
    const int nextPower = tote_bag::audio_helpers::nextPow2 (200);
    REQUIRE (nextPower == 256);

    // Returns the same as input if it is a power of 2
    const int nextPowerSame = tote_bag::audio_helpers::nextPow2 (256);
    REQUIRE (nextPowerSame == 256);
}

TEST_CASE ("Clamped cosh won't blow up", "[Audio Helpers]")
{
    // cosh will overflow floats at ~71.0f
    const auto floatResult = tote_bag::audio_helpers::clampedCosh (750.0f);

    REQUIRE (std::fpclassify (floatResult) == FP_NORMAL);

    // cosh will overflow doubles at ~710.0
    const auto doubleResult = tote_bag::audio_helpers::clampedCosh (750.0);

    REQUIRE (std::fpclassify (doubleResult) == FP_NORMAL);
}
