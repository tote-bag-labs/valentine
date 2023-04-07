// Tote Bag Labs 2023

#include "tbl_font.h"

namespace tote_bag
{

juce::Font FontHolder::getFont(const juce::String& fontName)
{
    auto fontIt = fonts.find(fontName);
    if (fontIt == fonts.end())
    {
        int size = 0;
        const auto resource = BinaryData::getNamedResource (fontName.toRawUTF8(), size);

        // Make sure you've passed the correct font name to this function.
        // If you're sure you've done that, make sure you've added the font
        // to your assets dir and BinaryData files.
        jassert(resource != nullptr);

        auto typeface = (juce::Typeface::createSystemTypefaceFor (resource,
                                                                  static_cast<size_t>(size)));

        auto font = std::make_unique<juce::Font> (typeface);

        const auto[newFontIt, inserted] = fonts.insert({fontName, std::move(font)});
        if(!inserted)
        {
            jassertfalse;
            return juce::Font{};
        }

        fontIt = newFontIt;
    }

    return *fontIt->second;
}

} // namespace tote_bag
