#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <ParameterComponents.h>
#include <Waveshaper.h>
#include "WSPointComponent.h"
#include "WaveshaperProcessor.h"
#include "WSLaf.h"
#include "XYPad.h"

class WaveshaperEditor final :  public juce::AudioProcessorEditor,
                                public juce::Timer
{
public:
    WaveshaperEditor(WaveshaperProcessor& p);
    ~WaveshaperEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void timerCallback() override;

private:
    static constexpr size_t NumPoints = DSP::Waveshaper::WS_POINTS;

    WaveshaperProcessor& processor;
    DSP::Waveshaper& waveshaper;
    WSLaf wsLaf;

    // Sliders
    mrta::ParameterButton enabledButton;
    mrta::ParameterSlider dryWetSlider;
    mrta::ParameterSlider inputGainSlider;
    mrta::ParameterSlider outputGainSlider;
    mrta::ParameterSlider yLeftSlider;
    mrta::ParameterSlider yRightSlider;
    std::vector<std::unique_ptr<WSPointComponent>> pointComponents;

    // Labels
    juce::Rectangle<int> titleBounds;
    juce::Label dryWetLabel     { "dryWetLabel"     , "MIX" };
    juce::Label inputGainLabel  { "inGainLabel"     , "DRIVE" };
    juce::Label outputGainLabel { "outGainLabel"    , "OUTPUT" };
    juce::Label xLabel          { "xLabel"          , "X" };
    juce::Label yLabel          { "yLabel"          , "Y" };
    juce::Label controlYLabel   { "controlYLabel"   , "CONTROL Y" };
    juce::Label controlXLabel   { "controlXLabel"   , "CONTROL X" };

    // Display
    juce::Rectangle<int> displayBounds;
    std::vector<std::unique_ptr<XYPad>> pads;
    std::vector<float> currentXs;
    std::vector<float> currentYs;

    // Separator positions (set in resized, drawn in paint)
    int separatorTopY    { 0 };  // above point components (below display)
    int separatorBottomY { 0 };  // below point components (above bottom knobs)

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperEditor)
};