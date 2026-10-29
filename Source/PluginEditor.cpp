#include "PluginEditor.h"
#include "BinaryData.h"

namespace
{
using juce::Colour;
using juce::Colours;

void drawScrew(juce::Graphics& g, float cx, float cy, float r, juce::Random& rnd)
{
    g.setColour(Colours::black.withAlpha(0.5f));
    g.fillEllipse(cx - r + 1.f, cy - r + 1.5f, r * 2.f, r * 2.f);
    g.setGradientFill(juce::ColourGradient(Colour(0xffd9dbe0), cx - r * 0.5f, cy - r * 0.6f, Colour(0xff3d3f44), cx + r, cy + r, true));
    g.fillEllipse(cx - r, cy - r, r * 2.f, r * 2.f);
    const float a = rnd.nextFloat() * 6.28f;
    g.setColour(Colour(0xff1a1b1d));
    g.drawLine(cx - std::cos(a) * r * 0.75f, cy - std::sin(a) * r * 0.75f, cx + std::cos(a) * r * 0.75f, cy + std::sin(a) * r * 0.75f, 1.6f);
}

void drawPlate(juce::Graphics& g, juce::Rectangle<int> r, Colour top, Colour bot, bool worn,
               const juce::String& model, const juce::String& desc, Colour text, juce::Random& rnd)
{
    auto rf = r.toFloat();
    g.setGradientFill(juce::ColourGradient(top, rf.getX(), rf.getY(), bot, rf.getX(), rf.getBottom(), false));
    g.fillRect(rf);

    for (int y = r.getY(); y < r.getBottom(); y += 2)
    {
        g.setColour(Colours::white.withAlpha(rnd.nextFloat() * 0.035f));
        g.drawHorizontalLine(y, rf.getX(), rf.getRight());
    }
    if (worn)
    {
        const int dots = (r.getWidth() * r.getHeight()) / 60;
        for (int i = 0; i < dots; ++i)
        {
            g.setColour(rnd.nextBool() ? Colour(0x24000000) : Colour(0x1affffff));
            g.fillRect(r.getX() + rnd.nextInt(r.getWidth()), r.getY() + rnd.nextInt(r.getHeight()), 1 + rnd.nextInt(2), 1 + rnd.nextInt(2));
        }
        for (int i = 0; i < 26; ++i)
        {
            const bool leftEdge = rnd.nextBool();
            const float x = leftEdge ? rf.getX() + rnd.nextFloat() * 10.f : rf.getRight() - rnd.nextFloat() * 10.f;
            const float y = rf.getY() + rnd.nextFloat() * rf.getHeight();
            g.setColour(Colour(0x40d9d2c4));
            g.fillEllipse(x, y, 2.f + rnd.nextFloat() * 5.f, 2.f + rnd.nextFloat() * 6.f);
        }
    }
    g.setColour(Colours::white.withAlpha(0.14f));  g.drawVerticalLine(r.getX() + 1, rf.getY(), rf.getBottom());
    g.setColour(Colours::black.withAlpha(0.65f));  g.drawVerticalLine(r.getRight() - 1, rf.getY(), rf.getBottom());
    g.setColour(Colours::black.withAlpha(0.45f));  g.drawVerticalLine(r.getX(), rf.getY(), rf.getBottom());

    const float sr = 5.5f;
    drawScrew(g, rf.getX() + 14.f, rf.getY() + 14.f, sr, rnd);
    drawScrew(g, rf.getRight() - 14.f, rf.getY() + 14.f, sr, rnd);
    drawScrew(g, rf.getX() + 14.f, rf.getBottom() - 14.f, sr, rnd);
    drawScrew(g, rf.getRight() - 14.f, rf.getBottom() - 14.f, sr, rnd);

    // escudo YMY
    juce::Rectangle<float> badge(rf.getCentreX() - 20.f, rf.getY() + 8.f, 40.f, 26.f);
    g.setGradientFill(juce::ColourGradient(Colour(0xff2a2b2e), badge.getX(), badge.getY(), Colour(0xff08080a), badge.getX(), badge.getBottom(), false));
    g.fillRoundedRectangle(badge, 5.f);
    g.setColour(Colour(0xffcfcac0)); g.drawRoundedRectangle(badge.reduced(1.f), 5.f, 1.5f);
    g.setFont(mkFont(13.f, true));
    g.drawText("YMY", badge.toNearestInt(), juce::Justification::centred);

    // titulo grabado
    auto tr = r.withTrimmedTop(r.getHeight() - 44).withTrimmedBottom(6);
    g.setFont(mkFont(18.f, true));
    g.setColour(Colours::black.withAlpha(0.7f)); g.drawText(model, tr.translated(1, 1).removeFromTop(24), juce::Justification::centred);
    g.setColour(text);                           g.drawText(model, tr.removeFromTop(24), juce::Justification::centred);
    g.setFont(mkFont(10.f, true));
    g.setColour(text.withAlpha(0.8f));           g.drawText(desc, tr, juce::Justification::centred);
}

}

