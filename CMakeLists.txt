# 3.24.1 is bundled in Visual Studio 2022 v17.4
# 3.24.1 is also bundled in CLion as of 2023
cmake_minimum_required(VERSION 3.24.1)

# Change this to the name of your plugin
# This cannot have spaces (but PRODUCT_NAME can)
set(PROJECT_NAME "Valentine")

# Set the plugin formats you'll be building here.
# Valid formats: AAX Unity VST AU AUv3 Standalone
set(FORMATS AU VST3 Standalone)

# This must be set before the project() call
# see: https://cmake.org/cmake/help/latest/variable/CMAKE_OSX_DEPLOYMENT_TARGET.html
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Support macOS down to High Sierra")

# Reads in VERSION file and sticks in it CURRENT_VERSION variable
# Be sure the file has no newlines
file(STRINGS VERSION CURRENT_VERSION)

# Find Git and if possible, get the current commit hash
find_package(Git QUIET)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short=7 HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_COMMIT_HASH "unknown")
endif()

# Configure a header file to pass the Git commit hash to the source code
configure_file(
  ${CMAKE_SOURCE_DIR}/version.h.in
  ${CMAKE_BINARY_DIR}/generated/version.h
)

# For simplicity, the name of the project is also the name of the target
project(${PROJECT_NAME} VERSION ${CURRENT_VERSION})

# By default we don't want Xcode schemes to be made for modules, etc
set(CMAKE_XCODE_GENERATE_SCHEME OFF)

# Adds all the module sources so they appear correctly in the IDE
# Must be set before JUCE is added as a sub-dir (or any targets are made)
# https://github.com/juce-framework/JUCE/commit/6b1b4cf7f6b1008db44411f2c8887d71a3348889
set_property(GLOBAL PROPERTY USE_FOLDERS YES)

# Create a /Modules directory in the IDE with the JUCE Module code
option(JUCE_ENABLE_MODULE_SOURCE_GROUPS "Show all module sources in IDE projects" ON)

# JUCE is setup as a submodule in the /JUCE folder
# Locally, you'll need to run `git submodule update --init --recursive` once
# and `git submodule update --remote --merge` to keep it up to date
# On Github Actions, it's managed by actions/checkout
add_subdirectory(libs/JUCE)

# Also using Foley's Finest meters
juce_add_module(libs/ff_meters)

# Check the readme at `docs/CMake API.md` in the JUCE repo for full config
juce_add_plugin("${PROJECT_NAME}"
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "Tote Bag Labs"
    BUNDLE_ID com.ToteBagLabs.Valentine
    # IS_SYNTH FALSE                       # Is this a synth or an effect?
    # NEEDS_MIDI_INPUT FALSE               # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT FALSE              # Does the plugin need midi output?
    # IS_MIDI_EFFECT FALSE                 # Is this plugin a MIDI effect?
    # EDITOR_WANTS_KEYBOARD_FOCUS FALSE    # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE # On MacOS, plugin will be copied to /Users/you/Library/Audio/Plug-Ins/
    PLUGIN_MANUFACTURER_CODE Valn               # This has to be one uppercase, rest lower for AU formats
    PLUGIN_CODE V001                            # A unique four-character plugin id with at least one upper-case character
    FORMATS "${FORMATS}"
    PRODUCT_NAME "${PROJECT_NAME}")        # The name of the final executable, which can differ from the target name

# C++20 please
target_compile_features("${PROJECT_NAME}" PRIVATE cxx_std_20)

