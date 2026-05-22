#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class WSLaf : public juce::LookAndFeel_V4
{
public:
    virtual void drawDocumentWindowBackground(
        juce::Graphics& g,
        juce::Rectangle<int> bounds
    );
    virtual void drawDisplay(
        juce::Rectangle<int> bounds,
        std::vector<double> x,
        std::vector<double> y
    );

    // Sliders
    virtual void WSLaf::drawRotarySlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider
    ) override;
    
    virtual void WSLaf::drawLinearSlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float /*minSliderPos*/,
        float /*maxSliderPos*/,
        juce::Slider::SliderStyle style,
        juce::Slider& slider
    ) override;

    // Buttons
    virtual void WSLaf::drawToggleButton(
        juce::Graphics& g,
        juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

private:

    static const juce::Colour COLOUR_0 { 0xfff1faee };
    static const juce::Colour COLOUR_1 { 0xffa8dadc };
    static const juce::Colour COLOUR_2 { 0xff457b9d };
    static const juce::Colour COLOUR_3 { 0xff1d3557 };
    static const juce::Colour COLOUR_4 { 0xffe63946 };
};