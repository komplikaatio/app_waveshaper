#pragma once

#include <BaseProcessor.h>
#include <juce_gui_basics/juce_gui_basics.h>

class WaveshaperEditor final : public juce::AudioProcessorEditor
{
public:
    WaveshaperEditor(mrta::BaseProcessor&);
    ~WaveshaperEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;

    // Sliders
    std::unique_ptr<mrta::ParameterButton> enabledButton;
    std::unique_ptr<mrta::ParameterSlider> dryWetSlider;
    std::unique_ptr<mrta::ParameterSlider> inputGainSlider;
    std::unique_ptr<mrta::ParameterSlider> outputGainSlider;
    std::vector<std::unique_ptr<mrta::ParameterSlider>> pointXSliders;
    std::vector<std::unique_ptr<mrta::ParameterSlider>> pointYSliders;

    // Labels
    juce::Label titleLabel      { "titleLabel"      , "W4V3 $H4P3R "};
    juce::Label dryWetLabel     { "dryWetLabel"     , "DRY / WET" };
    juce::Label inputGainLabel  { "inGainLabel"     , "DRIVE" };
    juce::Label outputGainLabel { "outGainLabel"    , "OUTPUT\nGAIN" };
    juce::Label xLabel          { "xLabel"          , "X" };
    juce::Label yLabel          { "yLabel"          , "Y" };

    // Display
    juce::Rectange<int> displayArea;
    std::vector<XYPad> pads;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperEditor)
};
