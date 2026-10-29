#include "Presets.h"

namespace pa
{
namespace
{
struct Base { const char* name; float target, cent, hpf, lowF, lowG, midF, midG, midQ, presF, presG, air, thr, ratio, att, rel, makeup, trans, drive, width; };
struct StyleD { const char* name; float target, centMul, low, mid, pres, air, ratio, trans, drive, width; };
struct CharD  { const char* name; float target, centMul, low, mid, pres, air, ratio, thr, trans, drive, width, relMul; };

const Base bases[] = {
    { "Güira",           -20, 5200, 450, 300, -2.0f, 2500, -1.5f, 1.0f, 6500, 2.0f, 2.5f, -22, 3.0f,  8,  90, 2.0f, 0.20f, 0.10f, 1.20f },
    { "Tambora",         -16,  900,  50, 100,  3.0f,  400, -2.0f, 1.2f, 4500, 2.0f, 0.5f, -18, 3.5f, 15, 120, 2.0f, 0.35f, 0.20f, 1.00f },
    { "Conga",           -18, 1500,  70, 140,  2.0f,  600, -2.0f, 1.1f, 3500, 2.5f, 1.0f, -20, 3.0f, 12, 100, 2.0f, 0.30f, 0.15f, 1.10f },
    { "Bongó",           -19, 2300, 120, 200,  0.0f,  900, -1.5f, 1.2f, 4500, 3.0f, 1.5f, -21, 2.8f,  6,  80, 2.0f, 0.35f, 0.10f, 1.10f },
    { "Timbal",          -19, 3200, 150, 250, -1.0f, 1200, -1.0f, 1.4f, 5500, 3.0f, 2.0f, -22, 3.2f,  5,  70, 2.0f, 0.45f, 0.15f, 1.05f },
    { "Percusión",       -19, 2600,  90, 160,  0.5f,  800, -1.5f, 1.0f, 4000, 2.0f, 1.5f, -21, 3.0f, 10,  90, 2.0f, 0.25f, 0.10f, 1.10f },
    { "Bus Percusión",   -17, 2400,  40, 120,  1.0f,  700, -1.0f, 0.8f, 3500, 1.5f, 1.5f, -18, 2.0f, 25, 150, 1.5f, 0.00f, 0.10f, 1.10f },
};

const StyleD styles[] = {
    { "Merengue",             1.0f, 1.05f,  0.0f,  0.0f,  1.0f,  0.5f,  0.3f,  0.10f, 0.05f,  0.00f },
    { "Merengue Típico",      0.0f, 0.92f,  1.0f,  1.0f, -0.5f, -0.5f,  0.0f,  0.15f, 0.10f,  0.00f },
    { "Bachata Moderna",     -1.5f, 1.00f,  0.0f, -0.5f,  1.0f,  1.5f, -0.3f, -0.10f, 0.00f,  0.20f },
    { "Bachata Tradicional", -1.0f, 0.90f,  0.5f,  0.5f, -0.5f, -0.5f, -0.2f,  0.00f, 0.05f,  0.00f },
    { "Bachata Urbana",      -0.5f, 1.05f,  1.0f, -1.0f,  1.5f,  1.0f,  0.2f,  0.05f, 0.10f,  0.10f },
    { "Mambo",                1.5f, 1.08f,  0.0f,  0.0f,  1.5f,  1.0f,  0.5f,  0.20f, 0.15f,  0.00f },
    { "En Vivo",             -2.0f, 0.97f, -0.5f,  0.5f,  0.0f,  0.0f,  0.5f, -0.10f, 0.08f, -0.10f },
    { "Estudio",              0.0f, 1.00f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.00f, 0.00f,  0.00f },
};

const CharD chars[] = {
    { "Natural",   0.0f, 1.00f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.0f,  0.00f, 0.00f,  0.00f, 1.00f },
    { "Brillante", 0.0f, 1.15f,  0.0f, -0.5f,  2.0f,  2.5f,  0.0f,  0.0f,  0.10f, 0.00f,  0.00f, 1.00f },
    { "Cálido",    0.0f, 0.85f,  1.5f,  0.5f, -1.5f, -2.0f,  0.0f,  0.0f,  0.00f, 0.10f,  0.00f, 1.00f },
    { "Seco",      0.0f, 1.00f, -1.0f,  0.0f,  0.0f,  0.0f,  0.3f,  1.0f,  0.30f, 0.00f, -0.10f, 0.75f },
    { "Pegao",     1.0f, 1.00f,  0.0f,  1.5f,  1.0f,  0.0f,  0.8f, -1.5f,  0.15f, 0.10f,  0.00f, 1.00f },
    { "Maco",      1.5f, 1.05f,  1.5f,  0.0f,  1.5f,  0.0f,  1.2f, -2.5f,  0.10f, 0.35f,  0.00f, 1.00f },
    { "Majao",     2.0f, 1.00f,  1.0f,  1.0f,  0.0f,  0.0f,  2.2f, -4.0f, -0.20f, 0.25f,  0.00f, 0.70f },
    { "Aireado",   0.0f, 1.10f, -1.0f,  0.0f,  1.0f,  3.5f, -0.3f,  0.0f,  0.00f, 0.00f,  0.30f, 1.00f },
};

template <typename T, size_t N> constexpr int count(const T (&)[N]) { return (int) N; }

std::vector<Preset> build()
{
    std::vector<Preset> out;
    for (const auto& b : bases)
        for (const auto& s : styles)
            for (const auto& c : chars)
            {
                Preset p;
                p.instrument = juce::String::fromUTF8(b.name);
                p.style      = juce::String::fromUTF8(s.name);
                p.character  = juce::String::fromUTF8(c.name);
                p.name       = p.instrument + " - " + p.style + " - " + p.character;
                p.targetDb       = juce::jlimit(-36.f, -6.f, b.target + s.target + c.target);
                p.targetCentroid = juce::jlimit(400.f, 9000.f, b.cent * s.centMul * c.centMul);
                p.hpf      = b.hpf;
                p.lowFreq  = b.lowF;
                p.lowGain  = juce::jlimit(-12.f, 12.f, b.lowG + s.low + c.low);
                p.midFreq  = b.midF;
                p.midQ     = b.midQ;
                p.midGain  = juce::jlimit(-12.f, 12.f, b.midG + s.mid + c.mid);
                p.presFreq = b.presF;
                p.presGain = juce::jlimit(-12.f, 12.f, b.presG + s.pres + c.pres);
                p.airGain  = juce::jlimit(-12.f, 12.f, b.air + s.air + c.air);
                p.threshold = juce::jlimit(-48.f, 0.f, b.thr + c.thr);
                p.ratio     = juce::jlimit(1.f, 12.f, b.ratio + s.ratio + c.ratio);
                p.attack    = b.att;
                p.release   = juce::jlimit(10.f, 800.f, b.rel * c.relMul);
                p.makeup    = b.makeup;
                p.transient = juce::jlimit(-1.f, 1.f, b.trans + s.trans + c.trans);
                p.drive     = juce::jlimit(0.f, 1.f, b.drive + s.drive + c.drive);
                p.width     = juce::jlimit(0.5f, 1.6f, b.width + s.width + c.width);
                p.output    = 0.f;
                out.push_back(p);
            }
    return out;
}
} // namespace

const std::vector<Preset>& getPresets() { static const std::vector<Preset> list = build(); return list; }

const juce::StringArray& getInstruments()
{
    static juce::StringArray a = [] { juce::StringArray r; for (const auto& b : bases) r.add(juce::String::fromUTF8(b.name)); return r; }();
    return a;
}
const juce::StringArray& getStyles()
{
    static juce::StringArray a = [] { juce::StringArray r; for (const auto& s : styles) r.add(juce::String::fromUTF8(s.name)); return r; }();
    return a;
}
const juce::StringArray& getCharacters()
{
    static juce::StringArray a = [] { juce::StringArray r; for (const auto& c : chars) r.add(juce::String::fromUTF8(c.name)); return r; }();
    return a;
}
}
