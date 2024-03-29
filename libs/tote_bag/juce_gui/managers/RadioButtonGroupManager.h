/*
  ==============================================================================

    RadioButtonStateManager.h
    Created: 6 Nov 2022 11:22:41pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace juce
{
class AudioParameterChoice;
} // namespace juce

namespace tote_bag
{

/** Manages button and parameter state for a radio button group.
 */
class RadioButtonGroupManager : public juce::Timer
{
public:
    RadioButtonGroupManager (juce::AudioParameterChoice& parameter, int groupId);

    ~RadioButtonGroupManager() override;

    void timerCallback() override;

    /** Attaches a radio button to be managed by this class.
        buttons managed by this class will be indexed in the order they
        were attached.
     */
    void attach (juce::Button* button);

private:
    /** Called by a managed button's onClick() method to change button and
        parameter state.
     */
    void buttonOnClickCallback (juce::Button* button, size_t index);

    /** Called periodically to check if param value has changed in a way not driven
        by a click on the button itself. e.g. via automation.
     */
    void updateGroupState();

    juce::AudioParameterChoice& mParameter;
    std::vector<juce::Button*> mManagedButtons;
    int mGroupId { 0 };
    size_t mCurrentParameterIndex { 0 };
    size_t mNextButtonIndex { 0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RadioButtonGroupManager)
};
} // namespace tote_bag

#pragma once
