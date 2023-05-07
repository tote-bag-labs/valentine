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
    inputGain = 0,
    bitCrush,
    saturation,
    ratio,
    attack,
    release,
    dryWet,
    makeupGain,
    nice,
    bypass,
    TOTAL_NUM_PARAMETERS
};

namespace
{
const size_t getParameterIndex (VParameter param)
{
    return static_cast<size_t> (param);
}
} // namespace

static constexpr auto numParams = static_cast<int> (VParameter::TOTAL_NUM_PARAMETERS);

inline const std::array<juce::String, numParams>& FFCompParameterID()
{
    static const std::array<juce::String, numParams> parameterIDs = {
        "Compress",
        "Crush",
        "Saturation",
        "Ratio",
        "AttackTime",
        "ReleaseTime",
        "Mix",
        "Makeup",
        "Nice",
        "Bypass"
    };

    return parameterIDs;
}

inline const std::array<juce::String, numParams>& FFCompParameterLabel()
{
    static const std::array<juce::String, numParams> parameterLabels = {
        "Input",
        "Crush",
        "Saturation",
        "Ratio",
        "Attack",
        "Release",
        "Mix",
        "Output",
        "Nice",
        "Bypass"
    };

    return parameterLabels;
}

inline const std::array<juce::String, numParams>& VParameterUnit()
{
    static const std::array<juce::String, numParams> unitLabels = {
        " dB",
        " %",
        " %",
        "",
        " ms",
        " ms",
        " %",
        " dB",
        "",
        ""
    };

    return unitLabels;
}

static constexpr std::array<float, numParams> FFCompParameterMin = {
    -24.0f,
    0.0f,
    0.0f,
    1.0f,
    0.02f,
    50.0f,
    0.0f,
    -12.0f,
    0.0f,
    0.0f
};

static constexpr std::array<float, numParams> FFCompParameterMax = {
    48.0f,
    100.0f,
    100.0f,
    21.0f,
    10.0f,
    1100.0f,
    100.0f,
    24.0f,
    1.0f,
    1.0f
};

static constexpr std::array<float, numParams> FFCompParameterDefaults = {
    0.0f,
    0.0f,
    0.0f,
    4.0f,
    1.3f,
    350.0f,
    100.0f,
    8.0f,
    0.0f,
    0.0f
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
    1.0f
};

static constexpr std::array<float, numParams> FFCompParamCenter = {
    23.0f,
    60.0f,
    60.0f,
    6.0f,
    5.0f,
    300.0f,
    50.0f,
    0.0f,
    0.5f,
    0.5f
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
    0
};

//==================================================================================

static constexpr std::array<int, 5> ratioValues = {
    4,
    8,
    12,
    20,
    1000,
};

static constexpr std::array<float, 5> kneeValues = {
    6.0f,
    3.84f,
    2.16f,
    .96f,
    0.0f,
};

static constexpr std::array<float, 5> thresholdValues = {
    -18.0f,
    -14.0f,
    -13.0f,
    -12.0f,
    -10.0f,
};

static constexpr float kMinSaturationGain = 1.0f;
static constexpr float kMaxSaturationGain = 30.0f;
