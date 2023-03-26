/*
  ==============================================================================

    VPresetManager.cpp
    Created: 3 Jan 2020 1:11:35pm
    Author:  Jose Diaz

  ==============================================================================
*/

#include "ToteBagPresetManager.h"

#include <juce_audio_processors/juce_audio_processors.h>

ToteBagPresetManager::ToteBagPresetManager (juce::AudioProcessor* inProcessor)
    : processor (inProcessor)
{
    presetDirectory = juce::File ((juce::File::getSpecialLocation (juce::File::userDocumentsDirectory)).getFullPathName()
                                  + static_cast<std::string> (directorySeparator) + "Tote Bag"
                                  + static_cast<std::string> (directorySeparator) + processor->getName());

    // checks if preset directory is there—if not, it creates the directory
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();

    updatePresetList();
}

void ToteBagPresetManager::createNewPreset()
{
    auto& parameters = processor->getParameters();

    for (int i = 0; i < parameters.size(); i++)
    {
        auto parameter = (juce::AudioProcessorParameterWithID*) parameters.getUnchecked (i);

        const float defaultValue = parameter->getDefaultValue();

        parameter->setValueNotifyingHost (defaultValue);
    }

    currentPresetName = "Untitled";
}

void ToteBagPresetManager::savePreset (juce::File presetFile)
{
    // check if the file passed in actually exists. if not, create it
    if (!presetFile.exists())
        presetFile.create();
    else
        presetFile.deleteFile();

    // update preset name for GUI display
    currentPresetName = presetFile.getFileNameWithoutExtension();

    // allocate memory on the stack, and fill it with our preset data
    juce::MemoryBlock destinationData;
    processor->getStateInformation (destinationData);

    // write the preset data to file
    presetFile.appendData (destinationData.getData(), destinationData.getSize());

    updatePresetList();
}

void ToteBagPresetManager::loadPreset (juce::File presetToLoad)
{
    // if preset successfully loads, save its name and index
    juce::MemoryBlock presetBinary;
    if (presetToLoad.loadFileAsData (presetBinary))
    {
        currentPresetName = presetToLoad.getFileNameWithoutExtension();

        // current preset name is set my set State Info
        processor->setStateInformation (presetBinary.getData(), static_cast<int> (presetBinary.getSize()));
    }
}

void ToteBagPresetManager::loadPreset (int index)
{
    auto presetToLoad = juce::File (presetDirectory.getFullPathName()
                                    + static_cast<std::string> (directorySeparator)
                                    + getPresetName (index)
                                    + static_cast<std::string> (presetFileExtension));
    loadPreset (presetToLoad);
}

const juce::String ToteBagPresetManager::getPresetName (int inPresetIndex)
{
    return presetList[inPresetIndex];
}

const juce::String ToteBagPresetManager::getCurrentPresetName()
{
    return currentPresetName;
}

int ToteBagPresetManager::getCurrentPresetIndex()
{
    return mCurrentPresetIndex;
}

const int ToteBagPresetManager::getNumberOfPresets()
{
    return presetList.size();
}

const juce::String ToteBagPresetManager::getCurrentPresetDirectory()
{
    return presetDirectory.getFullPathName();
}

void ToteBagPresetManager::setLastChosenPresetName (juce::String newPresetName)
{
    currentPresetName = newPresetName;
    mCurrentPresetIndex = findPresetIndex (currentPresetName);
}

void ToteBagPresetManager::updatePresetList()
{
    presetList.clear();

    auto directoryIterator = juce::RangedDirectoryIterator (juce::File (presetDirectory),
                                                            false,
                                                            static_cast<std::string> (presetFileExtensionWildcard),
                                                            juce::File::TypesOfFileToFind::findFiles);

    for (auto entry : directoryIterator)
        presetList.add (entry.getFile().getFileNameWithoutExtension());
}

int ToteBagPresetManager::findPresetIndex (const juce::String& presetName)
{
    const auto foundPreset = std::find (presetList.begin(),
                                        presetList.end(),
                                        presetName);

    if (foundPreset != presetList.end())
    {
        return static_cast<int> (foundPreset - presetList.begin());
    }

    return 0;
}
