/*
  ==============================================================================

    RadioButtonStateManager.cpp
    Created: 6 Nov 2022 11:22:41pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "RadioButtonGroupManager.h"

#include <juce_audio_processors/juce_audio_processors.h>

namespace tote_bag
{

RadioButtonGroupManager::RadioButtonGroupManager(juce::AudioParameterChoice& parameter, int groupId) :
    mParameter(parameter)
    ,mGroupId(groupId)
{
    mCurrentParameterIndex = mParameter.getIndex();
    
    startTimerHz(20);
}

RadioButtonGroupManager::~RadioButtonGroupManager()
{
    // button->onClick is dependent on this manager,
    // causing a crash if it is called after this has
    // been destroyed.
    for (auto button : mManagedButtons)
    {
        button->onClick = nullptr;
    }
    
    stopTimer();
}

void RadioButtonGroupManager::attach(juce::Button* button)
{
    const auto index = mNextButtonIndex;

    button->onClick = [this, button, index] {
        buttonOnClickCallback(button, index);
    };

    if (index == mCurrentParameterIndex)
    {
        button->setToggleState(true, juce::dontSendNotification);
    }

    button->setClickingTogglesState(true);
    button->setRadioGroupId(mGroupId);

    mManagedButtons.emplace_back(button);
    ++mNextButtonIndex;
}

void RadioButtonGroupManager::buttonOnClickCallback(juce::Button* button, int index)
{
    auto buttonOn = button->getToggleState();

    if (buttonOn)
    {
        mCurrentParameterIndex = index;

        const auto buttonVal = mParameter.convertTo0to1(index);

        mParameter.beginChangeGesture();
        mParameter.setValueNotifyingHost(buttonVal);
        mParameter.endChangeGesture();
    }
}

void RadioButtonGroupManager::updateGroupState()
{
    const auto parameterIndex = mParameter.getIndex();
    if(mCurrentParameterIndex != parameterIndex)
    {
        mManagedButtons[parameterIndex]->setToggleState(true,
                                                   juce::dontSendNotification);
        mCurrentParameterIndex = parameterIndex;
    }
}

void RadioButtonGroupManager::timerCallback()
{
    updateGroupState();
}

} // namespace tote_bag
