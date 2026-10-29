#include "PluginProcessor.h"
#include "PluginEditor.h"

using Coeffs = juce::dsp::IIR::Coefficients<float>;

namespace
{
template <typename MakeFn>
void updateBand(Band& band, float a, float b, float c, MakeFn&& make)
{
    if (a == band.a && b == band.b && c == band.c) return;
    band.a = a; band.b = b; band.c = c;
    auto coeffs = make();
    for (auto& f : band.f) *f.coefficients = *coeffs;
}

void runBand(Band& band, juce::dsp::AudioBlock<float>& block, int numCh)
{
    for (int c = 0; c < numCh; ++c)
    {
        auto ch = block.getSingleChannelBlock((size_t) c);
        juce::dsp::ProcessContextReplacing<float> ctx(ch);
        band.f[c].process(ctx);
    }
}

int classify(float centroid)
{
    if (centroid < 1100.f) return 1; // Tambora
    if (centroid < 1900.f) return 2; // Conga
    if (centroid < 2700.f) return 3; // Bongo
    if (centroid < 3800.f) return 4; // Timbal
    return 0;                        // Guira
}
}

juce::AudioProcessorValueTreeState::ParameterLayout PercussionAnalizerAudioProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout layout;

    auto parse = [](const String& t) { float v = t.getFloatValue(); if (t.containsIgnoreCase("k")) v *= 1000.f; return v; };

    auto add = [&](int idx, const char* name, NormalisableRange<float> r, float def, std::function<String(float, int)> fmt)
    {
        layout.add(std::make_unique<AudioParameterFloat>(ParameterID { kParamIds[idx], 1 }, name, r, def,
                   AudioParameterFloatAttributes().withStringFromValueFunction(fmt).withValueFromStringFunction(parse)));
    };

    auto fDb  = [](float v, int) { return String(v, 1) + " dB"; };
    auto fHz  = [](float v, int) { return v >= 1000.f ? String(v / 1000.f, 2) + " kHz" : String((int) v) + " Hz"; };
    auto fMs  = [](float v, int) { return String(v, 1) + " ms"; };
    auto fPct = [](float v, int) { return String((int) std::round(v * 100.f)) + " %"; };

    layout.add(std::make_unique<AudioParameterBool>(ParameterID { kParamIds[pAuto], 1 }, "Auto Mix", true));
    add(pAutoAmt,   "Auto Cantidad",   { 0.f, 100.f, 1.f }, 100.f, [](float v, int) { return String((int) v) + " %"; });
    add(pTargetDb,  "Nivel Objetivo",  { -36.f, -6.f, 0.1f }, -19.f, fDb);
    add(pTargetCent,"Brillo Objetivo", { 400.f, 9000.f, 1.f, 0.4f }, 2600.f, fHz);
    add(pHpf,       "Filtro Graves",   { 20.f, 600.f, 1.f, 0.4f }, 90.f, fHz);
    add(pLowGain,   "Low Gain",        { -12.f, 12.f, 0.1f }, 0.f, fDb);
    add(pLowFreq,   "Low Freq",        { 40.f, 400.f, 1.f, 0.5f }, 160.f, fHz);
    add(pMidGain,   "Mid Gain",        { -12.f, 12.f, 0.1f }, 0.f, fDb);
    add(pMidFreq,   "Mid Freq",        { 150.f, 5000.f, 1.f, 0.4f }, 800.f, fHz);
    add(pMidQ,      "Mid Q",           { 0.3f, 4.f, 0.01f }, 1.f, [](float v, int) { return String(v, 2); });
    add(pPresGain,  "Presencia Gain",  { -12.f, 12.f, 0.1f }, 0.f, fDb);
    add(pPresFreq,  "Presencia Freq",  { 2000.f, 12000.f, 1.f, 0.5f }, 4500.f, fHz);
    add(pAir,       "Aire",            { -12.f, 12.f, 0.1f }, 0.f, fDb);
    add(pThresh,    "Umbral",          { -48.f, 0.f, 0.1f }, -21.f, fDb);
    add(pRatio,     "Ratio",           { 1.f, 20.f, 0.1f, 0.5f }, 3.f, [](float v, int) { return String(v, 1) + ":1"; });
    add(pAttack,    "Ataque",          { 0.1f, 100.f, 0.1f, 0.4f }, 10.f, fMs);
    add(pRelease,   "Release",         { 10.f, 800.f, 1.f, 0.5f }, 90.f, fMs);
    add(pMakeup,    "Makeup",          { 0.f, 18.f, 0.1f }, 2.f, fDb);
    add(pTrans,     "Transientes",     { -1.f, 1.f, 0.01f }, 0.f, fPct);
    add(pDrive,     "Calor",           { 0.f, 1.f, 0.01f }, 0.1f, fPct);
    add(pWidth,     "Ancho",           { 0.f, 2.f, 0.01f }, 1.f, fPct);
    add(pMix,       "Mezcla",          { 0.f, 100.f, 1.f }, 100.f, [](float v, int) { return String((int) v) + " %"; });
    add(pOutput,    "Salida",          { -18.f, 12.f, 0.1f }, 0.f, fDb);
    return layout;
}

