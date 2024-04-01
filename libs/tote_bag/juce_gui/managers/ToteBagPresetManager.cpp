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
    presetDirectory = juce::File (
        (juce::File::getSpecialLocation (juce::File::userDocumentsDirectory))
            .getFullPathName()
        + static_cast<std::string> (directorySeparator) + "Tote Bag"
        + static_cast<std::string> (directorySeparator) + processor->getName());

    // checks if preset directory is there—if not, it creates the directory
    if (!presetDirectory.exists())
        presetDirectory.createDirectory();

    updatePresetList();
}

void ToteBagPresetManager::savePresetToFile()
{
    const juce::String currentPresetPath = presetDirectory.getFullPathName()
                                           + static_cast<std::string> (directorySeparator)
                                           + currentPresetName;

    juce::FileChooser chooser ("Save a file: ",
                               juce::File (currentPresetPath),
                               static_cast<std::string> (presetFileExtensionWildcard));

    if (chooser.browseForFileToSave (true))
    {
        savePreset (chooser.getResult());
    }
}

void ToteBagPresetManager::loadPresetFromFile()
{
    const juce::String currentPresetDirectory = presetDirectory.getFullPathName();

    if (currentPresetDirectory.isNotEmpty())
    {
        juce::FileChooser chooser (
            "Load a file: ",
            juce::File (currentPresetDirectory),
            static_cast<std::string> (presetFileExtensionWildcard));

        if (chooser.browseForFileToOpen())
        {
            loadPreset (chooser.getResult());
        }
    }
}

void ToteBagPresetManager::loadPreset (juce::File presetToLoad)
{
    // if preset successfully loads, save its name and index
    juce::MemoryBlock presetBinary;
    if (presetToLoad.loadFileAsData (presetBinary))
    {
        currentPresetName = presetToLoad.getFileNameWithoutExtension();

        // current preset name is set my set State Info
        processor->setStateInformation (presetBinary.getData(),
                                        static_cast<int> (presetBinary.getSize()));
    }
}

void ToteBagPresetManager::loadPreset (int index)
{
    auto presetToLoad = juce::File (
        presetDirectory.getFullPathName() + static_cast<std::string> (directorySeparator)
        + getPresetName (index) + static_cast<std::string> (presetFileExtension));
    loadPreset (presetToLoad);
}

void ToteBagPresetManager::loadNextPreset()
{
    auto newPresetIndex = mCurrentPresetIndex + 1;
    if (newPresetIndex > presetList.size() - 1)
    {
        newPresetIndex = 0;
    }
    loadPreset (newPresetIndex);
}

void ToteBagPresetManager::loadPreviousPreset()
{
    auto newPresetIndex = mCurrentPresetIndex - 1;
    if (newPresetIndex < 0)
    {
        newPresetIndex = presetList.size() - 1;
    }
    loadPreset (newPresetIndex);
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

void ToteBagPresetManager::setLastChosenPresetName (juce::String newPresetName)
{
    currentPresetName = newPresetName;
    mCurrentPresetIndex = findPresetIndex (currentPresetName);
}

void ToteBagPresetManager::createDefaultPreset()
{
    auto& parameters = processor->getParameters();

    for (int i = 0; i < parameters.size(); i++)
    {
        auto parameter =
            (juce::AudioProcessorParameterWithID*) parameters.getUnchecked (i);

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

    if (onPresetSaved)
    {
        onPresetSaved();
    }
}

void ToteBagPresetManager::updatePresetList()
{
    presetList.clear();

    auto directoryIterator = juce::RangedDirectoryIterator (
        juce::File (presetDirectory),
        false,
        static_cast<std::string> (presetFileExtensionWildcard),
        juce::File::TypesOfFileToFind::findFiles);

    for (auto entry : directoryIterator)
        presetList.add (entry.getFile().getFileNameWithoutExtension());
}

int ToteBagPresetManager::findPresetIndex (const juce::String& presetName)
{
    const auto foundPreset = std::find (presetList.begin(), presetList.end(), presetName);

    if (foundPreset != presetList.end())
    {
        return static_cast<int> (foundPreset - presetList.begin());
    }

    return 0;
}

void ToteBagPresetManager::setPresetSavedCallback (PresetSavedCallback callback)
{
    onPresetSaved = callback;
}
