/*
  ==============================================================================

    PresetPanel.cpp
    Created: 3 Jan 2020 1:10:53pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "PresetPanel.h"
#include "Common/GUI/Managers/ToteBagPresetManager.h"

PresetPanel::PresetPanel (ToteBagPresetManager& pManager,
                          const juce::String& bypassButtonText,
                          const juce::String& bypassParameterId,
                          juce::AudioProcessorValueTreeState& treeState) :
 mBypassButton (bypassButtonText,
                bypassParameterId,
                treeState)
,presetManager (pManager)
{
    // set up save and load buttons
    mSavePresetButton.setButtonText ("Save");
    mSavePresetButton.onClick = [this]() {
        savePreset();
    };
    addAndMakeVisible (mSavePresetButton);

    mLoadPresetButton.setButtonText ("Load");
    mLoadPresetButton.onClick = [this]() {
        loadPreset();
    };
    addAndMakeVisible (mLoadPresetButton);

    addAndMakeVisible (mBypassButton);

    // set up preset combo box
    mPresetDisplay.setColour (ComboBox::ColourIds::backgroundColourId, Colours::black);
    mPresetDisplay.setColour (ComboBox::ColourIds::textColourId, Colours::darkgrey);
    mPresetDisplay.setColour (ComboBox::ColourIds::outlineColourId, Colours::grey);

    addAndMakeVisible (mPresetDisplay);
    mPresetDisplay.onChange = [this]() {
        handlePresetDisplaySelection();
    };
    mPresetDisplay.setText (currentPresetName, dontSendNotification);
    mPresetDisplay.setSelectedItemIndex(presetManager.getCurrentPresetIndex(), dontSendNotification);

    updatePresetComboBox();

    // set up preset iterator buttons
    mNextPreset.setButtonText(">");
    mNextPreset.onClick = [this]() { incrementPreset(); };
    addAndMakeVisible(mNextPreset);

    mPreviousPreset.setButtonText("<");
    mPreviousPreset.onClick = [this]() { decrementPreset(); };
    addAndMakeVisible(mPreviousPreset);

    startTimerHz (20);
}

PresetPanel::~PresetPanel()
{
    stopTimer();
}

void PresetPanel::paint(Graphics& g)
{
    g.setColour(Colours::darkgrey);
    g.fillAll();
}

void PresetPanel::incrementPreset()
{
    auto newPresetIndex = presetManager.getCurrentPresetIndex() + 1;
    if (newPresetIndex > presetManager.getNumberOfPresets() - 1)
    {
        newPresetIndex = 0;
    }
    presetManager.loadPreset(newPresetIndex);
}

void PresetPanel::decrementPreset()
{
    auto newPresetIndex = presetManager.getCurrentPresetIndex() - 1;
    if (newPresetIndex < 0)
    {
        newPresetIndex = presetManager.getNumberOfPresets() - 1;
    }
    presetManager.loadPreset(newPresetIndex);
}

void PresetPanel::savePreset()
{
    auto currentPresetName = presetManager.getCurrentPresetName();

    String currentPresetPath = presetManager.getCurrentPresetDirectory()
                               + static_cast<std::string>(directorySeparator) + currentPresetName;

    FileChooser chooser ("Save a file: ",
                         File (currentPresetPath),
                         static_cast<std::string>(presetFileExtensionWildcard));

    if (chooser.browseForFileToSave (true))
    {
        File presetToSave (chooser.getResult());

        presetManager.savePreset (presetToSave);

        updatePresetComboBox();
    }
}

void PresetPanel::loadPreset()
{
    String currentPresetDirectory = presetManager.getCurrentPresetDirectory();

    if (currentPresetDirectory.isNotEmpty())
    {

        FileChooser chooser ("Load a file: ",
                            File (currentPresetDirectory),
                            static_cast<std::string>(presetFileExtensionWildcard));

        if (chooser.browseForFileToOpen())
        {
            File presetToLoad (chooser.getResult());
            
            presetManager.loadPreset (presetToLoad);

        }
    }
}

void PresetPanel::handlePresetDisplaySelection()
{
    if (mPresetDisplay.getSelectedItemIndex() != -1)
    {
        const int index = mPresetDisplay.getSelectedItemIndex();
        presetManager.loadPreset (index);
    }
}

void PresetPanel::timerCallback()
{

    auto presetNameInManager = presetManager.getCurrentPresetName();
    if (currentPresetName != presetNameInManager)
    {
        mPresetDisplay.setText (presetNameInManager, dontSendNotification);
        currentPresetName = presetNameInManager;
        
    }
    
}

void PresetPanel::updatePresetComboBox()
{
    
    mPresetDisplay.clear (dontSendNotification);

    const int numPresets = presetManager.getNumberOfPresets();

    for(int i = 0;  i < numPresets; i++)
    {
        mPresetDisplay.addItem (presetManager.getPresetName (i), (i+1));
    }
        
    mPresetDisplay.setText (presetManager.getCurrentPresetName(), dontSendNotification);
    
}

void PresetPanel::resized()
{
    const auto area = getLocalBounds();
    const auto margin = juce::roundToInt(area.getHeight() * .1f);

    // Adjust width to account for margins
    const auto actualWidth = area.getWidth() - (margin * 6);
    const auto buttonAreaWidth = actualWidth * .5f;

    // Get width button groups on left side. Coefficients should sum to 1.
    const auto saveLoadAreaWidth = buttonAreaWidth * .7f;
    const auto bypassAreaWidth = buttonAreaWidth * .2f;
    const auto incrementDecrementAreaWidth = buttonAreaWidth * .1f;

    auto x = area.getX() + margin; // We update this while we place components
    const auto y = area.getY() + margin;
    const auto h = juce::roundToInt(area.getHeight() - (margin * 2.0f));

    // Place bypass button
    const auto bypassButtonWidth = juce::roundToInt(bypassAreaWidth);
    juce::Rectangle<int> bypassBounds{x, y, bypassButtonWidth, h};
    mBypassButton.setBounds(bypassBounds);
    x += (bypassButtonWidth + margin);

    // Place save and load buttons
    const auto saveLoadButtonWidth = juce::roundToInt(saveLoadAreaWidth * .5f);
    juce::Rectangle<int> saveLoadBounds;
    for (auto button : {&mSavePresetButton, &mLoadPresetButton})
    {
        saveLoadBounds.setBounds(x, y, saveLoadButtonWidth, h);
        button->setBounds(saveLoadBounds);
        x += (saveLoadButtonWidth + margin);
    }

    // Place increment and decrement buttons
    juce::Rectangle<int> incrementDecrementBounds;
    const auto incrementDecrementButtonWidth = juce::roundToInt(incrementDecrementAreaWidth * .5f);
    for (auto button : {&mPreviousPreset, &mNextPreset})
    {
        incrementDecrementBounds.setBounds(x, y, incrementDecrementButtonWidth, h);
        button->setBounds(incrementDecrementBounds);
        x += (incrementDecrementButtonWidth + margin);
    }

    // Place preset selection window
    const auto presetAreaWidth = actualWidth - buttonAreaWidth;
    juce::Rectangle<int> presetSelectorBounds{x, y, juce::roundToInt(presetAreaWidth) - margin, h};
    mPresetDisplay.setBounds(presetSelectorBounds);
}
