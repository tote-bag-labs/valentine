/*
  ==============================================================================

    PresetPanel.cpp
    Created: 3 Jan 2020 1:10:53pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "PresetPanel.h"
#include "tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h"
#include "tote_bag/juce_gui/managers/ToteBagPresetManager.h"

#include <BinaryData.h>

PresetPanel::PresetPanel (ToteBagPresetManager& pManager,
                          const juce::String& bypassButtonText,
                          const juce::String& bypassParameterId,
                          juce::AudioProcessorValueTreeState& treeState)
    : borderThickness (0.0f)
    , mPreviousPreset ("PreviousPreset", juce::DrawableButton::ButtonStyle::ImageFitted)
    , mNextPreset ("NextPreset", juce::DrawableButton::ButtonStyle::ImageFitted)
    , mBypassButton (bypassButtonText, bypassParameterId, treeState)
    , presetManager (pManager)
{
    presetManager.setPresetSavedCallback ([this]() { updatePresetComboBox(); });

    // set up save and load buttons
    mSavePresetButton.setButtonText ("SAVE");
    mSavePresetButton.onClick = [this]() { presetManager.savePreset(); };
    addAndMakeVisible (mSavePresetButton);

    mSavePresetButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                                 tote_bag::colours::plainWhite);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                                 tote_bag::colours::plainWhite);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                                 tote_bag::colours::plainBlack);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                                 tote_bag::colours::plainBlack);

    mLoadPresetButton.setButtonText ("LOAD");
    mLoadPresetButton.onClick = [this]() { presetManager.loadPreset(); };
    addAndMakeVisible (mLoadPresetButton);

    mLoadPresetButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                                 tote_bag::colours::plainWhite);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                                 tote_bag::colours::plainWhite);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                                 tote_bag::colours::plainBlack);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                                 tote_bag::colours::plainBlack);

    mBypassButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                             tote_bag::colours::slateGrey);
    mBypassButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                             tote_bag::colours::slateGrey);
    mBypassButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                             tote_bag::colours::plainWhite);
    mBypassButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                             tote_bag::colours::plainWhite);

    addAndMakeVisible (mBypassButton);

    // set up preset combo box
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::backgroundColourId,
                              tote_bag::colours::slateGrey);
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::textColourId,
                              tote_bag::colours::plainWhite);
    mPresetDisplay.setColour (juce::ComboBox::ColourIds::outlineColourId,
                              tote_bag::colours::plainBlack);

    addAndMakeVisible (mPresetDisplay);
    mPresetDisplay.onChange = [this]() { handlePresetDisplaySelection(); };
    mPresetDisplay.setText (currentPresetName, juce::dontSendNotification);
    mPresetDisplay.setSelectedItemIndex (presetManager.getCurrentPresetIndex(),
                                         juce::dontSendNotification);

    updatePresetComboBox();

    setupPresetIncrementButtons();

    startTimerHz (20);
}

PresetPanel::~PresetPanel()
{
    stopTimer();
}

void PresetPanel::paint (juce::Graphics& g)
{
    g.setColour (tote_bag::colours::plainWhite);
    g.fillAll();

    g.setColour (tote_bag::colours::plainBlack);
    g.drawRect (getBounds(), borderThickness);
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
    borderThickness = juce::roundToInt (margin * .4f);

    auto presetBounds = area.reduced (borderThickness);

    const auto presetBoundsWidth = presetBounds.getWidth();

    const auto leftTotieGapWidth = juce::roundToInt (presetBoundsWidth * .02f);
    const auto totieButtonWidth = juce::roundToInt (presetBoundsWidth * .045f);
    const auto totieBypassGapWidth = juce::roundToInt (presetBoundsWidth * .005f);
    const auto bypassButtonWidth = juce::roundToInt (presetBoundsWidth * .15f);
    const auto bypassSaveGap = juce::roundToInt (presetBoundsWidth * .10f);
    const auto saveLoadButtonWidth = juce::roundToInt (presetBoundsWidth * .19f);
    const auto saveLoadGapWidth = juce::roundToInt (presetBoundsWidth * .0127f);
    const auto loadPrevGapWidth = juce::roundToInt (presetBoundsWidth * .028f);
    const auto prevNextButtonWidth = juce::roundToInt (presetBoundsWidth * .0225f);
    const auto prevNextGapWidth = juce::roundToInt (presetBoundsWidth * .0112f);
    const auto presetBoxWidth = juce::roundToInt (presetBoundsWidth * .25f);

    presetBounds.removeFromLeft (leftTotieGapWidth);
    presetBounds.removeFromLeft (totieButtonWidth);
    presetBounds.removeFromLeft (totieBypassGapWidth);
    mBypassButton.setBounds (presetBounds.removeFromLeft (bypassButtonWidth));
    presetBounds.removeFromLeft (bypassSaveGap);
    mSavePresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth));
    presetBounds.removeFromLeft (saveLoadGapWidth);
    mLoadPresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth));
    presetBounds.removeFromLeft (loadPrevGapWidth);
    mPreviousPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth));
    presetBounds.removeFromLeft (prevNextGapWidth);
    mNextPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth));
    presetBounds.removeFromLeft (loadPrevGapWidth);
    mPresetDisplay.setBounds (presetBounds.removeFromLeft (presetBoxWidth));
}

void PresetPanel::setupPresetIncrementButtons()
{
    mPreviousPreset.onClick = [this]() { presetManager.loadPreviousPreset(); };
    mPreviousPreset.setImages (
        juce::Drawable::createFromImageData (BinaryData::left_arrow_svg,
                                             BinaryData::left_arrow_svgSize)
            .get());
    addAndMakeVisible (mPreviousPreset);

    mNextPreset.onClick = [this]() { presetManager.loadNextPreset(); };
    mNextPreset.setImages (
        juce::Drawable::createFromImageData (BinaryData::right_arrow_svg,
                                             BinaryData::right_arrow_svgSize)
            .get());
    addAndMakeVisible (mNextPreset);
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
