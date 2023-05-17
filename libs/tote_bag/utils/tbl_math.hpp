// 2023 Tote Bag Labs

#include <algorithm>
#include <array>
#include <cstddef>
#include <utility>

namespace tote_bag
{
namespace math
{

/** Returns the indexes for the values in the input range smaller and larger than the input value.
 *  Input range must be sorted.
 */
template <typename ValueType, size_t ArraySize>
std::pair<size_t, size_t>
    findNearestIndices (const std::array<ValueType, ArraySize> inputRange,
                        const ValueType inputValue)
{
    static_assert (ArraySize > 1, "ArraySize must be greater than 1");

    auto it = std::upper_bound (inputRange.begin(), inputRange.end(), inputValue);
    auto upperIndex = static_cast<size_t> (std::distance (inputRange.begin(), it));

    if (upperIndex == 0)
    {
        return std::make_pair (0, 1);
    }
    else if (upperIndex == ArraySize)
    {
        return std::make_pair (ArraySize - 2, ArraySize - 1);
    }
    else
    {
        return std::make_pair (upperIndex - 1, upperIndex);
    }
}

/** Returns the values in the input range that are smaller and larger than the input value.
 *  Input range must be sorted.
 */
template <typename ValueType, size_t ArraySize>
std::pair<ValueType, ValueType>
    findNearestValues (const std::array<ValueType, ArraySize> inputRange,
                       const ValueType inputValue)
{
    const auto indices = findNearestIndices (inputRange, inputValue);
    return std::make_pair (inputRange[indices.first], inputRange[indices.second]);
}

/** Interpolates a value from one range of the other, using sections corresponding to
 *  the upper and lower bounds of the input value as found in the input range.
 */
template <typename ValueType, size_t ArraySize>
ValueType piecewiseLinearInterpolate (const std::array<ValueType, ArraySize> inputRange,
                                      const std::array<ValueType, ArraySize> outputRange,
                                      const ValueType input)
{
    const auto [lowerBound, upperBound] = findNearestIndices (inputRange, input);
    const auto normalizedInput = (input - inputRange[lowerBound])
                                 / (inputRange[upperBound] - inputRange[lowerBound]);

    return outputRange[lowerBound]
           + (outputRange[upperBound] - outputRange[lowerBound]) * normalizedInput;
}

} // namespace math
} // namespace tote_bag