//==============================================================================
RealLookAndFeel::RealLookAndFeel()
{
    setColour(juce::TextButton::textColourOffId, Colour(0xfff1e4c2));
    setColour(juce::TextButton::textColourOnId, Colour(0xff1a1206));
    setColour(juce::PopupMenu::backgroundColourId, Colour(0xff17181a));
    setColour(juce::PopupMenu::textColourId, Colour(0xfff1e4c2));
    setColour(juce::PopupMenu::highlightedBackgroundColourId, Colour(0xffd9822b));
    setColour(juce::PopupMenu::highlightedTextColourId, Colours::black);
}

void RealLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int w, int h, float pos, float startAngle, float endAngle, juce::Slider& s)
{
    auto area = juce::Rectangle<float>((float) x, (float) y, (float) w, (float) h).reduced(3.f);
    const float dia = juce::jmin(area.getWidth(), area.getHeight());
    const auto c = area.getCentre();
    const float R = dia * 0.5f;
    const float kr = R * 0.68f;
    const float ang = startAngle + pos * (endAngle - startAngle);
    const auto capARGB = (juce::uint32) (juce::int64) s.getProperties()["cap"];
    const bool coloured = capARGB != 0;
    const Colour cap(capARGB);

    // marcas
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.f * (endAngle - startAngle);
        const float rr = R * 0.92f;
        g.setColour(Colour(0xfff1e4c2).withAlpha(i == 0 || i == 10 ? 0.95f : 0.65f));
        g.fillEllipse(c.x + std::sin(a) * rr - 1.5f, c.y - std::cos(a) * rr - 1.5f, 3.f, 3.f);
    }
    // arco activo
    juce::Path arc;
    arc.addCentredArc(c.x, c.y, R * 0.80f, R * 0.80f, 0.f, startAngle, ang, true);
    g.setColour((coloured ? cap.brighter(0.4f) : Colour(0xffe8a33a)).withAlpha(0.55f));
    g.strokePath(arc, juce::PathStrokeType(2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // sombra
    for (int i = 0; i < 4; ++i)
    {
        g.setColour(Colours::black.withAlpha(0.18f));
        g.fillEllipse(c.x - kr - i, c.y - kr + 3.f - i * 0.5f, 2.f * (kr + i), 2.f * (kr + i));
    }
    // aro
    g.setGradientFill(juce::ColourGradient(Colour(0xff767a82), c.x - kr * 0.7f, c.y - kr * 0.9f, Colour(0xff0e0f11), c.x + kr * 0.7f, c.y + kr * 0.9f, false));
    g.fillEllipse(c.x - kr, c.y - kr, 2.f * kr, 2.f * kr);
    // moleteado
    g.setColour(Colours::black.withAlpha(0.35f));
    for (int i = 0; i < 48; ++i)
    {
        const float a = ang + (float) i * juce::MathConstants<float>::twoPi / 48.f;
        g.drawLine(c.x + std::sin(a) * kr * 0.90f, c.y - std::cos(a) * kr * 0.90f, c.x + std::sin(a) * kr * 0.99f, c.y - std::cos(a) * kr * 0.99f, 1.f);
    }
    // tapa
    const float kc = kr * 0.84f;
    if (coloured)
        g.setGradientFill(juce::ColourGradient(cap.brighter(0.55f), c.x - kc * 0.4f, c.y - kc * 0.55f, cap.darker(0.75f), c.x + kc * 0.5f, c.y + kc * 0.75f, false));
    else
        g.setGradientFill(juce::ColourGradient(Colour(0xff55585f), c.x - kc * 0.4f, c.y - kc * 0.55f, Colour(0xff17181b), c.x + kc * 0.5f, c.y + kc * 0.75f, false));
    g.fillEllipse(c.x - kc, c.y - kc, 2.f * kc, 2.f * kc);
    g.setGradientFill(juce::ColourGradient(Colours::white.withAlpha(0.33f), c.x - kc * 0.3f, c.y - kc * 0.6f, Colours::transparentWhite, c.x, c.y + kc * 0.2f, true));
    g.fillEllipse(c.x - kc, c.y - kc, 2.f * kc, 2.f * kc);
    g.setColour(Colours::black.withAlpha(0.5f));
    g.drawEllipse(c.x - kc, c.y - kc, 2.f * kc, 2.f * kc, 1.f);
    // indicador
    const float r0 = kc * 0.28f, r1 = kc * 0.95f;
    g.setColour(Colours::black.withAlpha(0.5f));
    g.drawLine(c.x + std::sin(ang) * r0 + 0.8f, c.y - std::cos(ang) * r0 + 1.f, c.x + std::sin(ang) * r1 + 0.8f, c.y - std::cos(ang) * r1 + 1.f, 2.6f);
    g.setColour(Colour(0xfffaf3e0));
    g.drawLine(c.x + std::sin(ang) * r0, c.y - std::cos(ang) * r0, c.x + std::sin(ang) * r1, c.y - std::cos(ang) * r1, 2.4f);
}

void RealLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& b, const Colour&, bool highlighted, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced(1.f);
    const bool on = b.getToggleState();
    Colour top = on ? Colour(0xffffc261) : Colour(0xff4a4c52);
    Colour bot = on ? Colour(0xffc7701a) : Colour(0xff1a1b1e);
    if (highlighted) { top = top.brighter(0.15f); bot = bot.brighter(0.1f); }
    if (down) std::swap(top, bot);
    if (on) { g.setColour(Colour(0x55ffa030)); g.fillRoundedRectangle(r.expanded(2.f), 5.f); }
    g.setGradientFill(juce::ColourGradient(top, r.getX(), r.getY(), bot, r.getX(), r.getBottom(), false));
    g.fillRoundedRectangle(r, 4.f);
    g.setColour(Colours::black.withAlpha(0.8f));
    g.drawRoundedRectangle(r, 4.f, 1.2f);
    g.setColour(Colours::white.withAlpha(0.18f));
    g.drawLine(r.getX() + 4.f, r.getY() + 1.5f, r.getRight() - 4.f, r.getY() + 1.5f, 1.f);
}

