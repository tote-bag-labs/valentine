// 2023 Tote Bag Labs

#include "tote_bag/utils/tbl_math.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

TEST_CASE ("findNearestIndices - basics", "[Math]")
{
    using namespace tote_bag::math;

    SECTION ("Get nearest indices for int")
    {
        const std::array<int, 10> array = {0, 1, 2, 3, 5, 7, 12, 100, 120, 130};
        const auto result = findNearestIndices (array, 110);

        REQUIRE (result.first == 7);
        REQUIRE (result.second == 8);
    }
}

TEST_CASE ("findNearestIndices - out of Bounds", "[Math]")
{
    using namespace tote_bag::math;

    SECTION ("Get nearest indices for out of bounds values")
    {
        const std::array<float, 5> array = {0.5f, 1.0f, 2.0f, 3.0f, 5.0f};

        const auto result2 = findNearestIndices (array, 0.0f);
        REQUIRE (result2.first == 0);
        REQUIRE (result2.second == 1);

        const auto result = findNearestIndices (array, 10.0f);
        REQUIRE (result.first == 3);
        REQUIRE (result.second == 4);
    }
}

TEST_CASE ("findNearestValues - basics", "[Math]")
{
    using namespace tote_bag::math;

    SECTION ("Get nearest values for int")
    {
        const std::array<int, 10> array = {0, 1, 2, 3, 5, 7, 12, 100, 120, 130};
        const auto result = findNearestValues (array, 110);

        REQUIRE (result.first == 100);
        REQUIRE (result.second == 120);
    }

    SECTION ("Get nearest values for float")
    {
        const std::array<float, 5> array = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f};
        const auto result = findNearestValues (array, 1.5f);

        REQUIRE (result.first == 1.0f);
        REQUIRE (result.second == 2.0f);
    }
}

TEST_CASE ("findNearestValues - out of bounds", "[Math]")
{
    using namespace tote_bag::math;

    SECTION ("Get nearest values for out of bounds values")
    {
        const std::array<float, 5> array = {0.5f, 1.0f, 2.0f, 3.0f, 5.0f};

        const auto result = findNearestValues (array, 10.0f);
        REQUIRE (result.first == 3.0f);
        REQUIRE (result.second == 5.0f);

        const auto result2 = findNearestValues (array, 0.0f);
        REQUIRE (result2.first == 0.5f);
        REQUIRE (result2.second == 1.0f);
    }
}

TEST_CASE ("piecewiseLinearInterpolation - basics", "[Math]")
{
    using namespace tote_bag::math;

    SECTION ("Remap to positive values")
    {
        const std::array<float, 5> inputRange = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f};
        const std::array<float, 5> outputRange = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f};
        const auto result = piecewiseLinearInterpolate (inputRange, outputRange, 1.5f);
        REQUIRE (result == 25.0f);
    }

    SECTION ("Remap to descending values")
    {
        const std::array<float, 5> inputRange = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f};
        const std::array<float, 5> outputRange = {50.0f, 40.0f, 30.0f, 20.0f, 10.0f};
        const auto result = piecewiseLinearInterpolate (inputRange, outputRange, 1.5f);
        REQUIRE (result == 35.0f);
    }

    SECTION ("Remap to negative values")
    {
        const std::array<float, 5> inputRange = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f};
        const std::array<float, 5> outputRange = {-10.0f, -20.0f, -30.0f, -40.0f, -50.0f};
        const auto result = piecewiseLinearInterpolate (inputRange, outputRange, 1.5f);
        REQUIRE (result == -25.0f);
    }

    SECTION ("Remap to control values")
    {
        const std::array<float, 5> inputRange = {0.0f, 1.0f, 2.0f, 3.0f, 5.0f};
        const std::array<float, 5> outputRange = {-10.0f, -20.0f, -30.0f, -40.0f, -50.0f};
        const auto result = piecewiseLinearInterpolate (inputRange, outputRange, 3.0f);
        REQUIRE (result == -40.0f);
    }
}
