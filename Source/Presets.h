#pragma once
#include <juce_core/juce_core.h>
#include <vector>

namespace pa
{
struct Preset
{
    juce::String name, instrument, style, character;
    float targetDb;        // nivel RMS objetivo de entrada (dBFS)
    float targetCentroid;  // brillo objetivo (Hz)
    float hpf;
    float lowGain, lowFreq;
    float midGain, midFreq, midQ;
    float presGain, presFreq;
    float airGain;
    float threshold, ratio, attack, release, makeup;
    float transient;       // -1..1
    float drive;           // 0..1
    float width;           // 0..2
    float output;          // dB
};

const std::vector<Preset>& getPresets();
const juce::StringArray& getInstruments();
const juce::StringArray& getStyles();
const juce::StringArray& getCharacters();
}