//==============================================================================
VuMeter::VuMeter(const juce::String& t, float mn, float mx, int mj, float red)
    : title(t), minDb(mn), maxDb(mx), majors(mj), redStart(red) {}

void VuMeter::tick()
{
    value += (target > value ? 0.35f : 0.12f) * (target - value);
    repaint();
}

void VuMeter::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(Colour(0xff3d3e42), b.getX(), b.getY(), Colour(0xff09090a), b.getRight(), b.getBottom(), false));
    g.fillRoundedRectangle(b, 7.f);
    auto face = b.reduced(7.f);
    g.setGradientFill(juce::ColourGradient(Colour(0xfff7e8b8), face.getCentreX(), face.getY(), Colour(0xffc7a456), face.getCentreX(), face.getBottom(), false));
    g.fillRoundedRectangle(face, 3.f);

    g.saveState();
    g.reduceClipRegion(face.toNearestInt());
    const juce::Point<float> pivot(face.getCentreX(), face.getBottom() + face.getHeight() * 0.28f);
    const float R = face.getHeight() * 1.12f;
    const float a0 = -0.80f, a1 = 0.80f;
    const int steps = majors * 2;
    auto pt = [&](float ang, float rad) { return juce::Point<float>(pivot.x + std::sin(ang) * rad, pivot.y - std::cos(ang) * rad); };

    for (int i = 0; i <= steps; ++i)
    {
        const float t = (float) i / (float) steps;
        const float ang = a0 + t * (a1 - a0);
        const bool major = (i % 2 == 0);
        g.setColour(t >= redStart ? Colour(0xffb02a18) : Colour(0xff2a1d0c));
        g.drawLine({ pt(ang, R * 0.84f), pt(ang, R * (major ? 0.94f : 0.89f)) }, major ? 1.8f : 1.f);
        if (major)
        {
            g.setFont(mkFont(10.f, true));
            const auto p = pt(ang, R * 0.76f);
            g.drawText(juce::String(juce::roundToInt(minDb + t * (maxDb - minDb))), juce::Rectangle<float>(p.x - 14.f, p.y - 7.f, 28.f, 14.f), juce::Justification::centred);
        }
    }
    g.setFont(mkFont(15.f, true));
    g.setColour(Colour(0xff2a1d0c).withAlpha(0.85f));
    g.drawText(title, juce::Rectangle<float>(face.getX(), face.getY() + face.getHeight() * 0.50f, face.getWidth(), 20.f), juce::Justification::centred);

    const float norm = juce::jlimit(0.f, 1.f, (value - minDb) / (maxDb - minDb));
    const float ang = a0 + norm * (a1 - a0);
    g.setColour(Colours::black.withAlpha(0.28f));
    g.drawLine({ pt(ang, 0.f) + juce::Point<float>(2.5f, 2.5f), pt(ang, R * 0.95f) + juce::Point<float>(2.5f, 2.5f) }, 2.f);
    g.setColour(Colour(0xff15110c));
    g.drawLine({ pt(ang, 0.f), pt(ang, R * 0.95f) }, 1.6f);

    g.setGradientFill(juce::ColourGradient(Colour(0xff222326), 0.f, face.getBottom() - 12.f, Colour(0xff050506), 0.f, face.getBottom(), false));
    g.fillRect(face.getX(), face.getBottom() - face.getHeight() * 0.12f, face.getWidth(), face.getHeight() * 0.12f);
    g.setGradientFill(juce::ColourGradient(Colours::white.withAlpha(0.35f), face.getX(), face.getY(), Colours::transparentWhite, face.getRight(), face.getCentreY(), false));
    g.fillRect(face.withHeight(face.getHeight() * 0.55f));
    g.restoreState();

    g.setColour(Colours::black.withAlpha(0.6f));
    g.drawRoundedRectangle(face, 3.f, 2.f);
}

