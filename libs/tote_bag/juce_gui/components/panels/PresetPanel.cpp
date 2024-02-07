/*
  ==============================================================================

    PresetPanel.cpp
    Created: 3 Jan 2020 1:10:53pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "PresetPanel.h"
#include "tote_bag/juce_gui/managers/ToteBagPresetManager.h"

PresetPanel::PresetPanel (ToteBagPresetManager& pManager,
                          const juce::String& bypassButtonText,
                          const juce::String& bypassParameterId,
                          juce::AudioProcessorValueTreeState& treeState)
    : mBypassButton (bypassButtonText, bypassParameterId, treeState)
    , presetManager (pManager)
{
    presetManager.setPresetSavedCallback ([this]() { updatePresetComboBox(); });

    // set up save and load buttons
    mSavePresetButton.setButtonText ("Save");
    mSavePresetButton.onClick = [this]() { presetManager.savePreset(); };
    addAndMakeVisible (mSavePresetButton);

    mLoadPresetButton.setButtonText ("Load");
    mLoadPresetButton.onClick = [this]() { presetManager.loadPreset(); };
    addAndMakeVisible (mLoadPresetButton);

    addAndMakeVisible (mBypassButton);

    // set up preset combo box
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::backgroundColourId,
                              juce::Colours::black);
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::textColourId,
                              juce::Colours::darkgrey);
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::outlineColourId,
                              juce::Colours::grey);

    addAndMakeVisible (mPresetDisplay);
    mPresetDisplay.onChange = [this]() { handlePresetDisplaySelection(); };
    mPresetDisplay.setText (currentPresetName, juce::dontSendNotification);
    mPresetDisplay.setSelectedItemIndex (presetManager.getCurrentPresetIndex(),
                                         juce::dontSendNotification);

    updatePresetComboBox();

    // set up preset iterator buttons
    mNextPreset.setButtonText (">");
    mNextPreset.onClick = [this]() { presetManager.loadNextPreset(); };
    addAndMakeVisible (mNextPreset);

    mPreviousPreset.setButtonText ("<");
    mPreviousPreset.onClick = [this]() { presetManager.loadPreviousPreset(); };
    addAndMakeVisible (mPreviousPreset);

    startTimerHz (20);
}

PresetPanel::~PresetPanel()
{
    stopTimer();
}

void PresetPanel::paint (juce::Graphics& g)
{
    g.setColour (juce::Colours::darkgrey);
    g.fillAll();
}

void PresetPanel::timerCallback()
{
    auto presetNameInManager = presetManager.getCurrentPresetName();
    if (currentPresetName != presetNameInManager)
    {
        mPresetDisplay.setText (presetNameInManager, juce::dontSendNotification);
        currentPresetName = presetNameInManager;
    }
}

void PresetPanel::resized()
{
    const auto area = getLocalBounds();
    const auto margin = juce::roundToInt (area.getHeight() * .1f);

    // Adjust width to account for margins
    const auto actualWidth = area.getWidth() - (margin * 6);
    const auto buttonAreaWidth = actualWidth * .5f;

    // Get width button groups on left side. Coefficients should sum to 1.
    const auto saveLoadAreaWidth = buttonAreaWidth * .7f;
    const auto bypassAreaWidth = buttonAreaWidth * .2f;
    const auto incrementDecrementAreaWidth = buttonAreaWidth * .1f;

    auto x = area.getX() + margin; // We update this while we place components
    const auto y = area.getY() + margin;
    const auto h = juce::roundToInt (area.getHeight() - (margin * 2.0f));

    // Place bypass button
    const auto bypassButtonWidth = juce::roundToInt (bypassAreaWidth);
    juce::Rectangle<int> bypassBounds {x, y, bypassButtonWidth, h};
    mBypassButton.setBounds (bypassBounds);
    x += (bypassButtonWidth + margin);

    // Place save and load buttons
    const auto saveLoadButtonWidth = juce::roundToInt (saveLoadAreaWidth * .5f);
    juce::Rectangle<int> saveLoadBounds;
    for (auto button : {&mSavePresetButton, &mLoadPresetButton})
    {
        saveLoadBounds.setBounds (x, y, saveLoadButtonWidth, h);
        button->setBounds (saveLoadBounds);
        x += (saveLoadButtonWidth + margin);
    }

    // Place increment and decrement buttons
    juce::Rectangle<int> incrementDecrementBounds;
    const auto incrementDecrementButtonWidth =
        juce::roundToInt (incrementDecrementAreaWidth * .5f);
    for (auto button : {&mPreviousPreset, &mNextPreset})
    {
        incrementDecrementBounds.setBounds (x, y, incrementDecrementButtonWidth, h);
        button->setBounds (incrementDecrementBounds);
        x += (incrementDecrementButtonWidth + margin);
    }

    // Place preset selection window
    const auto presetAreaWidth = actualWidth - buttonAreaWidth;
    juce::Rectangle<int> presetSelectorBounds {x,
                                               y,
                                               juce::roundToInt (presetAreaWidth)
                                                   - margin,
                                               h};
    mPresetDisplay.setBounds (presetSelectorBounds);
}

void PresetPanel::handlePresetDisplaySelection()
{
    if (mPresetDisplay.getSelectedItemIndex() != -1)
    {
        presetManager.loadPreset (mPresetDisplay.getSelectedItemIndex());
    }
}

void PresetPanel::updatePresetComboBox()
{
    mPresetDisplay.clear (juce::dontSendNotification);

    const int numPresets = presetManager.getNumberOfPresets();

    for (int i = 0; i < numPresets; i++)
    {
        mPresetDisplay.addItem (presetManager.getPresetName (i), (i + 1));
    }

    mPresetDisplay.setText (presetManager.getCurrentPresetName(),
                            juce::dontSendNotification);
}
