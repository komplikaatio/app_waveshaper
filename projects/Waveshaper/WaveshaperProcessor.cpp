#include "WaveshaperProcessor.h"
#include "WaveshaperEditor.h"

#include <algorithm>

// Build the parameter list with all 8 waveshaping points
static const std::vector<mrta::ParameterInfo> parameters = []()
{
    std::vector<mrta::ParameterInfo> p
    {
        { Param::ID::Enabled,    Param::Name::Enabled,    Param::Ranges::EnabledOff,     Param::Ranges::EnabledOn, true },

        { Param::ID::DryWet,     Param::Name::DryWet,     Param::Units::Percent,         Param::Ranges::DryWetDef,
          Param::Ranges::DryWetMin,    Param::Ranges::DryWetMax,    Param::Ranges::DryWetInc,    Param::Ranges::DryWetSkw },

        { Param::ID::InputGain,  Param::Name::InputGain,  Param::Units::Db,              Param::Ranges::InputGainDef,
          Param::Ranges::InputGainMin, Param::Ranges::InputGainMax, Param::Ranges::InputGainInc, Param::Ranges::InputGainSkw },

        { Param::ID::OutputGain, Param::Name::OutputGain, Param::Units::Db,              Param::Ranges::OutputGainDef,
          Param::Ranges::OutputGainMin, Param::Ranges::OutputGainMax, Param::Ranges::OutputGainInc, Param::Ranges::OutputGainSkw },

        { Param::ID::LeftY,      Param::Name::LeftY,      "",           Param::Ranges::LeftYDef,
          Param::Ranges::PointYMin, Param::Ranges::PointYMax, Param::Ranges::PointYInc, Param::Ranges::PointYSkw },

          { Param::ID::RightY,      Param::Name::RightY,  "",           Param::Ranges::RightYDef,
          Param::Ranges::PointYMin, Param::Ranges::PointYMax, Param::Ranges::PointYInc, Param::Ranges::PointYSkw }
    };

    constexpr int N = DSP::Waveshaper::WS_POINTS;

    for (int i = 0; i < N; ++i)
    {
        // X defaults evenly spaced 0..1; adjust if your points have bespoke defaults
        const float xDef = static_cast<float>(i) / static_cast<float>(N - 1);

        p.push_back({ Param::ID::pointX(i),      Param::Name::pointX(i),      "", Param::Ranges::PointXDef,
                      Param::Ranges::PointXMin, Param::Ranges::PointXMax, Param::Ranges::PointXInc, Param::Ranges::PointXSkw });

        p.push_back({ Param::ID::pointXRange(i), Param::Name::pointXRange(i), "", Param::Ranges::PointRangeDef,
                      Param::Ranges::PointRangeMin, Param::Ranges::PointRangeMax, Param::Ranges::PointRangeInc, Param::Ranges::PointRangeSkw });

        p.push_back({ Param::ID::pointXRate(i),  Param::Name::pointXRate(i),  "", Param::Ranges::PointRateDef,
                      Param::Ranges::PointRateMin, Param::Ranges::PointRateMax, Param::Ranges::PointRateInc, Param::Ranges::PointRateSkw });

        p.push_back({ Param::ID::pointY(i),      Param::Name::pointY(i),      "", static_cast<float>(i + 1) / (N + 1) * 2.f - 1.f,
                      Param::Ranges::PointYMin, Param::Ranges::PointYMax, Param::Ranges::PointYInc, Param::Ranges::PointYSkw });

        p.push_back({ Param::ID::pointYRange(i), Param::Name::pointYRange(i), "", Param::Ranges::PointRangeDef,
                      Param::Ranges::PointRangeMin, Param::Ranges::PointRangeMax, Param::Ranges::PointRangeInc, Param::Ranges::PointRangeSkw });

        p.push_back({ Param::ID::pointYRate(i),  Param::Name::pointYRate(i),  "", Param::Ranges::PointRateDef,
                      Param::Ranges::PointRateMin, Param::Ranges::PointRateMax, Param::Ranges::PointRateInc, Param::Ranges::PointRateSkw });
    }

    return p;
}();

