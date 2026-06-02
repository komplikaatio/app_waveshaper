#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <BinaryData.h>

class WSLaf : public juce::LookAndFeel_V4
{
public:
    WSLaf();

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
    void drawRotarySlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider
    ) override;
    
    void drawLinearSlider(
        juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPos,
        float minSliderPos,
        float maxSliderPos,
        juce::Slider::SliderStyle style,
        juce::Slider& slider
    ) override;

    // Buttons
    void drawButtonBackground(
        juce::Graphics& g,
        juce::Button& button,
        const juce::Colour& backgroundColour,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;
    
    void drawButtonText(
        juce::Graphics& g,
        juce::TextButton& button,
        bool shouldDrawButtonAsHighlighted,
        bool shouldDrawButtonAsDown
    ) override;

    // --- Colour palette ---
    // UI general (backgrounds, frames, chrome)
    inline static const juce::Colour UI_DARK    { 0xff21227a };  // #21227a  deep indigo bg
    inline static const juce::Colour UI_MID     { 0xff3a3b90 };  // #3a3b90  mid indigo
    inline static const juce::Colour UI_OLIVE   { 0xffbebd42 };  // #bebd42  olive-yellow accent
    inline static const juce::Colour UI_LIME    { 0xffd0cf75 };  // #d0cf75  lime text
    inline static const juce::Colour UI_PALE    { 0xffe1e0a8 };  // #e1e0a8  pale yellow
    inline static const juce::Colour UI_CREAM   { 0xfffdfde4 };  // #fdfde4  near-white

    // Pad / display / interactive elements
    inline static const juce::Colour PAD_ORANGE { 0xffff6b35 };  // #ff6b35  primary accent
    inline static const juce::Colour PAD_PEACH  { 0xfffb986a };  // #fb986a  secondary accent
    inline static const juce::Colour PAD_TAN    { 0xfff7c59f };  // #f7c59f  muted warm
    inline static const juce::Colour PAD_CREAM  { 0xfffdfddd };  // #fdfddd  light warm bg
    inline static const juce::Colour PAD_NAVY   { 0xff004e89 };  // #004e89  deep blue
    inline static const juce::Colour PAD_BLUE   { 0xff1a659e };  // #1a659e  mid blue

private:
    std::unique_ptr<juce::Drawable> bgImg;
};