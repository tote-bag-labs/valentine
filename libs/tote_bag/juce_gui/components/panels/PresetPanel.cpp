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

#if JUCE_ENABLE_LIVE_CONSTANT_EDITOR
    #include <juce_gui_extra/juce_gui_extra.h>
#endif // JUCE_ENABLE_LIVE_CONSTANT_EDITOR

#include <BinaryData.h>

namespace detail
{
constexpr auto kTotieWidth = 144.22f;
constexpr auto kTotieHeight = 135.51f;
constexpr auto kTotieHWRatio = kTotieHeight / kTotieWidth;
}

PresetPanel::PresetPanel (ToteBagPresetManager& pManager,
                          const juce::String& bypassButtonText,
                          const juce::String& bypassParameterId,
                          std::function<void()> infoButtonCallback,
                          juce::AudioProcessorValueTreeState& treeState)
    : mInfoButton ("ValentineInfo", juce::DrawableButton::ButtonStyle::ImageStretched)
    , mPreviousPreset ("PreviousPreset",
                       juce::DrawableButton::ButtonStyle::ImageStretched)
    , mNextPreset ("NextPreset", juce::DrawableButton::ButtonStyle::ImageStretched)
    , mBypassButton (bypassButtonText, bypassParameterId, treeState)
    , presetManager (pManager)
    , minfoButtonCallback (infoButtonCallback)
{
    presetManager.setPresetSavedCallback ([this]() { updatePresetDisplay(); });

    mInfoButton.onClick = [this]() { minfoButtonCallback(); };

    setupValentineInfoButton();
    setupBypassButton();
    setupSaveAndLoadButtons();
    setupPresetIncrementButtons();
    setupPresetDisplay();

    startTimerHz (20);
}

PresetPanel::~PresetPanel()
{
    stopTimer();
}

void PresetPanel::paint (juce::Graphics& g)
{
    g.setColour (tote_bag::colours::offWhite);
    g.fillAll();

    g.setColour (tote_bag::colours::plainBlack);
    g.drawRect (getBounds(), mBorderThickness);
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

    mBorderThickness = juce::roundToInt (area.getHeight() * .04f);

    auto presetBounds = area.reduced (mBorderThickness);

    const auto presetBoundsWidth = presetBounds.getWidth();
    const auto presetBoundsHeight = presetBounds.getHeight();
    const auto presetBoundsCentreY = presetBounds.getCentreY();

    const auto leftInfoButtonGapWidth = juce::roundToInt (presetBoundsWidth * .026f);
    presetBounds.removeFromLeft (leftInfoButtonGapWidth);

    const auto infoButtonWidth = juce::roundToInt (presetBoundsWidth * 0.05f);
    const auto infoButtonHeight =
        juce::roundToInt (infoButtonWidth * detail::kTotieHWRatio);
    const auto infoButtonY = presetBoundsCentreY - infoButtonHeight / 2;

    mInfoButton.setBounds (presetBounds.removeFromLeft (infoButtonWidth)
                               .withY (infoButtonY)
                               .withHeight (infoButtonHeight));

    const auto infoButtonBypassGapWidth = juce::roundToInt (presetBoundsWidth * 0.0027f);
    presetBounds.removeFromLeft (infoButtonBypassGapWidth);

    const auto bypassSaveLoadButtonHeight = juce::roundToInt (presetBoundsHeight * .8f);
    const auto bypassSaveLoadButtonY =
        presetBoundsCentreY - bypassSaveLoadButtonHeight / 2;

    const auto bypassButtonWidth = juce::roundToInt (presetBoundsWidth * .134f);

    mBypassButton.setBounds (presetBounds.removeFromLeft (bypassButtonWidth)
                                 .withY (bypassSaveLoadButtonY)
                                 .withHeight (bypassSaveLoadButtonHeight));

    const auto bypassSaveGapWidth = juce::roundToInt (presetBoundsWidth * .0825f);
    presetBounds.removeFromLeft (bypassSaveGapWidth);

    const auto saveLoadButtonWidth = juce::roundToInt (presetBoundsWidth * .157f);

    mSavePresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth)
                                     .withY (bypassSaveLoadButtonY)
                                     .withHeight (bypassSaveLoadButtonHeight));

    const auto saveLoadGapWidth = juce::roundToInt (presetBoundsWidth * .007f);
    presetBounds.removeFromLeft (saveLoadGapWidth);

    mLoadPresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth)
                                     .withY (bypassSaveLoadButtonY)
                                     .withHeight (bypassSaveLoadButtonHeight));

    // This is used to set the gap between load button and previous button as well
    // as next button and preset display box
    const auto loadPrevBoxGapWidth = juce::roundToInt (presetBoundsWidth * .016f);
    presetBounds.removeFromLeft (loadPrevBoxGapWidth);

    const auto prevNextButtonHeight = juce::roundToInt (presetBoundsHeight * .2f);
    const auto prevNextButtonY = presetBoundsCentreY - prevNextButtonHeight / 2;
    const auto prevNextButtonWidth = juce::roundToInt (presetBoundsWidth * .024f);

    mPreviousPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth)
                                   .withY (prevNextButtonY)
                                   .withHeight (prevNextButtonHeight));

    const auto prevNextGapWidth = juce::roundToInt (presetBoundsWidth * .0117f);
    presetBounds.removeFromLeft (prevNextGapWidth);

    mNextPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth)
                               .withY (prevNextButtonY)
                               .withHeight (prevNextButtonHeight));

    presetBounds.removeFromLeft (loadPrevBoxGapWidth);

    const auto presetDisplayHeight = juce::roundToInt (presetBoundsHeight * .85f);
    const auto presetDisplayY = presetBoundsCentreY - presetDisplayHeight / 2;
    const auto presetBoxWidth = juce::roundToInt (presetBoundsWidth * .27f);

    mPresetDisplay.setBounds (presetBounds.removeFromLeft (presetBoxWidth)
                                  .withY (presetDisplayY)
                                  .withHeight (presetDisplayHeight));
}