//==============================================================================
float SpectrumView::freqToX(double f, float w) const
{
    return (float) (std::log(f / 30.0) / std::log(18000.0 / 30.0)) * w;
}

void SpectrumView::tick()
{
    for (int i = 0; i < pa::SpectrumAnalyzer::numBands; ++i)
    {
        const float v = proc.analyzer.bands[(size_t) i].load();
        disp[(size_t) i] = v > disp[(size_t) i] ? v : disp[(size_t) i] * 0.85f + v * 0.15f;
    }
    repaint();
}

void SpectrumView::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(Colour(0xff0a0e0f), 0.f, b.getY(), Colour(0xff1b2426), 0.f, b.getBottom(), false));
    g.fillRoundedRectangle(b, 6.f);
    auto s = b.reduced(5.f);
    const float minDb = -96.f, maxDb = -24.f;
    auto yOf = [&](float db) { return s.getBottom() - juce::jlimit(0.f, 1.f, (db - minDb) / (maxDb - minDb)) * s.getHeight(); };

    g.setColour(Colour(0x22ffffff));
    for (double f : { 100.0, 1000.0, 10000.0 })
        g.drawVerticalLine((int) (s.getX() + freqToX(f, s.getWidth())), s.getY(), s.getBottom());
    for (int i = 1; i < 4; ++i) g.drawHorizontalLine((int) (s.getY() + s.getHeight() * i / 4.f), s.getX(), s.getRight());

    juce::Path line, fill;
    for (int i = 0; i < pa::SpectrumAnalyzer::numBands; ++i)
    {
        const float x = s.getX() + freqToX(pa::SpectrumAnalyzer::bandFreq(i), s.getWidth());
        const float y = yOf(disp[(size_t) i]);
        if (i == 0) { line.startNewSubPath(x, y); fill.startNewSubPath(x, s.getBottom()); fill.lineTo(x, y); }
        else { line.lineTo(x, y); fill.lineTo(x, y); }
    }
    fill.lineTo(s.getRight(), s.getBottom());
    fill.closeSubPath();
    g.setGradientFill(juce::ColourGradient(Colour(0xc0ffb03a), 0.f, s.getY(), Colour(0x08ffb03a), 0.f, s.getBottom(), false));
    g.fillPath(fill);
    g.setColour(Colour(0xffffd27a));
    g.strokePath(line, juce::PathStrokeType(1.4f));

    const float cm = proc.measuredCentroid.load();
    if (cm > 0.f)
    {
        const float x = s.getX() + freqToX(juce::jlimit(30.f, 18000.f, cm), s.getWidth());
        g.setColour(Colours::white.withAlpha(0.85f));
        g.drawVerticalLine((int) x, s.getY(), s.getBottom());
    }
    const float ct = *proc.apvts.getRawParameterValue("targetCent");
    {
        const float x = s.getX() + freqToX(juce::jlimit(30.f, 18000.f, ct), s.getWidth());
        g.setColour(Colour(0xff4fd1c5));
        g.drawVerticalLine((int) x, s.getY(), s.getBottom());
    }
    g.setFont(mkFont(9.f));
    g.setColour(Colour(0xaaf1e4c2));
    const double labF[3] = { 100.0, 1000.0, 10000.0 };
    const char* labT[3] = { "100", "1k", "10k" };
    for (int i = 0; i < 3; ++i)
        g.drawText(labT[i], juce::Rectangle<float>(s.getX() + freqToX(labF[i], s.getWidth()) + 2.f, s.getBottom() - 12.f, 24.f, 11.f), juce::Justification::left);
    g.setColour(Colours::black.withAlpha(0.7f));
    g.drawRoundedRectangle(b, 6.f, 2.f);
}

