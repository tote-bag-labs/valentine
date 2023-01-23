/*
  ==============================================================================

    PresetPanel.h
    Created: 3 Jan 2020 1:10:53pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

#include "Common/GUI/Components/Widgets/ParameterTextButton.h"

class ToteBagPresetManager;
class PresetPanel  : public juce::Component,
                     public juce::Timer
{
public:
    
    PresetPanel (ToteBagPresetManager& pManager,
                 const juce::String& bypassButtonText,
                 const juce::String& bypassParameterId,
                 juce::AudioProcessorValueTreeState& treeState);
    ~PresetPanel();
    
    void paint (Graphics& g) override;
    
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

    ComboBox mPresetDisplay;

    juce::String currentPresetName {"Untitled"};
    
    ToteBagPresetManager& presetManager;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetPanel)
};

