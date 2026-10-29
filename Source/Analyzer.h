#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <atomic>
#include <cmath>

namespace pa
{
class SpectrumAnalyzer
{
public:
    static constexpr int fftOrder = 11;
    static constexpr int fftSize  = 1 << fftOrder;
    static constexpr int numBands = 64;

    struct Frame { float rmsDb = -120.f; float centroid = 2000.f; };

    SpectrumAnalyzer()
        : fft(fftOrder),
          window((size_t) fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        for (auto& b : bands) b.store(-100.f);
        fifo.fill(0.f);
        data.fill(0.f);
        bandLo.fill(1);
        bandHi.fill(1);
    }

    static double bandFreq(int i) { return 30.0 * std::pow(18000.0 / 30.0, (double) i / (double) (numBands - 1)); }

    void prepare(double sr)
    {
        sampleRate = sr;
        fifoIndex = 0;
        const double fMin = 30.0, fMax = 18000.0;
        for (int i = 0; i < numBands; ++i)
        {
            const double f0 = fMin * std::pow(fMax / fMin, (double) i / numBands);
            const double f1 = fMin * std::pow(fMax / fMin, (double) (i + 1) / numBands);
            const int b0 = juce::jlimit(1, fftSize / 2 - 1, (int) std::floor(f0 * fftSize / sr));
            const int b1 = juce::jlimit(b0, fftSize / 2 - 1, (int) std::ceil(f1 * fftSize / sr));
            bandLo[(size_t) i] = b0;
            bandHi[(size_t) i] = b1;
        }
    }

    template <typename F>
    void pushSample(float x, F&& onFrame)
    {
        fifo[(size_t) fifoIndex++] = x;
        if (fifoIndex >= fftSize)
        {
            fifoIndex = 0;
            onFrame(analyseFrame());
        }
    }

    std::array<std::atomic<float>, numBands> bands;

private:
    Frame analyseFrame()
    {
        double sumSq = 0.0;
        for (int i = 0; i < fftSize; ++i) sumSq += (double) fifo[(size_t) i] * fifo[(size_t) i];

        Frame f;
        f.rmsDb = 10.f * std::log10((float) (sumSq / fftSize) + 1.0e-12f);

        std::copy(fifo.begin(), fifo.end(), data.begin());
        std::fill(data.begin() + fftSize, data.end(), 0.f);
        window.multiplyWithWindowingTable(data.data(), (size_t) fftSize);
        fft.performFrequencyOnlyForwardTransform(data.data());

        const double binHz = sampleRate / fftSize;
        double num = 0.0, den = 0.0;
        for (int k = 2; k < fftSize / 2; ++k)
        {
            const double hz = k * binHz;
            if (hz < 60.0 || hz > 16000.0) continue;
            const double m2 = (double) data[(size_t) k] * data[(size_t) k];
            num += hz * m2;
            den += m2;
        }
        f.centroid = den > 1.0e-9 ? (float) (num / den) : 2000.f;

        const float norm = 4.f / (float) fftSize;
        for (int i = 0; i < numBands; ++i)
        {
            double acc = 0.0;
            const int b0 = bandLo[(size_t) i], b1 = bandHi[(size_t) i];
            for (int k = b0; k <= b1; ++k) acc += (double) data[(size_t) k] * data[(size_t) k];
            acc /= (double) (b1 - b0 + 1);
            const float db = 10.f * std::log10((float) acc * norm * norm + 1.0e-12f);
            const float old = bands[(size_t) i].load();
            bands[(size_t) i].store(old * 0.6f + db * 0.4f);
        }
        return f;
    }

    double sampleRate = 48000.0;
    int fifoIndex = 0;
    juce::dsp::FFT fft;
    juce::dsp::WindowingFunction<float> window;
    std::array<float, (size_t) fftSize> fifo;
    std::array<float, (size_t) fftSize * 2> data;
    std::array<int, (size_t) numBands> bandLo, bandHi;
};
}
