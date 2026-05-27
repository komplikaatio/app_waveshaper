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
        juce::Graphics& g,
        juce::Rectangle<int> bounds,
        std::vector<float> x,
        std::vector<float> y,
        float yLeft,
        float yRight
    );

    virtual void drawTitle(
        juce::Graphics& g,
        juce::Rectangle<int> bounds
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
        float minSliderPos,
        float maxSliderPos,
        juce::Slider::SliderStyle style,
        juce::Slider& slider
    ) override;

    // Buttons
    void WSLaf::drawButtonBackground(
        juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;
    
    // Not in use since EnableButton doesn´t have text
    void drawButtonText(
        juce::Graphics& g,
        juce::TextButton& button,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

private:

    inline static const juce::Colour COLOUR_0 { 0xff08070c }; // #08070c
    inline static const juce::Colour COLOUR_1 { 0xff1f1a2c }; // #1f1a2c
    inline static const juce::Colour COLOUR_2 { 0xff3e3850 }; // #3e3850
    inline static const juce::Colour COLOUR_3 { 0xff4f2eaa }; // #4f2eaa
    inline static const juce::Colour COLOUR_4 { 0xff5924f0 }; // #5924f0
    inline static const juce::Colour COLOUR_5 { 0xff9e7ef9 }; // #9e7ef9
    inline static const juce::Colour COLOUR_6 { 0xffecf97e }; // #ecf97e
    inline static const juce::Colour COLOUR_7 { 0xff747a3c }; // #747a3c
};