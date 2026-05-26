#pragma once

#include <BaseProcessor.h>

#include <Waveshaper.h>
#include <Ramp.h>
#include <array>
#include <cmath>

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled    { "enabled" };
        static const juce::String DryWet     { "dry_wet" };
        static const juce::String InputGain  { "input_gain" };
        static const juce::String OutputGain { "output_gain" };
        static const juce::String LeftY      { "leftY" };
        static const juce::String RightY     { "rightY" };

        inline juce::String pointX     (int i) { return "point_" + juce::String(i) + "X"; }
        inline juce::String pointXRange(int i) { return "point_" + juce::String(i) + "XRange"; }
        inline juce::String pointXRate (int i) { return "point_" + juce::String(i) + "XRate"; }
        inline juce::String pointY     (int i) { return "point_" + juce::String(i) + "Y"; }
        inline juce::String pointYRange(int i) { return "point_" + juce::String(i) + "YRange"; }
        inline juce::String pointYRate (int i) { return "point_" + juce::String(i) + "YRate"; }
    }

    namespace Name
    {
        static const juce::String Enabled    { "Enabled" };
        static const juce::String DryWet     { "Dry/Wet" };
        static const juce::String InputGain  { "Input Gain" };
        static const juce::String OutputGain { "Output Gain" };
        static const juce::String LeftY      { "Left Y" };
        static const juce::String RightY     { "Right Y" };

        inline juce::String pointX     (int i) { return "X " + juce::String(i); }
        inline juce::String pointXRange(int i) { return "X " + juce::String(i) + " Range"; }
        inline juce::String pointXRate (int i) { return "X " + juce::String(i) + " Rate"; }
        inline juce::String pointY     (int i) { return "Y " + juce::String(i); }
        inline juce::String pointYRange(int i) { return "Y " + juce::String(i) + " Range"; }
        inline juce::String pointYRate (int i) { return "Y " + juce::String(i) + " Rate"; }
    }

    namespace Ranges
    {
        static const juce::String EnabledOff        { "Off" };
        static const juce::String EnabledOn         { "On" };

        // Dry/Wet  [0, 1]  linear
        static constexpr float DryWetMin        { 0.f };
        static constexpr float DryWetMax        { 1.f };
        static constexpr float DryWetInc        { 0.01f };
        static constexpr float DryWetSkw        { 1.f };
        static constexpr float DryWetDef        { 1.f };

        static constexpr float InputGainMin     { -30.f };
        static constexpr float InputGainMax     { 6.f };
        static constexpr float InputGainInc     { 0.1f };
        static constexpr float InputGainSkw     { 1.f };
        static constexpr float InputGainDef     { 0.f };

        static constexpr float OutputGainMin    { -30.f };
        static constexpr float OutputGainMax    { 6.f };
        static constexpr float OutputGainInc    { 0.1f };
        static constexpr float OutputGainSkw    { 1.f };
        static constexpr float OutputGainDef    { 0.f };

        static constexpr float PointXMin        { 0.f };
        static constexpr float PointXMax        { 1.f };
        static constexpr float PointXInc        { 0.001f };
        static constexpr float PointXSkw        { 1.f };
        static constexpr float PointXDef        { 0.5f };

        static constexpr float PointYMin        { -1.f };
        static constexpr float PointYMax        { 1.f };
        static constexpr float PointYInc        { 0.001f };
        static constexpr float PointYSkw        { 1.f };

        static constexpr float PointRangeMin    { 0.f };
        static constexpr float PointRangeMax    { 1.f };
        static constexpr float PointRangeInc    { 0.01f };
        static constexpr float PointRangeSkw    { 0.5f };
        static constexpr float PointRangeDef    { 0.f };

        static constexpr float PointRateMin     { 0.1f };
        static constexpr float PointRateMax     { 2000.f };
        static constexpr float PointRateInc     { 0.1f };
        static constexpr float PointRateSkw     { 0.3f };
        static constexpr float PointRateDef     { 250.f };

        static constexpr float LeftYDef         { -1.f };
        static constexpr float RightYDef        { -1.f };
    }

    namespace Units
    {
        static const juce::String Db      { "dB" };
        static const juce::String Ms      { "ms" };
        static const juce::String Percent { "%" };
    }
}

class WaveshaperProcessor : public mrta::BaseProcessor
{
public:
    static const unsigned int MaxChannels { 2 };
    static const unsigned int MaxProcessBlockSamples{ 32 };

    WaveshaperProcessor();
    ~WaveshaperProcessor() override;

    // Called before processing starts
    void prepare(double sampleRate, int samplesPerBlock) override;

    // Audio stream callback
    void process(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // Creates the GUI
    juce::AudioProcessorEditor* createEditor() override;

private:

    DSP::Waveshaper waveshaper;

    DSP::Ramp<float> enableRamp;
    DSP::Ramp<float> dryRamp;
    DSP::Ramp<float> wetRamp;
    DSP::Ramp<float> inputGainRamp;
    DSP::Ramp<float> outputGainRamp;
    
    bool enabled            { true };
    float dryWet            { 1.f };
    float inputGainDb       { 0.f };
    float outputGainDb      { 0.f };
    float inputGainLinear   { 1.f };
    float outputGainLinear  { 1.f };
    
    juce::AudioBuffer<float> fxBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperProcessor)
};