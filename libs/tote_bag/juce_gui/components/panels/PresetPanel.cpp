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
constexpr auto kTotieHeight = 135.51;
constexpr auto kTotieHWRatio = kTotieHeight / kTotieWidth;

constexpr auto kBorderThickness = 1;
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
    g.drawRect (getBounds(), detail::kBorderThickness);
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

    auto presetBounds = area.reduced (detail::kBorderThickness);

    const auto presetBoundsWidth = presetBounds.getWidth();
    const auto presetBoundsHeight = presetBounds.getHeight();
    const auto presetBoundsCentreY = presetBounds.getCentreY();

    const auto leftInfoButtonGapWidth = juce::roundToInt (presetBoundsWidth * .026f);

    const auto infoButtonBypassGapWidth = juce::roundToInt (presetBoundsWidth * 0.0027f);
    const auto bypassButtonWidth = juce::roundToInt (presetBoundsWidth * .134f);
    const auto bypassSaveGapWidth = juce::roundToInt (presetBoundsWidth * .0825f);
    const auto saveLoadButtonWidth = juce::roundToInt (presetBoundsWidth * .157f);
    const auto saveLoadGapWidth = juce::roundToInt (presetBoundsWidth * .007f);
    const auto loadPrevGapWidth = juce::roundToInt (presetBoundsWidth * .016f);
    const auto prevNextButtonWidth = juce::roundToInt (presetBoundsWidth * .024f);
    const auto prevNextGapWidth = juce::roundToInt (presetBoundsWidth * .0117f);
    const auto presetBoxWidth = juce::roundToInt (presetBoundsWidth * .27f);

    presetBounds.removeFromLeft (leftInfoButtonGapWidth);

    const auto infoButtonWidth = juce::roundToInt (presetBoundsWidth * 0.05f);
    const auto infoButtonHeight =
        juce::roundToInt (infoButtonWidth * detail::kTotieHWRatio);
    const auto infoButtonY = presetBoundsCentreY - infoButtonHeight / 2;
    mInfoButton.setBounds (presetBounds.removeFromLeft (infoButtonWidth)
                               .withY (infoButtonY)
                               .withHeight (infoButtonHeight));

    presetBounds.removeFromLeft (infoButtonBypassGapWidth);

    const auto bypassSaveLoadHeight = juce::roundToInt (presetBoundsHeight * .8f);
    const auto bypassSaveLoadY = presetBoundsCentreY - bypassSaveLoadHeight / 2;
    mBypassButton.setBounds (presetBounds.removeFromLeft (bypassButtonWidth)
                                 .withY (bypassSaveLoadY)
                                 .withHeight (bypassSaveLoadHeight));

    presetBounds.removeFromLeft (bypassSaveGapWidth);

    mSavePresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth)
                                     .withY (bypassSaveLoadY)
                                     .withHeight (bypassSaveLoadHeight));

    presetBounds.removeFromLeft (saveLoadGapWidth);

    mLoadPresetButton.setBounds (presetBounds.removeFromLeft (saveLoadButtonWidth)
                                     .withY (bypassSaveLoadY)
                                     .withHeight (bypassSaveLoadHeight));

    presetBounds.removeFromLeft (loadPrevGapWidth);

    const auto prevNextButtonHeight = juce::roundToInt (presetBoundsHeight * .2f);
    const auto prevNextButtonY = presetBoundsCentreY - prevNextButtonHeight / 2;
    mPreviousPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth)
                                   .withY (prevNextButtonY)
                                   .withHeight (prevNextButtonHeight));

    presetBounds.removeFromLeft (prevNextGapWidth);

    mNextPreset.setBounds (presetBounds.removeFromLeft (prevNextButtonWidth)
                               .withY (prevNextButtonY)
                               .withHeight (prevNextButtonHeight));

    presetBounds.removeFromLeft (loadPrevGapWidth);

    const auto presetDisplayHeight = juce::roundToInt (presetBoundsHeight * .85f);
    const auto presetDisplayY = presetBoundsCentreY - presetDisplayHeight / 2;
    mPresetDisplay.setBounds (presetBounds.removeFromLeft (presetBoxWidth)
                                  .withY (presetDisplayY)
                                  .withHeight (presetDisplayHeight));
}

void PresetPanel::setupValentineInfoButton()
{
    mInfoButton.setImages (
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
    mSavePresetButton.onClick = [this]() { presetManager.savePreset(); };
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
    mLoadPresetButton.onClick = [this]() { presetManager.loadPreset(); };
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
