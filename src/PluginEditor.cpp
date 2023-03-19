/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "PluginProcessor.h"
#include "ValentineParameters.h"
//==============================================================================
ValentineAudioProcessorEditor::ValentineAudioProcessorEditor (ValentineAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processor (p)
{
    addAndMakeVisible (mainPanel);

    const auto savedWidth = audioProcessor.getSavedGUIwidth();
    const auto w = savedWidth != 0 ? savedWidth : startingWidth;

    setResizable (true, true);
    setResizeLimits (minimumWidth,
                     juce::roundToInt (minimumWidth / ratio),
                     maximumWidth,
                     juce::roundToInt (maximumWidth / ratio));
    getConstrainer()->setFixedAspectRatio (ratio);

    setSize (w, juce::roundToInt (w / ratio));
}

ValentineAudioProcessorEditor::~ValentineAudioProcessorEditor()
{
    processor.saveGUIwidth (getWidth());
}

//==============================================================================
void ValentineAudioProcessorEditor::paint (juce::Graphics& g)
{
}

void ValentineAudioProcessorEditor::resized()
{
    mainPanel.setBounds (getLocalBounds());
}