# Manually list all .h and .cpp files for the plugin
set(SourceFiles
    src/PluginEditor.h
    src/PluginProcessor.h
    src/ValentineParameters.h
    src/PluginEditor.cpp
    src/PluginProcessor.cpp
    
    src/gui/panels/ValentineBottomRowPanel.h
    src/gui/panels/ValentineCenterPanel.h
    src/gui/panels/ValentineMainPanel.h
    src/gui/panels/ValentineTopRowPanel.h
    src/gui/panels/ValentineBottomRowPanel.cpp
    src/gui/panels/ValentineCenterPanel.cpp
    src/gui/panels/ValentineMainPanel.cpp
    src/gui/panels/ValentineTopRowPanel.cpp

    libs/tote_bag/dsp/AudioHelpers.h
    libs/tote_bag/dsp/CircularBuffer.h
    libs/tote_bag/dsp/Compressor.h
    libs/tote_bag/dsp/DigiDegraders.h
    libs/tote_bag/dsp/EnvelopeDetector.h
    libs/tote_bag/dsp/Saturation.h
    libs/tote_bag/dsp/ThiranAllpass.h
    libs/tote_bag/dsp/Compressor.cpp
    libs/tote_bag/dsp/DigiDegraders.cpp
    libs/tote_bag/dsp/EnvelopeDetector.cpp
    libs/tote_bag/dsp/Saturation.cpp
    
    libs/tote_bag/juce_gui/components/panels/InfoPanel.h
    libs/tote_bag/juce_gui/components/panels/PresetPanel.h
    libs/tote_bag/juce_gui/components/panels/VerticalMeterPanel.h
    libs/tote_bag/juce_gui/components/panels/InfoPanel.cpp
    libs/tote_bag/juce_gui/components/panels/PresetPanel.cpp
    libs/tote_bag/juce_gui/components/panels/VerticalMeterPanel.cpp

    libs/tote_bag/juce_gui/components/widgets/DrawableParameterButton.h
    libs/tote_bag/juce_gui/components/widgets/FlatTextButton.h
    libs/tote_bag/juce_gui/components/widgets/FlatTextChooser.h
    libs/tote_bag/juce_gui/components/widgets/LabelSlider.h
    libs/tote_bag/juce_gui/components/widgets/ParameterSlider.h
    libs/tote_bag/juce_gui/components/widgets/ParameterTextButton.h
    libs/tote_bag/juce_gui/components/widgets/tbl_ToggleButton.h
    libs/tote_bag/juce_gui/components/widgets/DrawableParameterButton.cpp
    libs/tote_bag/juce_gui/components/widgets/FlatTextButton.cpp
    libs/tote_bag/juce_gui/components/widgets/FlatTextChooser.cpp
    libs/tote_bag/juce_gui/components/widgets/LabelSlider.cpp
    libs/tote_bag/juce_gui/components/widgets/ParameterSlider.cpp
    libs/tote_bag/juce_gui/components/widgets/ParameterTextButton.cpp
    libs/tote_bag/juce_gui/components/widgets/tbl_ToggleButton.cpp

    libs/tote_bag/juce_gui/lookandfeel/LookAndFeel.h
    libs/tote_bag/juce_gui/lookandfeel/LookAndFeelConstants.h
    libs/tote_bag/juce_gui/lookandfeel/MeterLookAndFeelMethods.h
    libs/tote_bag/juce_gui/lookandfeel/LookAndFeel.cpp

    libs/tote_bag/juce_gui/managers/ToteBagPresetManager.h
    libs/tote_bag/juce_gui/managers/RadioButtonGroupManager.h
    libs/tote_bag/juce_gui/managers/ToteBagPresetManager.cpp
    libs/tote_bag/juce_gui/managers/RadioButtonGroupManager.cpp

    libs/tote_bag/juce_gui/utilities/GraphicsUtilities.h
    libs/tote_bag/juce_gui/utilities/GraphicsUtilities.cpp
    libs/tote_bag/juce_gui/utilities/tbl_font.h
    libs/tote_bag/juce_gui/utilities/tbl_font.cpp

    libs/tote_bag/utils/macros.hpp
    libs/tote_bag/utils/type_helpers.hpp
    libs/tote_bag/utils/tbl_math.hpp
    )

target_sources("${PROJECT_NAME}" PRIVATE ${SourceFiles})

# No, we don't want our source buried in extra nested folders
set_target_properties("${PROJECT_NAME}" PROPERTIES FOLDER "")

# The Xcode source tree should uhhh, still look like the source tree, yo
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES ${SourceFiles})

set(AssetFiles
    src/gui/assets/fonts/Roboto-Regular.ttf
    src/gui/assets/fonts/RobotoMono-Regular.ttf
    src/gui/assets/logos/val_totebag_logo.svg
    libs/tote_bag/assets/left_arrow.svg
    libs/tote_bag/assets/right_arrow.svg
    libs/tote_bag/assets/totie_pink.svg
   )

# Setup our binary data as a target
juce_add_binary_data(Assets SOURCES ${AssetFiles})

# Required for Linux happiness:
# See https://forum.juce.com/t/loading-pytorch-model-using-binarydata/39997/2
set_target_properties(Assets PROPERTIES POSITION_INDEPENDENT_CODE TRUE)

