cmake_minimum_required(VERSION 3.22)

# Runtime estatico: el usuario no necesita instalar Visual C++ Redistributable
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(PercussionAnalizer VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.6
    GIT_SHALLOW    TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_binary_data(PA_Assets SOURCES assets/logo.png)

juce_add_plugin(PercussionAnalizer
    COMPANY_NAME "ORYMY And YMY Records"
    PLUGIN_MANUFACTURER_CODE Ymyr
    PLUGIN_CODE Pana
    FORMATS VST3
    PRODUCT_NAME "PERCUSSION ANALIZER"
    VST3_CATEGORIES Fx Analyzer EQ Dynamics
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(PercussionAnalizer PRIVATE
    Source/Presets.cpp
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp)

target_compile_definitions(PercussionAnalizer PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_compile_options(PercussionAnalizer PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)

target_link_libraries(PercussionAnalizer
    PRIVATE
        PA_Assets
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags)
