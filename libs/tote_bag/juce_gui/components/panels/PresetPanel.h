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

class ToteBagPresetManager;
class PresetPanel : public juce::Component,
                    public juce::Timer
{
public:
    PresetPanel (ToteBagPresetManager& pManager,
                 const juce::String& bypassButtonText,
                 const juce::String& bypassParameterId,
                 juce::AudioProcessorValueTreeState& treeState);
    ~PresetPanel() override;

    void paint (juce::Graphics& g) override;

    void incrementPreset();

    void decrementPreset();

    void savePreset();

    void loadPreset();

    void handlePresetDisplaySelection();

    void resized() override;

    void timerCallback() override;

    void updatePresetComboBox();

private:
    juce::TextButton mSavePresetButton;
    juce::TextButton mLoadPresetButton;
    juce::TextButton mPreviousPreset;
    juce::TextButton mNextPreset;
    ParameterTextButton mBypassButton;

    juce::ComboBox mPresetDisplay;

    juce::String currentPresetName { "Untitled" };

    ToteBagPresetManager& presetManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPanel)
};