# This cleans up the folder organization on Xcode.
# It tucks the Plugin varieties into a "Targets" folder and generate an Xcode Scheme manually
# Xcode scheme generation is turned off globally to limit noise from other targets
# The non-hacky way of doing this is via the global PREDEFINED_TARGETS_FOLDER property
# However that doesn't seem to be working in Xcode
# Not all plugin types (au, vst) available on each build type (win, macos, linux)
foreach(target ${FORMATS} "All")
    if(TARGET ${PROJECT_NAME}_${target})
        set_target_properties(${PROJECT_NAME}_${target} PROPERTIES
            # Tuck the actual plugin targets into a folder where they won't bother us
            FOLDER "Targets"

            # MacOS only: Sets the default executable that Xcode will open on build
            # For this exact path to to work, manually build the AudioPluginHost.xcodeproj in the JUCE subdir
            XCODE_SCHEME_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/JUCE/extras/AudioPluginHost/Builds/MacOSX/build/Debug/AudioPluginHost.app"

            # Let us build the target in Xcode
            XCODE_GENERATE_SCHEME ON)
    endif()
endforeach()
set_target_properties(Assets PROPERTIES FOLDER "Targets")

# We'll need to link to these from our plugin as well as our tests
set(JUCE_DEPENDENCIES
    juce::juce_audio_utils
    juce::juce_dsp)

target_compile_definitions("${PROJECT_NAME}"
    PUBLIC
    # JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_ENABLE_GPL_MODE=1
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
    JUCE_MODAL_LOOPS_PERMITTED=1
    JUCE_ENABLE_LIVE_CONSTANT_EDITOR=0
    )

target_include_directories("${PROJECT_NAME}"
                            PRIVATE
                           "${CMAKE_CURRENT_SOURCE_DIR}/src"
                           "${CMAKE_CURRENT_SOURCE_DIR}/libs"
                           "${CMAKE_BINARY_DIR}")

target_link_libraries("${PROJECT_NAME}"
    PRIVATE
    Assets
    ${JUCE_DEPENDENCIES}
    ff_meters
    PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags)


# Required for ctest (which is just easier for cross-platform CI)
# include(CTest) does this too, but adds tons of targets we don't want
# See: https://github.com/catchorg/Catch2/issues/2026
# You could forgo ctest and call ./tests directly from the build dir
enable_testing()

# "GLOBS ARE BAD" is brittle and silly UX, sorry CMake!
# CONFIGURE_DEPENDS / Clion's CMake integration makes globbing absolutely fine
file(GLOB_RECURSE TestFiles CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests PREFIX "" FILES ${TestFiles})

# Use Catch2 v3 on the devel branch
Include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
    GIT_TAG v3.4.0)
FetchContent_MakeAvailable(Catch2) # find_package equivalent

# Setup the test executable, again C++ 20 please
add_executable(Tests ${TestFiles})
target_compile_features(Tests PRIVATE cxx_std_20)

# Our test executable also wants to know about our plugin code...
target_include_directories(Tests
                           PRIVATE
                          "${CMAKE_CURRENT_SOURCE_DIR}/src"
                          "${CMAKE_CURRENT_SOURCE_DIR}/libs")

target_link_libraries(Tests PRIVATE Catch2::Catch2WithMain "${PROJECT_NAME}")

# We can't link again to the shared juce target without ODR violations
# However, we can steal its include dirs and compile definitions to use in tests!
# https://forum.juce.com/t/windows-linker-issue-on-develop/55524/2
target_compile_definitions(Tests PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME},COMPILE_DEFINITIONS>)
target_include_directories(Tests PRIVATE $<TARGET_PROPERTY:${PROJECT_NAME},INCLUDE_DIRECTORIES>)

# Make an Xcode Scheme for the test executable so we can run tests in the IDE
set_target_properties(Tests PROPERTIES XCODE_GENERATE_SCHEME ON)

# Organize the test source in the tests/ folder in the IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/tests PREFIX "" FILES ${TestFiles})

# Load and use the .cmake file provided by Catch2
# https://github.com/catchorg/Catch2/blob/devel/docs/cmake-integration.md
# We have to manually provide the source directory here for now
include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)
catch_discover_tests(Tests)

# Color our warnings and errors
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
   add_compile_options (-fdiagnostics-color=always)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
   add_compile_options (-fcolor-diagnostics)
endif ()
