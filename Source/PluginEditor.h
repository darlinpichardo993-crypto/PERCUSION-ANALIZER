#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

using Proc = PercussionAnalizerAudioProcessor;

inline juce::Font mkFont(float h, bool bold = false)
{
    return juce::Font(juce::FontOptions(h, bold ? juce::Font::bold : juce::Font::plain));
}

//==============================================================================
class RealLookAndFeel : public juce::LookAndFeel_V4
{
public:
    RealLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider&) override;
    void drawButtonBackground(juce::Graphics&, juce::Button&, const juce::Colour&, bool highlighted, bool down) override;
};

//==============================================================================
class VuMeter : public juce::Component
{
public:
    VuMeter(const juce::String& title, float minDb, float maxDb, int majors, float redStart);
    void setValue(float db) { target = db; }
    void tick();
    void paint(juce::Graphics&) override;
private:
    juce::String title; float minDb, maxDb; int majors; float redStart;
    float target = -90.f, value = -90.f;
};

class SpectrumView : public juce::Component
{
public:
    explicit SpectrumView(Proc& p) : proc(p) { disp.fill(-100.f); }
    void tick();
    void paint(juce::Graphics&) override;
private:
    float freqToX(double f, float w) const;
    Proc& proc;
    std::array<float, pa::SpectrumAnalyzer::numBands> disp;
};

class EqCurveView : public juce::Component
{
public:
    explicit EqCurveView(Proc& p) : proc(p) {}
    void paint(juce::Graphics&) override;
private:
    Proc& proc;
};

class LedMeter : public juce::Component
{
public:
    explicit LedMeter(Proc& p) : proc(p) {}
    void tick();
    void paint(juce::Graphics&) override;
private:
    Proc& proc;
    float val[2] { -90.f, -90.f };
};

//==============================================================================
class PercussionAnalizerAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit PercussionAnalizerAudioProcessorEditor(Proc&);
    ~PercussionAnalizerAudioProcessorEditor() override;
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::TextBoxBelow };
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> att;
    };

    Knob* addKnob(int paramIndex, const juce::String& name, juce::Colour cap, juce::Colour labelColour);
    void layoutGrid(juce::Rectangle<int> area, const std::vector<Knob*>& ks, int cols);
    void computeLayout();
    void renderBackground();
    void timerCallback() override;
    void showPresetMenu();
    void updatePresetText();
    void stepPreset(int delta);

    Proc& proc;
    RealLookAndFeel laf;

    std::vector<std::unique_ptr<Knob>> knobs;
    Knob *kTarget {}, *kCent {}, *kAutoAmt {};
    Knob *kHpf {}, *kLowG {}, *kLowF {}, *kMidG {}, *kMidF {}, *kMidQ {}, *kPresG {}, *kPresF {}, *kAir {};
    Knob *kThr {}, *kRatio {}, *kAtt {}, *kRel {}, *kMakeup {}, *kTrans {};
    Knob *kDrive {}, *kWidth {}, *kMix {}, *kOut {};

    VuMeter vuLevel { "NIVEL", -36.f, 0.f, 6, 0.83f };
    VuMeter vuGr    { "GR", 0.f, 24.f, 6, 2.f };
    SpectrumView spectrum;
    EqCurveView eqView;
    LedMeter leds;

    juce::TextButton autoButton { "AUTO MIX" }, learnButton { "APRENDER" }, contButton { "CONTINUO" }, suggestButton { "SUGERIR PRESET" };
    juce::TextButton prevButton { "<" }, nextButton { ">" }, presetButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoAtt;
    juce::Label detectedLabel, stateLabel, corrLabel;

    juce::Image background, logoImage;
    juce::Rectangle<int> woodL, woodR, header, panelA, panelB, panelC, panelD;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PercussionAnalizerAudioProcessorEditor)
};
