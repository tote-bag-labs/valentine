// Tote Bag Labs 2023

#include "BinaryData.h"

#include <juce_gui_basics/juce_gui_basics.h>

#include <map>

#pragma once

namespace tote_bag
{

/** A class that holds our custom fonts.
 */
class FontHolder
{
    public:
    /** Gets a Font from the map, creating it if it doesn't exist.
     */
    juce::Font getFont(const juce::String& fontName);

    private:
    std::map<juce::String, std::unique_ptr<juce::Font>> fonts;
};

} // namespace tote_bag
