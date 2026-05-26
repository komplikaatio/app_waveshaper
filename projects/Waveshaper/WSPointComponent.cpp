#include "WSPointComponent.h"

WSPointComponent::WSPointComponent(
    size_t pointIdx,
    juce::AudioProcessorValueTreeState &apvts,
    DSP::Waveshaper &waveshaper)
    :
    pointIdx        { pointIdx },
    waveshaper      { waveshaper },

    // Initialize sliders
    xSlider         { Param::ID::pointX     (pointIdx)   , apvts },
    xRangeSlider    { Param::ID::pointXRange(pointIdx)   , apvts },
    xRateSlider     { Param::ID::pointXRate (pointIdx)   , apvts },
    ySlider         { Param::ID::pointY     (pointIdx)   , apvts },
    yRangeSlider    { Param::ID::pointYRange(pointIdx)   , apvts },
    yRateSlider     { Param::ID::pointYRate (pointIdx)   , apvts }
{
    // Set sliders style
    xSlider         .setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    xRangeSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    xRateSlider     .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    ySlider         .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    yRangeSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    yRateSlider     .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    // Add components
    addAndMakeVisible(xSlider);
    addAndMakeVisible(xRangeSlider);
    addAndMakeVisible(xRateSlider);
    addAndMakeVisible(ySlider);
    addAndMakeVisible(yRangeSlider);
    addAndMakeVisible(yRateSlider);

    addAndMakeVisible(xLabel);
    addAndMakeVisible(xRangeLabel);
    addAndMakeVisible(xRateLabel);
    addAndMakeVisible(yLabel);
    addAndMakeVisible(yRangeLabel);
    addAndMakeVisible(yRateLabel);
}