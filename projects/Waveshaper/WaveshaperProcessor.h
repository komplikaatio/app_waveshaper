#pragma once

#include <BaseProcessor.h>

#include <Ramp.h>

namespace Param
{
    namespace ID
    {
        static const juce::String Enabled { "enabled" };
        static const juce::String DryWet { "dry_wet" };
        static const juce::String InputGain { "input_gain" }; 
        static const juce::String OutputGain { "output_gain" };
    }

    namespace Name
    {
        static const juce::String Enabled { "Enabled" };
        static const juce::String DryWet { "Dry/Wet" };
        static const juce::String InputGain { "Drive" };
        static const juce::String OutputGain { "Output Gain" };
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
    bool enabled { true };

    void buildParameters(std::vector<mrta::ParameterInfo>& parameters);

    static constexpr int NumPoints { 4 };

    juce::AudioBuffer<float> fxBuffer;

    


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperProcessor)
};