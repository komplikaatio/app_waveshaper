#include "WaveshaperProcessor.h"
#include "WaveshaperEditor.h"

#include <algorithm>

static const std::vector<mrta::ParameterInfo> parameters
{
    { Param::ID::Enabled,  Param::Name::Enabled,  Param::Ranges::EnabledOff, Param::Ranges::EnabledOn, true },
    { Param::ID::Offset,   Param::Name::Offset,   Param::Units::Ms,  2.f,  Param::Ranges::OffsetMin,   Param::Ranges::OffsetMax,   Param::Ranges::OffsetInc,   Param::Ranges::OffsetSkw },
    { Param::ID::Depth,    Param::Name::Depth,    Param::Units::Ms,  2.f,  Param::Ranges::DepthMin,    Param::Ranges::DepthMax,    Param::Ranges::DepthInc,    Param::Ranges::DepthSkw },
    { Param::ID::Rate,     Param::Name::Rate,     Param::Units::Hz,  0.5f, Param::Ranges::RateMin,     Param::Ranges::RateMax,     Param::Ranges::RateInc,     Param::Ranges::RateSkw },
    { Param::ID::ModType,  Param::Name::ModType,  Param::Ranges::ModLabels, 0 }
};

WaveshaperProcessor::WaveshaperProcessor() :
    mrta::BaseProcessor(parameters),
    enableRamp(0.05f)
{
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
