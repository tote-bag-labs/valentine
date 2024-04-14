/*
  ==============================================================================

    PresetPanel.h
    Created: 3 Jan 2020 1:10:53pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "tote_bag/juce_gui/components/widgets/ParameterTextButton.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <functional>

class ToteBagPresetManager;
class PresetPanel : public juce::Component, public juce::Timer
{
public:
    PresetPanel (ToteBagPresetManager& pManager,
                 const juce::String& bypassButtonText,
                 const juce::String& bypassParameterId,
                 std::function<void()> infoButtonCallback,
                 juce::AudioProcessorValueTreeState& treeState);
    ~PresetPanel() override;

    void paint (juce::Graphics& g) override;

    void timerCallback() override;

    void resized() override;

private:
    void setupValentineInfoButton();

    void setupBypassButton();

    void setupSaveAndLoadButtons();

    void setupPresetIncrementButtons();

    void setupPresetDisplay();

    void handlePresetDisplaySelection();

    void updatePresetDisplay();

    juce::Rectangle<int> outerBorder;

    juce::DrawableButton mInfoButton;
    juce::TextButton mSavePresetButton;
    juce::TextButton mLoadPresetButton;
    juce::DrawableButton mPreviousPreset;
    juce::DrawableButton mNextPreset;
    ParameterTextButton mBypassButton;

    juce::ComboBox mPresetDisplay;

    juce::String currentPresetName {"Untitled"};

    ToteBagPresetManager& presetManager;

    std::function<void()> minfoButtonCallback;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPanel)
};