//==============================================================================
void EqCurveView::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(Colour(0xff0a0e12), 0.f, b.getY(), Colour(0xff172027), 0.f, b.getBottom(), false));
    g.fillRoundedRectangle(b, 6.f);
    auto s = b.reduced(5.f);
    g.setColour(Colour(0x22ffffff));
    g.drawHorizontalLine((int) s.getCentreY(), s.getX(), s.getRight());
    for (double f : { 100.0, 1000.0, 10000.0 })
        g.drawVerticalLine((int) (s.getX() + (float) (std::log(f / 20.0) / std::log(1000.0)) * s.getWidth()), s.getY(), s.getBottom());

    const double sr = proc.getSampleRate() > 0 ? proc.getSampleRate() : 48000.0;
    auto P = [&](int i) { return proc.apvts.getRawParameterValue(kParamIds[i])->load(); };
    using C = juce::dsp::IIR::Coefficients<float>;
    auto hp  = C::makeHighPass(sr, P(pHpf), 0.707f);
    auto lo  = C::makeLowShelf(sr, P(pLowFreq), 0.707f, juce::Decibels::decibelsToGain(P(pLowGain)));
    auto mi  = C::makePeakFilter(sr, P(pMidFreq), P(pMidQ), juce::Decibels::decibelsToGain(P(pMidGain)));
    auto pr  = C::makePeakFilter(sr, P(pPresFreq), 0.9f, juce::Decibels::decibelsToGain(P(pPresGain)));
    auto ai  = C::makeHighShelf(sr, juce::jmin(10000.0, sr * 0.45), 0.707f, juce::Decibels::decibelsToGain(P(pAir)));

    juce::Path p;
    const int N = 120;
    for (int i = 0; i < N; ++i)
    {
        const double t = (double) i / (N - 1);
        const double f = 20.0 * std::pow(1000.0, t);
        const double m = hp->getMagnitudeForFrequency(f, sr) * lo->getMagnitudeForFrequency(f, sr) * mi->getMagnitudeForFrequency(f, sr)
                       * pr->getMagnitudeForFrequency(f, sr) * ai->getMagnitudeForFrequency(f, sr);
        const float db = juce::jlimit(-18.f, 18.f, juce::Decibels::gainToDecibels((float) m));
        const float x = s.getX() + (float) t * s.getWidth();
        const float y = s.getCentreY() - db / 18.f * s.getHeight() * 0.5f;
        if (i == 0) p.startNewSubPath(x, y); else p.lineTo(x, y);
    }
    g.setColour(Colour(0x554fd1c5));
    g.strokePath(p, juce::PathStrokeType(4.5f));
    g.setColour(Colour(0xffa8f3ec));
    g.strokePath(p, juce::PathStrokeType(1.6f));
    g.setColour(Colours::black.withAlpha(0.7f));
    g.drawRoundedRectangle(b, 6.f, 2.f);
}

//==============================================================================
void LedMeter::tick()
{
    for (int c = 0; c < 2; ++c)
    {
        const float v = proc.outPeak[c].load();
        val[c] = juce::jmax(v, val[c] - 1.5f);
    }
    repaint();
}

void LedMeter::paint(juce::Graphics& g)
{
    auto b = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(Colour(0xff0a1214), 0.f, b.getY(), Colour(0xff142326), 0.f, b.getBottom(), false));
    g.fillRoundedRectangle(b, 6.f);
    auto s = b.reduced(10.f, 10.f);
    s.removeFromBottom(14.f);
    const int segs = 28;
    const float colW = s.getWidth() * 0.34f;
    for (int c = 0; c < 2; ++c)
    {
        const float x = s.getX() + (c == 0 ? s.getWidth() * 0.12f : s.getWidth() * 0.54f);
        for (int j = 0; j < segs; ++j)
        {
            const float thr = -48.f + (float) j * (48.f / (float) segs);
            const bool lit = val[c] > thr;
            Colour col = j >= segs - 3 ? Colour(0xffe0392b) : (j >= segs - 8 ? Colour(0xffe8b83a) : Colour(0xff43c768));
            const float h = s.getHeight() / (float) segs;
            g.setColour(lit ? col : col.withAlpha(0.14f));
            g.fillRect(x, s.getBottom() - (float) (j + 1) * h + 1.f, colW, h - 2.f);
        }
        g.setColour(Colour(0xaaf1e4c2));
        g.setFont(mkFont(11.f, true));
        g.drawText(c == 0 ? "L" : "R", juce::Rectangle<float>(x, s.getBottom() + 1.f, colW, 13.f), juce::Justification::centred);
    }
    g.setColour(Colours::black.withAlpha(0.7f));
    g.drawRoundedRectangle(b, 6.f, 2.f);
}