void PresetPanel::setupValentineInfoButton()
{
    mInfoButton.setImages (
        juce::Drawable::createFromImageData (BinaryData::totie_outline_svg,
                                             BinaryData::totie_outline_svgSize)
            .get(),
        juce::Drawable::createFromImageData (BinaryData::totie_pink_svg,
                                             BinaryData::totie_pink_svgSize)
            .get());
    addAndMakeVisible (mInfoButton);
}

void PresetPanel::setupBypassButton()
{
    mBypassButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                             tote_bag::colours::bypassGrey);
    mBypassButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                             tote_bag::colours::plainWhite);
    mBypassButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                             tote_bag::colours::bypassInGrey);
    mBypassButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                             tote_bag::colours::bypassInTextGrey);

    addAndMakeVisible (mBypassButton);
}

void PresetPanel::setupSaveAndLoadButtons()
{
    mSavePresetButton.setButtonText ("SAVE");
    mSavePresetButton.onClick = [this]() { presetManager.savePresetToFile(); };
    addAndMakeVisible (mSavePresetButton);

    mSavePresetButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                                 tote_bag::colours::plainWhite);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                                 tote_bag::colours::offWhite);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                                 tote_bag::colours::plainBlack);
    mSavePresetButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                                 tote_bag::colours::plainBlack);

    mLoadPresetButton.setButtonText ("LOAD");
    mLoadPresetButton.onClick = [this]() { presetManager.loadPresetFromFile(); };
    addAndMakeVisible (mLoadPresetButton);

    mLoadPresetButton.setColour (juce::TextButton::ColourIds::buttonColourId,
                                 tote_bag::colours::plainWhite);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::buttonOnColourId,
                                 tote_bag::colours::offWhite);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::textColourOffId,
                                 tote_bag::colours::plainBlack);
    mLoadPresetButton.setColour (juce::TextButton::ColourIds::textColourOnId,
                                 tote_bag::colours::plainBlack);
}

void PresetPanel::setupPresetIncrementButtons()
{
    mPreviousPreset.onClick = [this]() { presetManager.loadPreviousPreset(); };
    mPreviousPreset.setImages (
        juce::Drawable::createFromImageData (BinaryData::arrow_left_svg,
                                             BinaryData::arrow_left_svgSize)
            .get());
    addAndMakeVisible (mPreviousPreset);

    mNextPreset.onClick = [this]() { presetManager.loadNextPreset(); };
    mNextPreset.setImages (
        juce::Drawable::createFromImageData (BinaryData::arrow_right_svg,
                                             BinaryData::arrow_right_svgSize)
            .get());
    addAndMakeVisible (mNextPreset);
}

void PresetPanel::setupPresetDisplay()
{
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

    updatePresetDisplay();
}

void PresetPanel::handlePresetDisplaySelection()
{
    if (mPresetDisplay.getSelectedItemIndex() != -1)
    {
        presetManager.loadPreset (mPresetDisplay.getSelectedItemIndex());
    }
}

void PresetPanel::updatePresetDisplay()
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
