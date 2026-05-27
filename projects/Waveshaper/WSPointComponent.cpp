#include "WSPointComponent.h"
#include "WaveshaperProcessor.h"

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
    ySlider         .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yRangeSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    yRateSlider     .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    xSlider         .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    xRangeSlider    .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    xRateSlider     .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    ySlider         .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    yRangeSlider    .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    yRateSlider     .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);

    // Add components
    addAndMakeVisible(xSlider);
    addAndMakeVisible(xRangeSlider);
    addAndMakeVisible(xRateSlider);
    addAndMakeVisible(ySlider);
    addAndMakeVisible(yRangeSlider);
    addAndMakeVisible(yRateSlider);

    addAndMakeVisible(xRangeLabel);
    addAndMakeVisible(xRateLabel);
    addAndMakeVisible(yRangeLabel);
    addAndMakeVisible(yRateLabel);
}

void WSPointComponent::paint(juce::Graphics &g)
{
}

void WSPointComponent::resized()
{
    auto bounds = getLocalBounds();
    const auto sectionHeight    = bounds.getHeight() / 2;
    const auto sliderHeight     = sectionHeight / 2;
    const auto sliderWidth      = bounds.getWidth() / 2;
    const auto labelHeight      = sectionHeight / 4;

    // Top (x)
    auto topBounds = bounds.removeFromTop(sectionHeight);
    xSlider     .setBounds(topBounds    .removeFromTop(sliderHeight - labelHeight));

    auto xRangeBounds = topBounds       .removeFromLeft(sliderWidth);
    auto xRateBounds  = topBounds;

    xRangeLabel .setBounds(xRangeBounds .removeFromTop(labelHeight));
    xRangeSlider.setBounds(xRangeBounds);
    xRateLabel  .setBounds(xRateBounds  .removeFromTop(labelHeight));
    xRateSlider .setBounds(xRateBounds);

    // Bottom (y)
    auto bottomBounds = bounds;
    ySlider     .setBounds(bottomBounds .removeFromLeft(sliderWidth));
    
    auto yRangeBounds = bottomBounds    .removeFromTop(sliderHeight);
    auto yRateBounds  = bottomBounds;

    yRangeLabel .setBounds(yRangeBounds .removeFromTop(labelHeight));
    yRangeSlider.setBounds(yRangeBounds);
    yRateLabel  .setBounds(yRateBounds  .removeFromTop(labelHeight));
    yRateSlider .setBounds(yRateBounds);
}

mrta::ParameterSlider &WSPointComponent::getXSlider()
{
    return xSlider;
}

mrta::ParameterSlider &WSPointComponent::getYSlider()
{
    return ySlider;
}