//==============================================================================
PercussionAnalizerAudioProcessorEditor::PercussionAnalizerAudioProcessorEditor(Proc& p)
    : juce::AudioProcessorEditor(&p), proc(p), spectrum(p), eqView(p), leds(p)
{
    setLookAndFeel(&laf);
    logoImage = juce::ImageFileFormat::loadFrom(BinaryData::logo_png, (size_t) BinaryData::logo_pngSize);

    const Colour cream(0xfff1e4c2), white(0xfffff6e8);
    const Colour none;
    kTarget  = addKnob(pTargetDb,   "NIVEL OBJ.",  none, cream);
    kCent    = addKnob(pTargetCent, "BRILLO OBJ.", none, cream);
    kAutoAmt = addKnob(pAutoAmt,    "AUTO %",      none, cream);

    kHpf   = addKnob(pHpf,      "HPF",      none,               cream);
    kLowG  = addKnob(pLowGain,  "LOW",      Colour(0xff2d63d6), cream);
    kLowF  = addKnob(pLowFreq,  "LOW HZ",   Colour(0xff2d63d6), cream);
    kMidG  = addKnob(pMidGain,  "MID",      Colour(0xff2fa84f), cream);
    kMidF  = addKnob(pMidFreq,  "MID HZ",   Colour(0xff2fa84f), cream);
    kMidQ  = addKnob(pMidQ,     "MID Q",    Colour(0xff2fa84f), cream);
    kPresG = addKnob(pPresGain, "PRES",     Colour(0xffe8872b), cream);
    kPresF = addKnob(pPresFreq, "PRES HZ",  Colour(0xffe8872b), cream);
    kAir   = addKnob(pAir,      "AIRE",     Colour(0xffd62f2f), cream);

    kThr    = addKnob(pThresh,  "UMBRAL",      none, white);
    kRatio  = addKnob(pRatio,   "RATIO",       none, white);
    kAtt    = addKnob(pAttack,  "ATAQUE",      none, white);
    kRel    = addKnob(pRelease, "RELEASE",     none, white);
    kMakeup = addKnob(pMakeup,  "MAKEUP",      none, white);
    kTrans  = addKnob(pTrans,   "TRANSIENT",   none, white);

    kDrive = addKnob(pDrive,  "CALOR",   none, white);
    kWidth = addKnob(pWidth,  "ANCHO",   none, white);
    kMix   = addKnob(pMix,    "MEZCLA",  none, white);
    kOut   = addKnob(pOutput, "SALIDA",  none, white);

    addAndMakeVisible(vuLevel);
    addAndMakeVisible(vuGr);
    addAndMakeVisible(spectrum);
    addAndMakeVisible(eqView);
    addAndMakeVisible(leds);

    for (auto* b : { &autoButton, &learnButton, &contButton, &suggestButton, &prevButton, &nextButton, &presetButton })
        addAndMakeVisible(*b);
    autoButton.setClickingTogglesState(true);
    autoAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(proc.apvts, "auto", autoButton);
    learnButton.onClick   = [this] { proc.startLearning(); };
    contButton.onClick    = [this] { proc.startContinuous(); };
    suggestButton.onClick = [this] { proc.loadSuggestedPreset(); updatePresetText(); };
    prevButton.onClick    = [this] { stepPreset(-1); };
    nextButton.onClick    = [this] { stepPreset(1); };
    presetButton.onClick  = [this] { showPresetMenu(); };

    for (auto* l : { &detectedLabel, &stateLabel, &corrLabel })
    {
        l->setJustificationType(juce::Justification::centred);
        l->setColour(juce::Label::textColourId, Colour(0xffe8d9b0));
        l->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(*l);
    }
    detectedLabel.setFont(mkFont(12.f, true));
    stateLabel.setFont(mkFont(11.f, true));
    corrLabel.setFont(mkFont(11.f));

    updatePresetText();
    setResizable(true, true);
    setResizeLimits(840, 480, 1680, 960);
    getConstrainer()->setFixedAspectRatio(1120.0 / 640.0);
    setSize(1120, 640);
    startTimerHz(30);
}

PercussionAnalizerAudioProcessorEditor::~PercussionAnalizerAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

PercussionAnalizerAudioProcessorEditor::Knob* PercussionAnalizerAudioProcessorEditor::addKnob(int idx, const juce::String& name, Colour cap, Colour labelColour)
{
    auto k = std::make_unique<Knob>();
    k->slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    k->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 16);
    k->slider.setColour(juce::Slider::textBoxTextColourId, Colour(0xfff6ead0));
    k->slider.setColour(juce::Slider::textBoxOutlineColourId, Colours::transparentBlack);
    k->slider.setColour(juce::Slider::textBoxBackgroundColourId, Colour(0x33000000));
    k->slider.getProperties().set("cap", (juce::int64) cap.getARGB());
    k->label.setText(name, juce::dontSendNotification);
    k->label.setJustificationType(juce::Justification::centred);
    k->label.setFont(mkFont(11.5f, true));
    k->label.setColour(juce::Label::textColourId, labelColour);
    k->label.setInterceptsMouseClicks(false, false);
    k->att = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(proc.apvts, kParamIds[idx], k->slider);
    addAndMakeVisible(k->slider);
    addAndMakeVisible(k->label);
    knobs.push_back(std::move(k));
    return knobs.back().get();
}

void PercussionAnalizerAudioProcessorEditor::layoutGrid(juce::Rectangle<int> area, const std::vector<Knob*>& ks, int cols)
{
    const int rows = ((int) ks.size() + cols - 1) / cols;
    const int cw = area.getWidth() / cols, ch = area.getHeight() / rows;
    for (size_t i = 0; i < ks.size(); ++i)
    {
        juce::Rectangle<int> cell(area.getX() + ((int) i % cols) * cw, area.getY() + ((int) i / cols) * ch, cw, ch);
        ks[i]->label.setBounds(cell.removeFromBottom(16));
        ks[i]->slider.setBounds(cell);
    }
}

void PercussionAnalizerAudioProcessorEditor::computeLayout()
{
    auto r = getLocalBounds();
    const int side = juce::roundToInt((float) r.getWidth() * 0.018f);
    woodL = r.removeFromLeft(side);
    woodR = r.removeFromRight(side);
    header = r.removeFromTop(juce::roundToInt((float) getHeight() * 0.10f));
    const int gap = 0;
    const int total = r.getWidth();
    const int wA = (int) (total * 0.30f), wB = (int) (total * 0.26f), wC = (int) (total * 0.24f);
    panelA = r.removeFromLeft(wA); r.removeFromLeft(gap);
    panelB = r.removeFromLeft(wB);
    panelC = r.removeFromLeft(wC);
    panelD = r;
}