WaveshaperProcessor::WaveshaperProcessor() :
    mrta::BaseProcessor(parameters),
    enableRamp(0.05f),
    dryRamp(0.05f),
    wetRamp(0.05f),
    inputGainRamp(0.05f),
    outputGainRamp(0.05f)
{

    registerParameterCallback(Param::ID::Enabled,
        [this](float newValue, bool force)
        {
            enabled = newValue > 0.5f;
            enableRamp.setTarget(enabled ? 1.f : 0.f, force);
            
            // Update dry/wet ramping
            const float enabledValue { enabled ? 1.f : 0.f };
            wetRamp.setTarget(dryWet * enabledValue, force);
            dryRamp.setTarget((1.f - dryWet) * enabledValue + (1.f - enabledValue), force);
        });

    registerParameterCallback(Param::ID::DryWet,
        [this](float newValue, bool force)
        {
            dryWet = newValue;
            const float enabledValue { enabled ? 1.f : 0.f };
            // Smoothly transition the wet gain
            wetRamp.setTarget(dryWet * enabledValue, force);
            // Dry gain is 1 when disabled, then (1-wet) when enabled
            dryRamp.setTarget((1.f - dryWet) * enabledValue + (1.f - enabledValue), force);
        });

    registerParameterCallback(Param::ID::InputGain,
        [this](float newValue, bool /*force*/)
        {
            inputGainDb = newValue;
            // Convert dB to linear: linear = 10^(dB/20)
            inputGainLinear = std::pow(10.f, inputGainDb / 20.f);
            inputGainRamp.setTarget(inputGainLinear);
        });

    registerParameterCallback(Param::ID::OutputGain,
        [this](float newValue, bool /*force*/)
        {
            outputGainDb = newValue;
            // Convert dB to linear
            outputGainLinear = std::pow(10.f, outputGainDb / 20.f);
            outputGainRamp.setTarget(outputGainLinear);
        });
    
    // Register callbacks for waveshaper points
    for (auto pointIdx = 0; pointIdx < DSP::Waveshaper::WS_POINTS; ++pointIdx)
    {
        registerParameterCallback(Param::ID::pointX(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setX(pointIdx, newValue); });

        registerParameterCallback(Param::ID::pointXRange(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setXRandomRange(pointIdx, newValue); });

        registerParameterCallback(Param::ID::pointXRate(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setXRandomRate(pointIdx, newValue); });

        registerParameterCallback(Param::ID::pointY(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setY(pointIdx, newValue); });

        registerParameterCallback(Param::ID::pointYRange(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setYRandomRange(pointIdx, newValue); });

        registerParameterCallback(Param::ID::pointYRate(pointIdx),
        [this, pointIdx](float newValue, bool /*force*/) { waveshaper.setYRandomRate(pointIdx, newValue); });
    }
}

WaveshaperProcessor::~WaveshaperProcessor()
{
}

void WaveshaperProcessor::prepare(double newSampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    
    enableRamp      .prepare(newSampleRate);
    dryRamp         .prepare(newSampleRate);
    wetRamp         .prepare(newSampleRate);
    inputGainRamp   .prepare(newSampleRate);
    outputGainRamp  .prepare(newSampleRate);
    
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

void WaveshaperProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };
    
    // Copy input to FX buffer
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));

    auto* const* bufferData = buffer.getArrayOfWritePointers();
    auto* const* fxData = fxBuffer.getArrayOfWritePointers();

    // Apply input gain
    inputGainRamp.applyGain(fxData, numChannels, numSamples);
    
    // Process waveshaper
    waveshaper.process(fxBuffer);

    // Apply output gain
    outputGainRamp.applyGain(fxData, numChannels, numSamples);
    
    // Apply enable ramp to FX buffer (bypass smoothing)
    enableRamp.applyGain(fxData, numChannels, numSamples);
    
    // Apply wet ramp to FX buffer
    wetRamp.applyGain(fxData, numChannels, numSamples);
    
    // Apply dry ramp to original signal
    dryRamp.applyGain(bufferData, numChannels, numSamples);
    
    // Mix dry and wet signals
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        buffer.addFrom(ch, 0, fxBuffer, ch, 0, static_cast<int>(numSamples));
}

juce::AudioProcessorEditor* WaveshaperProcessor::createEditor()
{
    return new WaveshaperEditor(*this);
}

CREATE_PLUGIN(WaveshaperProcessor)
