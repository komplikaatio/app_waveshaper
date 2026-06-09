#include "WSPointComponent.h"
#include "WaveshaperProcessor.h"

WSPointComponent::WSPointComponent(
    size_t pointIdx,
    juce::AudioProcessorValueTreeState& apvts,
    DSP::Waveshaper& waveshaper)
    :
    pointIdx        { pointIdx },
    waveshaper      { waveshaper },

    // Initialize sliders
    xSlider         { Param::ID::pointX     (pointIdx), apvts },
    xRangeSlider    { Param::ID::pointXRange(pointIdx), apvts },
    xRateSlider     { Param::ID::pointXRate (pointIdx), apvts },
    ySlider         { Param::ID::pointY     (pointIdx), apvts },
    yRangeSlider    { Param::ID::pointYRange(pointIdx), apvts },
    yRateSlider     { Param::ID::pointYRate (pointIdx), apvts }
{
    // Slider styles
    ySlider         .setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    yRangeSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    yRateSlider     .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    xSlider         .setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    xRangeSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    xRateSlider     .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    // Add style
    auto styleTextBox = [](juce::Slider& s)
    {
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 12);
        s.setColour(juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
        s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        s.setNumDecimalPlacesToDisplay(2);
    };
    styleTextBox(xSlider);
    styleTextBox(ySlider);
    styleTextBox(xRangeSlider);
    styleTextBox(xRateSlider);
    styleTextBox(yRangeSlider);
    styleTextBox(yRateSlider);

    xRangeSlider    .setMouseDragSensitivity(200);
    xRateSlider     .setMouseDragSensitivity(200);
    yRangeSlider    .setMouseDragSensitivity(200);
    yRateSlider     .setMouseDragSensitivity(200);

    // Label styling
    auto setupLabel = [](juce::Label& label, float fontSize)
    {
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(juce::FontOptions(fontSize)));
    };

    setupLabel(RangeLabel, 10.f);
    setupLabel(RateLabel,  10.f);

    // Add everything
    addAndMakeVisible(ySlider);
    addAndMakeVisible(yRangeSlider);
    addAndMakeVisible(yRateSlider);

    addAndMakeVisible(RangeLabel);
    addAndMakeVisible(RateLabel);

    addAndMakeVisible(xSlider);
    addAndMakeVisible(xRangeSlider);
    addAndMakeVisible(xRateSlider);
}

void WSPointComponent::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    g.setColour(juce::Colour(0xff3a3b90));
    g.drawRect(bounds, 1.f);

    const float midY = bounds.getCentreY();
    g.setColour(juce::Colour(0xff3a3b90).withAlpha(0.8f));
    const float inset = 4.f;
    const float gap = 6.f;
    g.drawLine(bounds.getX() + inset, midY - gap, bounds.getRight() - inset, midY - gap, 1.5f);
    g.drawLine(bounds.getX() + inset, midY + gap, bounds.getRight() - inset, midY + gap, 1.5f);
}

void WSPointComponent::resized()
{
    const auto full   = getLocalBounds();
    const int  midY   = full.getY() + full.getHeight() / 2;
    const int  gap    = 5; 

    juce::Rectangle<int> sepStrip (full.getX(), midY - gap, full.getWidth(), gap * 2);
    auto labelLeft  = sepStrip.removeFromLeft(sepStrip.getWidth() / 2);
    auto labelRight = sepStrip;
    RangeLabel.setBounds(labelLeft);
    RateLabel .setBounds(labelRight);

    // Working area above the separator (Y controls)
    auto yBounds = full.withBottom(midY - gap).reduced(4, 4);
    yBounds.removeFromBottom(2);

    const int sliderH = 27;
    const int knobSize = 40;

    ySlider.setBounds(yBounds.removeFromTop(sliderH));

    auto yKnobRow  = yBounds;
    auto yLeft  = yKnobRow.removeFromLeft(yKnobRow.getWidth() / 2);
    auto yRight = yKnobRow;
    yRangeSlider.setBounds(yLeft .withSizeKeepingCentre(knobSize, knobSize));
    yRateSlider .setBounds(yRight.withSizeKeepingCentre(knobSize, knobSize));

    // Working area below the separator (X controls)
    auto xBounds = full.withTop(midY + gap).reduced(4, 4);
    xBounds.removeFromTop(2); 

    xSlider.setBounds(xBounds.removeFromBottom(sliderH));
    auto xKnobRow  = xBounds;
    auto xLeft  = xKnobRow.removeFromLeft(xKnobRow.getWidth() / 2);
    auto xRight = xKnobRow;
    xRangeSlider.setBounds(xLeft .withSizeKeepingCentre(knobSize, knobSize));
    xRateSlider .setBounds(xRight.withSizeKeepingCentre(knobSize, knobSize));
}

mrta::ParameterSlider& WSPointComponent::getXSlider()
{
    return xSlider;
}

mrta::ParameterSlider& WSPointComponent::getYSlider()
{
    return ySlider;
}