void PercussionAnalizerAudioProcessorEditor::resized()
{
    computeLayout();
    renderBackground();

    // cabecera
    {
        auto h = header.reduced(10, 8);
        h.removeFromLeft(h.getHeight() + 330);
        auto right = h.removeFromRight(190);
        juce::ignoreUnused(right);
        const int bw = 30;
        const int total = bw * 2 + 380 + 12;
        auto c = h.withSizeKeepingCentre(juce::jmin(total, h.getWidth()), h.getHeight());
        prevButton.setBounds(c.removeFromLeft(bw)); c.removeFromLeft(6);
        nextButton.setBounds(c.removeFromRight(bw)); c.removeFromRight(6);
        presetButton.setBounds(c);
    }

    auto content = [](juce::Rectangle<int> p) { return p.withTrimmedTop(40).withTrimmedBottom(50).reduced(12, 0); };

    // Panel A
    {
        auto c = content(panelA);
        auto top = c.removeFromTop((int) (c.getHeight() * 0.34f));
        vuLevel.setBounds(top.removeFromLeft((int) (top.getWidth() * 0.42f)).reduced(0, 2));
        spectrum.setBounds(top.reduced(6, 2));
        detectedLabel.setBounds(c.removeFromTop(22));
        stateLabel.setBounds(c.removeFromTop(18));
        corrLabel.setBounds(c.removeFromTop(18));
        auto row1 = c.removeFromTop(30).reduced(0, 2);
        autoButton.setBounds(row1.removeFromLeft(row1.getWidth() / 2).reduced(3, 0));
        suggestButton.setBounds(row1.reduced(3, 0));
        auto row2 = c.removeFromTop(30).reduced(0, 2);
        learnButton.setBounds(row2.removeFromLeft(row2.getWidth() / 2).reduced(3, 0));
        contButton.setBounds(row2.reduced(3, 0));
        layoutGrid(c, { kTarget, kCent, kAutoAmt }, 3);
    }
    // Panel B
    {
        auto c = content(panelB);
        eqView.setBounds(c.removeFromTop((int) (c.getHeight() * 0.22f)).reduced(0, 2));
        layoutGrid(c, { kHpf, kLowG, kLowF, kMidG, kMidF, kMidQ, kPresG, kPresF, kAir }, 3);
    }
    // Panel C
    {
        auto c = content(panelC);
        vuGr.setBounds(c.removeFromTop((int) (c.getHeight() * 0.34f)).reduced(10, 2));
        layoutGrid(c, { kThr, kRatio, kAtt, kRel, kMakeup, kTrans }, 3);
    }
    // Panel D
    {
        auto c = content(panelD);
        leds.setBounds(c.removeFromTop((int) (c.getHeight() * 0.34f)).reduced(14, 2));
        layoutGrid(c, { kDrive, kWidth, kMix, kOut }, 2);
    }
}

