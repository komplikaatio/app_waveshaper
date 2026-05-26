#pragma once

#include <BaseProcessor.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <Waveshaper.h>
#include "WSPointComponent.h"

class WaveshaperEditor final : public juce::AudioProcessorEditor
{
public:
    WaveshaperEditor(mrta::BaseProcessor&);
    ~WaveshaperEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;
    DSP::Waveshaper& waveshaper;

    // Sliders
    mrta::ParameterButton enabledButton;
    mrta::ParameterSlider dryWetSlider;
    mrta::ParameterSlider inputGainSlider;
    mrta::ParameterSlider outputGainSlider;
    mrta::ParameterSlider yLeftSlider;
    mrta::ParameterSlider yRightSlider;
    std::vector<WSPointComponent> pointComponents;

    // Labels
    juce::Label titleLabel      { "titleLabel"      , "W4V3 $H4P3R "};
    juce::Label dryWetLabel     { "dryWetLabel"     , "DRY / WET" };
    juce::Label inputGainLabel  { "inGainLabel"     , "DRIVE" };
    juce::Label outputGainLabel { "outGainLabel"    , "OUTPUT\nGAIN" };
    juce::Label xLabel          { "xLabel"          , "X" };
    juce::Label yLabel          { "yLabel"          , "Y" };
    juce::Label xRangeLabel     { "xRangelabel"     , "RANGE" };
    juce::Label yRangeLabel     { "yRangelabel"     , "RANGE" };
    juce::Label xRateLabel      { "xRatelabel"      , "RATE" };
    juce::Label yRateLabel      { "yRatelabel"      , "RATE" };

    // Display
    juce::Rectange<int> displayArea;
    std::vector<XYPad> pads;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperEditor)
};
