#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include "Ramp.h"
#include "RandomRamp.h"

namespace DSP
{

class Waveshaper
{
public:
    struct Point
    {
        float x                         { 0.f };
        float y                         { 0.f };
        Ramp<float> xRamp               { 0.01f };
        Ramp<float> yRamp               { 0.01f };
        RandomRamp<float> xRandomRamp   { 0.01f };
        RandomRamp<float> yRandomRamp   { 0.01f };
    };
    static constexpr size_t WS_POINTS   { 7 }; // Good to use odd number so one point sits at x = 0.0 (DC)
    static constexpr float  X_MIN       { -1.f };
    static constexpr float  X_MAX       { 1.f };
    static constexpr float  Y_MIN       { -1.f };
    static constexpr float  Y_MAX       { 1.f };

    Waveshaper();

    void prepare(double newSampleRate);

    void process(juce::AudioBuffer<float>& buffer);

    // Parameter setters
    void setX           (size_t pointIdx, float newValue);
    void setY           (size_t pointIdx, float newValue);
    void setXRandomRange(size_t pointIdx, float newValue);
    void setYRandomRange(size_t pointIdx, float newValue);
    void setXRandomRate (size_t pointIdx, float newValueMs);
    void setYRandomRate (size_t pointIdx, float newValueMs);
    void setLeftY       (float newValue);
    void setRightY      (float newValue);

    float getCurrentX   (size_t pointIdx);
    float getCurrentY   (size_t pointIdx);

private:
    // Size of each point x range
    static constexpr auto SizeX = 1.f / static_cast<float>(WS_POINTS) * 2.f;
    static constexpr auto SizeXInv = 1.f / SizeX;

    // Pad with left and right Y-only points
    std::array<Point, WS_POINTS + 2> points;

    // Atomic access to positions updated once per block
    std::array<std::atomic<float>, WS_POINTS> currentXs;
    std::array<std::atomic<float>, WS_POINTS> currentYs;

    float applyWaveshaping(float inputSample);
    void step();
    void updateCurrentPositions();
    Point& getPoint(size_t pointIdx);
};

}