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
const juce::String& presetFileExtension()
{
    static const juce::String extension = ".tbp";
    return extension;
}

const juce::String& presetFileExtensionWildcard()
{
    static const juce::String wildcard = "*.tbp";
    return wildcard;
}
}

#if JUCE_WINDOWS
namespace
{
const juce::String& directorySeparator()
{
static const juce::String separator = "\\";
return separator;
}
}
#elif JUCE_MAC
namespace
{
const juce::String& directorySeparator()
{
static const juce::String separator = "/";
return separator;
}
}
#endif

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
    
    void savePreset(juce::File presetToSave);
    
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

    int findPresetIndex(const juce::String& presetName);
        
    juce::File presetDirectory;
    juce::String currentPresetName {"Untitled"};
    int mCurrentPresetIndex{0};
    juce::Array<juce::String> presetList;
    
    juce::AudioProcessor* processor;
    
    bool presetIsDirty = false;
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ToteBagPresetManager)
};
