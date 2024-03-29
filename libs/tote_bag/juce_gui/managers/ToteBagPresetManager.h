/*
  ==============================================================================

    VPresetManager.h
    Created: 3 Jan 2020 1:11:35pm
    Author:  Jose Diaz

  ==============================================================================
*/

#pragma once

#include <juce_core/juce_core.h>

namespace
{
constexpr std::string_view presetFileExtension = ".tbp";
constexpr std::string_view presetFileExtensionWildcard = "*.tbp";
#if JUCE_WINDOWS
constexpr std::string_view directorySeparator = "\\";
#else
constexpr std::string_view directorySeparator = "/";
#endif
}

namespace juce
{
class AudioProcessor;
}

class ToteBagPresetManager
{
public:
    ToteBagPresetManager (juce::AudioProcessor* inProcessor);

    /** Returns the number of presets loaded to the local presets array */
    const int getNumberOfPresets();

    /** Sets all parameters to default and sets preset name to "Untitled"*/
    void createNewPreset();

    void savePreset (juce::File presetToSave);

    void loadPreset (juce::File presetToLoad);

    void loadPreset (int presetIndex);

    /** Returns the present name for a given index of the preset directory */
    const juce::String getPresetName (int inPresetIndex);
    /** Returns the name of the currently loaded preset */
    const juce::String getCurrentPresetName();

    int getCurrentPresetIndex();

    /** Returns the base Preset Directory */
    const juce::String getCurrentPresetDirectory();
    /** Allows caller to set the name of the currently loaded preset. used to facilitate state restore */
    void setLastChosenPresetName (juce::String newPresetName);

private:
    /** Iterates over the preset directory and adds the files to localPresets  */
    void updatePresetList();

    int findPresetIndex (const juce::String& presetName);

    juce::File presetDirectory;
    juce::String currentPresetName { "Untitled" };
    int mCurrentPresetIndex { 0 };
    juce::Array<juce::String> presetList;

    juce::AudioProcessor* processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToteBagPresetManager)
};
