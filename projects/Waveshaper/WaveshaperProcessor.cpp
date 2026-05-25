#include "WaveshaperProcessor.h"
#include "WaveshaperEditor.h"

#include <algorithm>

// Build the parameter list with all 8 waveshaping points
static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,           Param::Name::Enabled,           Param::Ranges::EnabledOff,      Param::Ranges::EnabledOn, true },

    { Param::ID::DryWet,            Param::Name::DryWet,            Param::Units::Percent,          Param::Ranges::DryWetDefault,
      Param::Ranges::DryWetMin,     Param::Ranges::DryWetMax,       Param::Ranges::DryWetInc,       Param::Ranges::DryWetSkw },
 
    { Param::ID::InputGain,         Param::Name::InputGain,         Param::Units::Db,               Param::Ranges::InputGainDefault,
      Param::Ranges::InputGainMin,  Param::Ranges::InputGainMax,    Param::Ranges::InputGainInc,    Param::Ranges::InputGainSkw },
 
    { Param::ID::OutputGain,        Param::Name::OutputGain,        Param::Units::Db,               Param::Ranges::OutputGainDefault,
      Param::Ranges::OutputGainMin, Param::Ranges::OutputGainMax,   Param::Ranges::OutputGainInc,   Param::Ranges::OutputGainSkw },
    
    // Waveshaping curve points - initialized to a linear transfer function by default
    { Param::ID::Point0,            Param::Name::Point0,            "",                             -1.0f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point1,            Param::Name::Point1,            "",                             -0.714f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point2,            Param::Name::Point2,            "",                             -0.429f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point3,            Param::Name::Point3,            "",                             -0.143f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point4,            Param::Name::Point4,            "",                             0.143f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point5,            Param::Name::Point5,            "",                             0.429f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point6,            Param::Name::Point6,            "",                             0.714f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
    
    { Param::ID::Point7,            Param::Name::Point7,            "",                             1.0f,
      Param::Ranges::PointMin,      Param::Ranges::PointMax,        Param::Ranges::PointInc,        Param::Ranges::PointSkw },
};

WaveshaperProcessor::WaveshaperProcessor() :
    mrta::BaseProcessor(parameters),
    enableRamp(0.05f),
    dryRamp(0.05f),
    wetRamp(0.05f)
{
    // Initialize waveshaper points to a linear transfer function
    for (int i = 0; i < 8; ++i)
    {
        waveshaperPoints[i] = -1.0f + (2.0f * i / 7.0f);
    }
    
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
        });

    registerParameterCallback(Param::ID::OutputGain,
        [this](float newValue, bool /*force*/)
        {
            outputGainDb = newValue;
            // Convert dB to linear
            outputGainLinear = std::pow(10.f, outputGainDb / 20.f);
        });
    
    // Register callbacks for waveshaper points
    registerParameterCallback(Param::ID::Point0,
        [this](float newValue, bool /*force*/) { waveshaperPoints[0] = newValue; });
    
    registerParameterCallback(Param::ID::Point1,
        [this](float newValue, bool /*force*/) { waveshaperPoints[1] = newValue; });
    
    registerParameterCallback(Param::ID::Point2,
        [this](float newValue, bool /*force*/) { waveshaperPoints[2] = newValue; });
    
    registerParameterCallback(Param::ID::Point3,
        [this](float newValue, bool /*force*/) { waveshaperPoints[3] = newValue; });
    
    registerParameterCallback(Param::ID::Point4,
        [this](float newValue, bool /*force*/) { waveshaperPoints[4] = newValue; });
    
    registerParameterCallback(Param::ID::Point5,
        [this](float newValue, bool /*force*/) { waveshaperPoints[5] = newValue; });
    
    registerParameterCallback(Param::ID::Point6,
        [this](float newValue, bool /*force*/) { waveshaperPoints[6] = newValue; });
    
    registerParameterCallback(Param::ID::Point7,
        [this](float newValue, bool /*force*/) { waveshaperPoints[7] = newValue; });
}

WaveshaperProcessor::~WaveshaperProcessor()
{
}

void WaveshaperProcessor::prepare(double newSampleRate, int samplesPerBlock)
{
    const unsigned int numChannels { static_cast<unsigned int>(std::max(getMainBusNumInputChannels(), getMainBusNumOutputChannels())) };
    
    enableRamp.prepare(newSampleRate);
    dryRamp.prepare(newSampleRate);
    wetRamp.prepare(newSampleRate);
    
    fxBuffer.setSize(static_cast<int>(numChannels), samplesPerBlock);
    fxBuffer.clear();
}

float WaveshaperProcessor::applyWaveshaper(float input) const
{
    // Clamp input to [-1, 1] range for table lookup
    const float clampedInput = std::clamp(input, -1.0f, 1.0f);
    
    // Map input [-1, 1] to index [0, 7]
    // input = -1.0 maps to index 0
    // input = 1.0 maps to index 7
    const float indexFloat = (clampedInput + 1.0f) * 3.5f; // (clampedInput + 1) / 2 * 7
    
    const int indexLower = static_cast<int>(std::floor(indexFloat));
    const int indexUpper = std::min(indexLower + 1, 7);
    
    // Linear interpolation between two points
    const float fraction = indexFloat - indexLower;
    const float output = waveshaperPoints[indexLower] * (1.0f - fraction) + 
                         waveshaperPoints[indexUpper] * fraction;
    
    return output;
}

void WaveshaperProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    const unsigned int numChannels { static_cast<unsigned int>(buffer.getNumChannels()) };
    const unsigned int numSamples { static_cast<unsigned int>(buffer.getNumSamples()) };
    
    // Copy input to FX buffer
    for (int ch = 0; ch < static_cast<int>(numChannels); ++ch)
        fxBuffer.copyFrom(ch, 0, buffer, ch, 0, static_cast<int>(numSamples));
    
    // Process each sample through the waveshaper chain
    auto** fxData = fxBuffer.getArrayOfWritePointers();
    
    for (unsigned int ch = 0; ch < numChannels; ++ch)
    {
        for (unsigned int n = 0; n < numSamples; ++n)
        {
            // Apply input gain
            float sample = fxData[ch][n] * inputGainLinear;
            
            // Apply waveshaping
            sample = applyWaveshaper(sample);
            
            // Apply output gain
            sample = sample * outputGainLinear;
            
            fxData[ch][n] = sample;
        }
    }
    
    // Apply enable ramp to FX buffer (bypass smoothing)
    enableRamp.applyGain(fxData, numChannels, numSamples);
    
    // Apply wet ramp to FX buffer
    wetRamp.applyGain(fxData, numChannels, numSamples);
    
    // Apply dry ramp to original signal
    auto** bufferData = buffer.getArrayOfWritePointers();
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
