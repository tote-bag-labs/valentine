/*
  ==============================================================================

    FFCompParameters.h
    Created: 3 Aug 2019 9:54:08pm
    Author:  dev

  ==============================================================================
*/

#pragma once

#include <array>

#include <juce_core/juce_core.h>

constexpr int ValentineParameterVersion = 1;

enum class VParameter {
    bitCrush = 0,
    inputGain,
    saturation,
    ratio,
    attack,
    release,
    dryWet,
    makeupGain,
    bypass,
    TOTAL_NUM_PARAMETERS
};

namespace
{
const size_t getParameterIndex (VParameter param)
{
    return static_cast<size_t> (param);
}

inline constexpr float kMinBits = 1.0f;
inline constexpr float kMaxBits = 16.0f;

inline constexpr float kMinSaturationGain = 1.0f;
inline constexpr float kMaxSaturationGain = 30.0f;

// The largest the ratio can be as far as the parameter itself is concerned.
// Processing, we actually use a ratio of 1000:1 for this ratio value.
inline constexpr float kRatioParameterMax = 21.0f;

inline constexpr float kRatioMin = 1.0f;
inline constexpr float kRatioMax = 1000.0f;

inline constexpr float kKneeMin = 7.0f;
inline constexpr float kKneeMax = 0.0f;

inline constexpr float kThresholdMin = -15.0f;
inline constexpr float kThresholdMax = -10.0f;

inline constexpr size_t kNumRatioControlPoints = 6;
inline constexpr std::array<float, kNumRatioControlPoints> kRatioControlPoints = {
    kRatioMin,
    4.0f,
    8.0f,
    12.0f,
    20.0,
    kRatioMax,
};

inline constexpr std::array<float, kNumRatioControlPoints> kKneeControlPoints = {
    kKneeMin,
    6.0f,
    3.84f,
    2.16f,
    .96f,
    kKneeMax,
};

inline constexpr std::array<float, kNumRatioControlPoints> kThresholdControlPoints = {
    kThresholdMin,
    -18.0f,
    -14.0f,
    -13.0f,
    -12.0f,
    kThresholdMax,
};
} // namespace

static constexpr auto numParams = static_cast<int> (VParameter::TOTAL_NUM_PARAMETERS);

inline const std::array<juce::String, numParams>& FFCompParameterID()
{
    static const std::array<juce::String, numParams> parameterIDs = {
        "Crush",
        "Compress",
        "Saturate",
        "Ratio",
        "AttackTime",
        "ReleaseTime",
        "Mix",
        "Makeup",
        "Bypass",
    };

    return parameterIDs;
}

inline const std::array<juce::String, numParams>& FFCompParameterLabel()
{
    static const std::array<juce::String, numParams> parameterLabels = {
        "Crush",
        "Compress",
        "Saturate",
        "Ratio",
        "Attack",
        "Release",
        "Mix",
        "Output",
        "Bypass",
    };

    return parameterLabels;
}

inline const std::array<juce::String, numParams>& VParameterUnit()
{
    static const std::array<juce::String, numParams> unitLabels = {
        " %",
        " dB",
        " %",
        "",
        " ms",
        " ms",
        " %",
        " dB",
        "",
    };

    return unitLabels;
}

static constexpr std::array<float, numParams> FFCompParameterMin = {
    0.0f,
    -24.0f,
    0.0f,
    kRatioMin,
    0.02f,
    50.0f,
    0.0f,
    -12.0f,
    0.0f,
};

static constexpr std::array<float, numParams> FFCompParameterMax = {
    100.0f,
    48.0f,
    100.0f,
    kRatioParameterMax,
    10.0f,
    1100.0f,
    100.0f,
    24.0f,
    1.0f,
};

static constexpr std::array<float, numParams> FFCompParameterDefaults = {
    0.0f,
    0.0f,
    0.0f,
    4.0f,
    1.3f,
    350.0f,
    100.0f,
    0.0f,
    0.0f,
};

static constexpr std::array<float, numParams> FFCompParameterIncrement = {
    0.00001f,
    0.00001f,
    0.00001f,
    0.00001f,
    0.00001f,
    0.00001f,
    0.00001f,
    0.00001f,
    1.0f,
};

static constexpr std::array<float, numParams> FFCompParamCenter = {
    60.0f,
    23.0f,
    60.0f,
    6.0f,
    5.0f,
    300.0f,
    50.0f,
    0.0f,
    0.5f,
};

static constexpr std::array<int, numParams> VParamPrecision = {
    2,
    2,
    2,
    2,
    2,
    2,
    0,
    2,
    0,
};

//==================================================================================
