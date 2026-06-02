#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <ParameterComponents.h>
#include <Waveshaper.h>

class WSPointComponent : public juce::Component
{
public:
    WSPointComponent(
        size_t pointIdx,
        juce::AudioProcessorValueTreeState& apvts,
        DSP::Waveshaper& waveshaper
    );

    void paint (juce::Graphics& g) override;
    void resized() override;

    mrta::ParameterSlider& getXSlider();
    mrta::ParameterSlider& getYSlider();

private:
    size_t pointIdx;
    DSP::Waveshaper& waveshaper;

    mrta::ParameterSlider xSlider;
    mrta::ParameterSlider xRangeSlider;
    mrta::ParameterSlider xRateSlider;

    mrta::ParameterSlider ySlider;
    mrta::ParameterSlider yRangeSlider;
    mrta::ParameterSlider yRateSlider;

    juce::Label RangeLabel { "", "RANGE" };
    juce::Label RateLabel  { "", "RATE" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WSPointComponent)
};