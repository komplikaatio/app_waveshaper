#include "WaveshaperProcessor.h"
#include "WaveshaperEditor.h"

#include <algorithm>

static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,           Param::Name::Enabled,           Param::Ranges::EnabledOff,      Param::Ranges::EnabledOn, true },

    { Param::ID::DryWet,            Param::Name::DryWet,            Param::Units::Percent,          Param::Ranges::DryWetDefault,
      Param::Ranges::DryWetMin,     Param::Ranges::DryWetMax,       Param::Ranges::DryWetInc,       Param::Ranges::DryWetSkw },
 
    { Param::ID::InputGain,         Param::Name::InputGain,         Param::Units::Db,               Param::Ranges::InputGainDefault,
      Param::Ranges::InputGainMin,  Param::Ranges::InputGainMax,    Param::Ranges::InputGainInc,    Param::Ranges::InputGainSkw },
 
    { Param::ID::OutputGain,        Param::Name::OutputGain,        Param::Units::Db,               Param::Ranges::OutputGainDefault,
      Param::Ranges::OutputGainMin, Param::Ranges::OutputGainMax,   Param::Ranges::OutputGainInc,   Param::Ranges::OutputGainSkw },
};

WaveshaperProcessor::WaveshaperProcessor() :
    mrta::BaseProcessor(parameters),
    enableRamp(0.05f)
{
    for (int i = 0; i < NumPoints; ++i)
    {
        // Initializing each point
    }
    registerParameterCallback(Param::ID::Enabled,
        [this](float newValue, bool force)
        {
            enabled = newValue > 0.5f;
            enableRamp.setTarget(enabled ? 1.f : 0.f, force);
        });

    registerParameterCallback(Param::ID::Offset,
        [this] (float newValue, bool /*force*/)
        {

        });

    registerParameterCallback(Param::ID::Depth,
        [this](float newValue, bool /*force*/)
        {
      
        });

    registerParameterCallback(Param::ID::Rate,
        [this] (float newValue, bool /*force*/)
        {

        });

    registerParameterCallback(Param::ID::ModType,
        [this](float newValue, bool /*force*/)
        {

        });
}

WaveshaperProcessor::~WaveshaperProcessor()
{
}

void WaveshaperProcessor::prepare(double sampleRate, int samplesPerBlock)
{

}

void WaveshaperProcessor::process(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{

}

juce::AudioProcessorEditor* WaveshaperProcessor::createEditor()
{
    return new WaveshaperEditor(*this);
}

CREATE_PLUGIN(WaveshaperProcessor)