PercussionAnalizerAudioProcessor::PercussionAnalizerAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "STATE", createLayout())
{
    for (int i = 0; i < pCount; ++i) params[i] = apvts.getRawParameterValue(kParamIds[i]);

    const auto& list = pa::getPresets();
    int def = 0;
    for (size_t i = 0; i < list.size(); ++i)
        if (list[i].name == juce::String::fromUTF8("Percusión - Merengue - Natural")) { def = (int) i; break; }
    loadPreset(def);
}

const juce::String PercussionAnalizerAudioProcessor::getProgramName(int index)
{
    const auto& l = pa::getPresets();
    return (index >= 0 && index < (int) l.size()) ? l[(size_t) index].name : juce::String();
}

void PercussionAnalizerAudioProcessor::loadPreset(int index)
{
    const auto& list = pa::getPresets();
    if (index < 0 || index >= (int) list.size()) return;
    const auto& p = list[(size_t) index];

    auto set = [this](int idx, float v)
    {
        if (auto* prm = apvts.getParameter(kParamIds[idx]))
            prm->setValueNotifyingHost(prm->convertTo0to1(v));
    };
    set(pTargetDb, p.targetDb);   set(pTargetCent, p.targetCentroid);
    set(pHpf, p.hpf);
    set(pLowGain, p.lowGain);     set(pLowFreq, p.lowFreq);
    set(pMidGain, p.midGain);     set(pMidFreq, p.midFreq);   set(pMidQ, p.midQ);
    set(pPresGain, p.presGain);   set(pPresFreq, p.presFreq); set(pAir, p.airGain);
    set(pThresh, p.threshold);    set(pRatio, p.ratio);       set(pAttack, p.attack);
    set(pRelease, p.release);     set(pMakeup, p.makeup);     set(pTrans, p.transient);
    set(pDrive, p.drive);         set(pWidth, p.width);       set(pOutput, p.output);

    currentPreset = index;
    apvts.state.setProperty("presetIndex", index, nullptr);
}

void PercussionAnalizerAudioProcessor::loadSuggestedPreset()
{
    const int inst = detectedInstrument.load();
    if (inst < 0) return;
    const auto& list = pa::getPresets();
    const auto& cur = list[(size_t) juce::jlimit(0, (int) list.size() - 1, currentPreset.load())];
    const auto instName = pa::getInstruments()[inst];
    int first = -1, exact = -1;
    for (size_t i = 0; i < list.size(); ++i)
    {
        if (list[i].instrument != instName) continue;
        if (first < 0) first = (int) i;
        if (list[i].style == cur.style && list[i].character == cur.character) { exact = (int) i; break; }
    }
    loadPreset(exact >= 0 ? exact : first);
}

bool PercussionAnalizerAudioProcessor::isBusesLayoutSupported(const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::mono() && out != juce::AudioChannelSet::stereo()) return false;
    return out == l.getMainInputChannelSet();
}

