#pragma once

#include <BaseProcessor.h>

#include <Ramp.h>
#include <array>
#include <cmath>

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String DryWet { "dry_wet" };
        static const juce::String InputGain { "input_gain" }; 
        static const juce::String OutputGain { "output_gain" };
        
        // Waveshaping curve points
        static const juce::String Point0 { "point_0" };
        static const juce::String Point1 { "point_1" };
        static const juce::String Point2 { "point_2" };
        static const juce::String Point3 { "point_3" };
        static const juce::String Point4 { "point_4" };
        static const juce::String Point5 { "point_5" };
        static const juce::String Point6 { "point_6" };
        static const juce::String Point7 { "point_7" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String DryWet { "Dry/Wet" };
        static const juce::String InputGain { "Drive" };
        static const juce::String OutputGain { "Output Gain" };
        
        static const juce::String Point0 { "Point 0" };
        static const juce::String Point1 { "Point 1" };
        static const juce::String Point2 { "Point 2" };
        static const juce::String Point3 { "Point 3" };
        static const juce::String Point4 { "Point 4" };
        static const juce::String Point5 { "Point 5" };
        static const juce::String Point6 { "Point 6" };
        static const juce::String Point7 { "Point 7" };
    }

    namespace Ranges
    {
        static const juce::String EnabledOff { "Off" };
        static const juce::String EnabledOn { "On" };

        // Dry/Wet  [0, 1]  linear
        static constexpr float DryWetMin { 0.f };
        static constexpr float DryWetMax { 1.f };
        static constexpr float DryWetInc { 0.01f };
        static constexpr float DryWetSkw { 1.f };
        static constexpr float DryWetDefault { 1.f };

        static constexpr float InputGainMin { -30.f };
        static constexpr float InputGainMax { 6.f };
        static constexpr float InputGainInc { 0.1f };
        static constexpr float InputGainSkw { 1.f };
        static constexpr float InputGainDefault { 0.f };

        static constexpr float OutputGainMin { -30.f };
        static constexpr float OutputGainMax { 6.f };
        static constexpr float OutputGainInc { 0.1f };
        static constexpr float OutputGainSkw { 1.f };
        static constexpr float OutputGainDefault { 0.f };

        static constexpr float PointMin { -1.f };
        static constexpr float PointMax { 1.f };
        static constexpr float PointInc { 0.001f };
        static constexpr float PointSkw { 1.f };
    }

    namespace Units
    {
        static const juce::String Db      { "dB" };
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
    DSP::Ramp<float> enableRamp;
    DSP::Ramp<float> dryRamp;
    DSP::Ramp<float> wetRamp;
    
    bool enabled { true };
    float dryWet { 1.f };
    float inputGainDb { 0.f };
    float outputGainDb { 0.f };
    float inputGainLinear { 1.f };
    float outputGainLinear { 1.f };
    
    // Waveshaping curve: 8 points defining the transfer function
    // Points span from -1.0 to 1.0, equally spaced
    std::array<float, 8> waveshaperPoints;
    
    juce::AudioBuffer<float> fxBuffer;

    // Helper function to apply waveshaping using linear interpolation
    float applyWaveshaper(float input) const;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperProcessor)
};