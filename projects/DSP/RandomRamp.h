#pragma once

#include <cmath>
#include <random>
#include <cassert>

namespace DSP
{

template<typename F>
class RandomRamp
{
public:
    // Default RandomRamp time of 50ms
    static constexpr F DefaultRampTime { static_cast<F>(0.05) };

    static_assert(std::is_floating_point<F>::value, "Only supports floating point!");

    RandomRamp(F rampTimeSec) :
        rampTime { std::fmax(rampTimeSec, minRampTime) },
        gen(rd()),
        randomDist(0.0f, 1.0f)
    { }

    ~RandomRamp() { }

    // Default ctor
    RandomRamp() :
        rampTime { DefaultRampTime },
        gen(rd()),
        randomDist(0.0f, 1.0f)
    { }

    // No copy semantics
    RandomRamp(const RandomRamp&) = delete;
    const RandomRamp& operator=(const RandomRamp&) = delete;

    // No move semantics
    RandomRamp(RandomRamp&&) = delete;
    const RandomRamp& operator=(RandomRamp&&) = delete;

    // Update sample rate of the RandomRamp time, optionally allowing to
    // skip to RandomRamp value
    void prepare(double newSampleRate, bool skipRamp = false, F skipRampToValue = static_cast<F>(0))
    {
        sampleRate = newSampleRate;
        if (skipRamp)
            setTarget(skipRampToValue, true);
        else
            setTarget(targetValue);
    }

    // Set the target value to the RandomRamp
    // optionally allowing to skip the RandomRamp
    void setTarget(F newTargetValue, bool skipRamp = false)
    {
        if (std::abs(newTargetValue - currentValue) > minDelta)
        {
            targetValue = newTargetValue;
            rampStep = (targetValue - currentValue) / static_cast<F>(sampleRate * rampTime);
        }

        if (skipRamp)
            currentValue = targetValue = newTargetValue;
    }

    // Set new Random time
    void setRampTime(F newRampTimeSec)
    {
        rampTime = std::fmax(newRampTimeSec, minRampTime);
    }

    float getNext()
    {
        const F targetDelta { std::fabs(targetValue - currentValue) };
        if ((targetDelta > std::fabs(static_cast<F>(2) * rampStep)) && (std::fabs(rampStep) > minDelta))
            currentValue += rampStep;
        else
        {
            currentValue = targetValue;

            // Generate new random target
            setTarget(randomDist(gen) * (max - min) + min);
        }

        return currentValue;
    }

    void setRange(F newMin, F newMax)
    {
        assert ( newMin <= newMax);
        min = newMin;
        max = newMax;
    }

    void setMin(F newMin)
    {
        assert(newMin <= max);
        min = newMin;
    }

    void setMax(F newMax)
    {
        assert(newMax >= min);
        max = newMax;
    }

    // Minimum RandomRamp time in secondes
    static constexpr F minRampTime { static_cast<F>(1e-3) }; // 1ms

    // Minimun absolute differente between target and current value
    static constexpr F minDelta { static_cast<F>(1e-9) };

private:
    double sampleRate { 48000.0 };
    F rampTime;
    F rampStep { static_cast<F>(0) };
    F targetValue { static_cast<F>(0) };
    F currentValue { static_cast<F>(0) };
    F min { 0.f };
    F max { 1.f };

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_real_distribution<float> randomDist;
};

}