void PercussionAnalizerAudioProcessorEditor::renderBackground()
{
    const int W = getWidth(), H = getHeight();
    if (W <= 0 || H <= 0) return;
    background = juce::Image(juce::Image::ARGB, W, H, true);
    juce::Graphics g(background);
    juce::Random rnd(1234);

    g.fillAll(Colour(0xff0c0c0e));

    // laterales de madera
    for (auto r : { woodL, woodR })
    {
        g.setGradientFill(juce::ColourGradient(Colour(0xff7a3715), (float) r.getX(), 0.f, Colour(0xff2b1006), (float) r.getRight(), 0.f, false));
        g.fillRect(r);
        for (int x = r.getX(); x < r.getRight(); x += 2)
        {
            g.setColour((rnd.nextBool() ? Colours::black : Colour(0xffb5651f)).withAlpha(rnd.nextFloat() * 0.25f));
            g.drawVerticalLine(x, (float) r.getY(), (float) r.getBottom());
        }
    }

    // paneles
    drawPlate(g, panelA, Colour(0xff26272a), Colour(0xff0d0d0f), false, "PA-100", "ANALYZER  /  AUTO MIX", Colour(0xffe8dcc0), rnd);
    drawPlate(g, panelB, Colour(0xff3a3c42), Colour(0xff17181b), false, "PA-200", "PERCUSSION EQ", Colour(0xffe8dcc0), rnd);
    drawPlate(g, panelC, Colour(0xffc43a24), Colour(0xff7d1f10), true,  "PA-300", "VINTAGE COMPRESSOR", Colour(0xfffff1dc), rnd);
    drawPlate(g, panelD, Colour(0xff1c5f73), Colour(0xff0d3441), false, "PA-400", "OUTPUT  /  MASTER", Colour(0xffe6f4f6), rnd);

    // cabecera
    g.setGradientFill(juce::ColourGradient(Colour(0xff2f3034), 0.f, (float) header.getY(), Colour(0xff0f0f11), 0.f, (float) header.getBottom(), false));
    g.fillRect(header);
    g.setColour(Colours::black); g.drawHorizontalLine(header.getBottom() - 1, (float) header.getX(), (float) header.getRight());
    g.setColour(Colours::white.withAlpha(0.12f)); g.drawHorizontalLine(header.getY(), (float) header.getX(), (float) header.getRight());

    auto h = header.reduced(10, 6);
    if (logoImage.isValid())
        g.drawImage(logoImage, h.removeFromLeft(h.getHeight()).toFloat(), juce::RectanglePlacement::centred);
    h.removeFromLeft(8);
    auto tb = h.removeFromLeft(320);
    g.setFont(mkFont(23.f, true));
    g.setColour(Colours::black.withAlpha(0.7f)); g.drawText("PERCUSSION ANALIZER", tb.translated(1, 1).removeFromTop(30), juce::Justification::left);
    g.setColour(Colour(0xfff1e4c2));              g.drawText("PERCUSSION ANALIZER", tb.removeFromTop(30), juce::Justification::left);
    g.setFont(mkFont(10.5f));
    g.setColour(Colour(0xffb59a63));
    g.drawText(juce::String::fromUTF8("Mezcla y ecualización automática · Merengue y Bachata dominicana"), tb, juce::Justification::left);
    auto tag = header.reduced(14, 8).removeFromRight(190);
    g.setFont(mkFont(12.f, true));
    g.setColour(Colour(0xffd9822b));
    g.drawText(juce::String::fromUTF8("ORYMY · YMY RECORDS"), tag.removeFromTop(20), juce::Justification::right);
    g.setFont(mkFont(10.f));
    g.setColour(Colour(0xff8a8a8a));
    g.drawText("VST3  v1.0", tag, juce::Justification::right);
}

void PercussionAnalizerAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.drawImageAt(background, 0, 0);
}

void PercussionAnalizerAudioProcessorEditor::updatePresetText()
{
    presetButton.setButtonText(proc.getProgramName(proc.getCurrentPresetIndex()));
}

void PercussionAnalizerAudioProcessorEditor::stepPreset(int delta)
{
    const int n = (int) pa::getPresets().size();
    proc.loadPreset((proc.getCurrentPresetIndex() + delta + n) % n);
    updatePresetText();
}

void PercussionAnalizerAudioProcessorEditor::showPresetMenu()
{
    juce::PopupMenu root;
    const auto& list = pa::getPresets();
    const auto& inst = pa::getInstruments();
    const auto& sty = pa::getStyles();
    const int cur = proc.getCurrentPresetIndex();
    for (int i = 0; i < inst.size(); ++i)
    {
        juce::PopupMenu im;
        for (int s = 0; s < sty.size(); ++s)
        {
            juce::PopupMenu sm;
            for (size_t k = 0; k < list.size(); ++k)
                if (list[k].instrument == inst[i] && list[k].style == sty[s])
                    sm.addItem((int) k + 1, list[k].character, true, (int) k == cur);
            im.addSubMenu(sty[s], sm);
        }
        root.addSubMenu(inst[i], im);
    }
    juce::Component::SafePointer<PercussionAnalizerAudioProcessorEditor> safe(this);
    root.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&presetButton),
                       [safe](int result)
                       {
                           if (safe == nullptr || result <= 0) return;
                           safe->proc.loadPreset(result - 1);
                           safe->updatePresetText();
                       });
}

void PercussionAnalizerAudioProcessorEditor::timerCallback()
{
    vuLevel.setValue(proc.vuIn.load()); vuLevel.tick();
    vuGr.setValue(proc.grDb.load());    vuGr.tick();
    spectrum.tick();
    eqView.repaint();
    leds.tick();

    const int di = proc.detectedInstrument.load();
    if (proc.measuredCentroid.load() > 0.f && di >= 0)
        detectedLabel.setText(juce::String::fromUTF8("DETECTADO: ") + pa::getInstruments()[di].toUpperCase() + " (aprox.)", juce::dontSendNotification);
    else
        detectedLabel.setText(juce::String::fromUTF8("SIN SEÑAL - toca la percusión"), juce::dontSendNotification);

    const int st = proc.learnState.load();
    if (st == LearnMode::learning)
        stateLabel.setText("APRENDIENDO... " + juce::String((int) (proc.learnProgress.load() * 100.f)) + " %", juce::dontSendNotification);
    else if (st == LearnMode::locked)
        stateLabel.setText("AJUSTE FIJADO", juce::dontSendNotification);
    else
        stateLabel.setText("ANALIZANDO EN CONTINUO", juce::dontSendNotification);

    corrLabel.setText("Ganancia auto " + juce::String(proc.autoGainDb.load(), 1) + " dB   |   Brillo auto " + juce::String(proc.autoTiltDb.load(), 1) + " dB",
                      juce::dontSendNotification);
}
