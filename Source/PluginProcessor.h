#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Presets.h"
#include "Analyzer.h"

enum ParamIndex
{
    pAuto = 0, pAutoAmt, pTargetDb, pTargetCent,
    pHpf, pLowGain, pLowFreq, pMidGain, pMidFreq, pMidQ, pPresGain, pPresFreq, pAir,
    pThresh, pRatio, pAttack, pRelease, pMakeup, pTrans,
    pDrive, pWidth, pMix, pOutput,
    pCount
};

inline constexpr const char* kParamIds[pCount] = {
    "auto", "autoAmt", "targetDb", "targetCent",
    "hpf", "lowGain", "lowFreq", "midGain", "midFreq", "midQ", "presGain", "presFreq", "air",
    "thresh", "ratio", "attack", "release", "makeup", "trans",
    "drive", "width", "mix", "output"
};

namespace LearnMode { constexpr int continuous = 0, learning = 1, locked = 2; }

struct Band
{
    juce::dsp::IIR::Filter<float> f[2];
    float a = -1e9f, b = -1e9f, c = -1e9f;
    void invalidate() { a = b = c = -1e9f; }
};

class PercussionAnalizerAudioProcessor : public juce::AudioProcessor
{
public:
    PercussionAnalizerAudioProcessor();
    ~PercussionAnalizerAudioProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "PERCUSSION ANALIZER"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return (int) pa::getPresets().size(); }
    int getCurrentProgram() override { return currentPreset.load(); }
    void setCurrentProgram(int index) override { loadPreset(index); }
    const juce::String getProgramName(int index) override;
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    juce::AudioProcessorValueTreeState apvts;

    void loadPreset(int index);
    int  getCurrentPresetIndex() const { return currentPreset.load(); }
    void startLearning()   { learnRequested = true; }
    void startContinuous() { continuousRequested = true; }
    void loadSuggestedPreset();

    // ---- datos para la interfaz
    pa::SpectrumAnalyzer analyzer;
    std::atomic<float> vuIn { -90.f }, grDb { 0.f }, outPeak[2] { { -90.f }, { -90.f } };
    std::atomic<float> measuredCentroid { 0.f }, measuredLevel { -90.f };
    std::atomic<float> autoGainDb { 0.f }, autoTiltDb { 0.f }, learnProgress { 0.f };
    std::atomic<int>   learnState { LearnMode::continuous }, detectedInstrument { -1 };

private:
    void handleFrame(const pa::SpectrumAnalyzer::Frame& f);
    float readParam(int i) const { return params[i]->load(); }

    std::atomic<float>* params[pCount] {};
    std::atomic<int> currentPreset { 0 };
    std::atomic<bool> learnRequested { false }, continuousRequested { false };

    double sampleRate = 48000.0;

    // estado del analisis (hilo de audio)
    bool  avgInit = false;
    float levelAvg = -30.f, logCentAvg = std::log(2500.f), learnElapsed = 0.f;
    static constexpr float gateDb = -55.f, learnLength = 8.f;

    // cadena
    Band tiltLo, tiltHi, hpf, low, mid, pres, air;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> gainSm, tiltSm;
    juce::dsp::Limiter<float> limiter;
    juce::dsp::DryWetMixer<float> mixer;

    // compresor enlazado
    float grState = 0.f;
    // transient shaper
    float fastEnv = 0.f, slowEnv = 0.f;
    float lastOutGain = 1.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PercussionAnalizerAudioProcessor)
};