void PercussionAnalizerAudioProcessor::prepareToPlay(double sr, int blockSize)
{
    sampleRate = sr;
    juce::dsp::ProcessSpec spec { sr, (juce::uint32) juce::jmax(blockSize, 1), 2 };
    juce::dsp::ProcessSpec mono { sr, (juce::uint32) juce::jmax(blockSize, 1), 1 };

    for (auto* b : { &tiltLo, &tiltHi, &hpf, &low, &mid, &pres, &air })
    {
        for (auto& f : b->f) f.prepare(mono);
        b->invalidate();
    }
    limiter.prepare(spec);
    limiter.setThreshold(-0.3f);
    limiter.setRelease(60.f);
    mixer.prepare(spec);
    mixer.setWetLatency(0);

    analyzer.prepare(sr);
    gainSm.reset(sr, 0.4);   gainSm.setCurrentAndTargetValue(1.f);
    tiltSm.reset(sr, 0.4);   tiltSm.setCurrentAndTargetValue(0.f);
    grState = 0.f; fastEnv = slowEnv = 0.f; lastOutGain = 1.f;
    avgInit = false;
}

void PercussionAnalizerAudioProcessor::handleFrame(const pa::SpectrumAnalyzer::Frame& f)
{
    vuIn.store(f.rmsDb);
    if (f.rmsDb < gateDb) return;

    const float frameSec = (float) pa::SpectrumAnalyzer::fftSize / (float) sampleRate;
    const float lc = std::log(juce::jmax(f.centroid, 50.f));
    if (!avgInit) { levelAvg = f.rmsDb; logCentAvg = lc; avgInit = true; }

    const int state = learnState.load();
    if (state != LearnMode::locked)
    {
        const float tau = state == LearnMode::learning ? 0.6f : 2.5f;
        const float a = 1.f - std::exp(-frameSec / tau);
        levelAvg   += a * (f.rmsDb - levelAvg);
        logCentAvg += a * (lc - logCentAvg);

        if (state == LearnMode::learning)
        {
            learnElapsed += frameSec;
            learnProgress.store(juce::jmin(1.f, learnElapsed / learnLength));
            if (learnElapsed >= learnLength) learnState.store(LearnMode::locked);
        }
    }
    const float cent = std::exp(logCentAvg);
    measuredCentroid.store(cent);
    measuredLevel.store(levelAvg);
    detectedInstrument.store(classify(cent));
}

void PercussionAnalizerAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = juce::jmin(buffer.getNumChannels(), 2);
    const int n = buffer.getNumSamples();
    if (numCh == 0 || n == 0) return;
    for (int c = numCh; c < buffer.getNumChannels(); ++c) buffer.clear(c, 0, n);

    if (learnRequested.exchange(false))
    {
        avgInit = false; learnElapsed = 0.f;
        learnProgress.store(0.f);
        learnState.store(LearnMode::learning);
    }
    if (continuousRequested.exchange(false)) learnState.store(LearnMode::continuous);

    // ---- 1. analisis de la entrada
    {
        const float* r0 = buffer.getReadPointer(0);
        const float* r1 = numCh > 1 ? buffer.getReadPointer(1) : r0;
        for (int i = 0; i < n; ++i)
            analyzer.pushSample(0.5f * (r0[i] + r1[i]), [this](const pa::SpectrumAnalyzer::Frame& f) { handleFrame(f); });
    }

    // ---- 2. correcciones automaticas
    const bool autoOn = readParam(pAuto) > 0.5f;
    const float amt = readParam(pAutoAmt) / 100.f;
    float gainDb = 0.f, tiltDb = 0.f;
    if (autoOn && avgInit)
    {
        gainDb = juce::jlimit(-18.f, 18.f, readParam(pTargetDb) - levelAvg) * amt;
        const float r = std::log(readParam(pTargetCent)) - logCentAvg;
        tiltDb = juce::jlimit(-6.f, 6.f, 3.f * r / 0.6931f) * amt;
    }
    autoGainDb.store(gainDb);
    autoTiltDb.store(tiltDb);
    gainSm.setTargetValue(juce::Decibels::decibelsToGain(gainDb));
    tiltSm.setTargetValue(tiltDb);

    // ---- 3. dry para el mix
    mixer.pushDrySamples(juce::dsp::AudioBlock<const float>(buffer.getArrayOfReadPointers(), (size_t) numCh, (size_t) n));

    juce::dsp::AudioBlock<float> block(buffer.getArrayOfWritePointers(), (size_t) numCh, (size_t) n);
    const float nyq = (float) sampleRate * 0.45f;

    // ---- 4. ganancia automatica
    if (gainSm.isSmoothing())
    {
        for (int i = 0; i < n; ++i)
        {
            const float g = gainSm.getNextValue();
            for (int c = 0; c < numCh; ++c) buffer.getWritePointer(c)[i] *= g;
        }
    }
    else buffer.applyGain(gainSm.getTargetValue());

    // ---- 5. tilt automatico (brillo)
    const float tilt = tiltSm.skip(n);
    updateBand(tiltLo, 250.f, tilt, 0.f, [&] { return Coeffs::makeLowShelf(sampleRate, 250.f, 0.707f, juce::Decibels::decibelsToGain(-tilt * 0.5f)); });
    updateBand(tiltHi, 3500.f, tilt, 0.f, [&] { return Coeffs::makeHighShelf(sampleRate, 3500.f, 0.707f, juce::Decibels::decibelsToGain(tilt * 0.5f)); });
    runBand(tiltLo, block, numCh);
    runBand(tiltHi, block, numCh);

    // ---- 6. EQ del preset
    const float hpfF = juce::jmin(readParam(pHpf), nyq);
    updateBand(hpf, hpfF, 0.f, 0.f, [&] { return Coeffs::makeHighPass(sampleRate, hpfF, 0.707f); });
    const float lf = juce::jmin(readParam(pLowFreq), nyq), lg = readParam(pLowGain);
    updateBand(low, lf, lg, 0.f, [&] { return Coeffs::makeLowShelf(sampleRate, lf, 0.707f, juce::Decibels::decibelsToGain(lg)); });
    const float mf = juce::jmin(readParam(pMidFreq), nyq), mg = readParam(pMidGain), mq = readParam(pMidQ);
    updateBand(mid, mf, mg, mq, [&] { return Coeffs::makePeakFilter(sampleRate, mf, mq, juce::Decibels::decibelsToGain(mg)); });
    const float pf = juce::jmin(readParam(pPresFreq), nyq), pg = readParam(pPresGain);
    updateBand(pres, pf, pg, 0.f, [&] { return Coeffs::makePeakFilter(sampleRate, pf, 0.9f, juce::Decibels::decibelsToGain(pg)); });
    const float ag = readParam(pAir);
    const float airF = juce::jmin(10000.f, nyq);
    updateBand(air, airF, ag, 0.f, [&] { return Coeffs::makeHighShelf(sampleRate, airF, 0.707f, juce::Decibels::decibelsToGain(ag)); });
    runBand(hpf, block, numCh);
    runBand(low, block, numCh);
    runBand(mid, block, numCh);
    runBand(pres, block, numCh);
    runBand(air, block, numCh);

    // ---- 7. calor (saturacion suave)
    const float drive = readParam(pDrive);
    if (drive > 0.001f)
    {
        const float k = 1.f + 3.f * drive;
        for (int c = 0; c < numCh; ++c)
        {
            float* d = buffer.getWritePointer(c);
            for (int i = 0; i < n; ++i) d[i] = std::tanh(k * d[i]) / k;
        }
    }

    // ---- 8. transientes
    const float trans = readParam(pTrans);
    if (std::abs(trans) > 0.01f)
    {
        const auto coef = [this](float sec) { return 1.f - std::exp(-1.f / ((float) sampleRate * sec)); };
        const float fa = coef(0.0005f), fr = coef(0.02f), sa = coef(0.02f), sr2 = coef(0.12f);
        for (int i = 0; i < n; ++i)
        {
            float det = 0.f;
            for (int c = 0; c < numCh; ++c) det = juce::jmax(det, std::abs(buffer.getReadPointer(c)[i]));
            fastEnv += (det > fastEnv ? fa : fr) * (det - fastEnv);
            slowEnv += (det > slowEnv ? sa : sr2) * (det - slowEnv);
            const float diff = juce::jlimit(-1.f, 1.f, (fastEnv - slowEnv) / (slowEnv + 1.0e-4f));
            const float g = juce::Decibels::decibelsToGain(trans * 6.f * diff);
            for (int c = 0; c < numCh; ++c) buffer.getWritePointer(c)[i] *= g;
        }
    }

    // ---- 9. compresor enlazado (estereo)
    {
        const float thr = readParam(pThresh), ratio = readParam(pRatio);
        const float ac = std::exp(-1.f / ((float) sampleRate * readParam(pAttack) * 0.001f));
        const float rc = std::exp(-1.f / ((float) sampleRate * readParam(pRelease) * 0.001f));
        const float knee = 6.f;
        float grSum = 0.f;
        for (int i = 0; i < n; ++i)
        {
            float det = 0.f;
            for (int c = 0; c < numCh; ++c) det = juce::jmax(det, std::abs(buffer.getReadPointer(c)[i]));
            const float lvl = 20.f * std::log10(det + 1.0e-6f);
            const float over = lvl - thr;
            float outL;
            if (2.f * over < -knee) outL = lvl;
            else if (2.f * std::abs(over) <= knee)
            {
                const float x = over + knee * 0.5f;
                outL = lvl + (1.f / ratio - 1.f) * x * x / (2.f * knee);
            }
            else outL = thr + over / ratio;
            const float target = juce::jmax(0.f, lvl - outL);
            const float co = target > grState ? ac : rc;
            grState = co * grState + (1.f - co) * target;
            const float g = juce::Decibels::decibelsToGain(-grState);
            for (int c = 0; c < numCh; ++c) buffer.getWritePointer(c)[i] *= g;
            grSum += grState;
        }
        grDb.store(grSum / (float) n);
    }

    // ---- 10. ancho estereo
    if (numCh == 2)
    {
        const float w = readParam(pWidth);
        float* l = buffer.getWritePointer(0);
        float* r = buffer.getWritePointer(1);
        for (int i = 0; i < n; ++i)
        {
            const float m = 0.5f * (l[i] + r[i]);
            const float s = 0.5f * (l[i] - r[i]) * w;
            l[i] = m + s;
            r[i] = m - s;
        }
    }

    // ---- 11. makeup + salida + mezcla + limitador
    const float outGain = juce::Decibels::decibelsToGain(readParam(pMakeup) + readParam(pOutput));
    // Apply the output gain to every active channel (not just the left channel).
    for (int c = 0; c < numCh; ++c)
        buffer.applyGainRamp(c, 0, n, lastOutGain, outGain);
    lastOutGain = outGain;

    mixer.setWetMixProportion(readParam(pMix) / 100.f);
    mixer.mixWetSamples(block);

    juce::dsp::ProcessContextReplacing<float> ctx(block);
    limiter.process(ctx);

    for (int c = 0; c < 2; ++c)
    {
        const float pk = c < numCh ? buffer.getMagnitude(c, 0, n) : 0.f;
        outPeak[c].store(juce::Decibels::gainToDecibels(pk, -90.f));
    }
}

juce::AudioProcessorEditor* PercussionAnalizerAudioProcessor::createEditor()
{
    return new PercussionAnalizerAudioProcessorEditor(*this);
}

void PercussionAnalizerAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("presetIndex", currentPreset.load(), nullptr);
    if (auto xml = state.createXml()) copyXmlToBinary(*xml, destData);
}

void PercussionAnalizerAudioProcessor::setStateInformation(const void* data, int size)
{
    if (auto xml = getXmlFromBinary(data, size))
        if (xml->hasTagName(apvts.state.getType()))
        {
            auto vt = juce::ValueTree::fromXml(*xml);
            apvts.replaceState(vt);
            currentPreset = (int) vt.getProperty("presetIndex", currentPreset.load());
        }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PercussionAnalizerAudioProcessor();
}
