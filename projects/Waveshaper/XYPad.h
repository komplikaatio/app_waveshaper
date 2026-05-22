#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class XYPad : public juce::Component
{
public:
    XYPad(juce::Slider& sliderX, juce::Slider& sliderY);

    ~XYPad() override = default;

    void paint(juce::Graphics& g) override;

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    void updateFromMouse(juce::Point<float> pos);

    juce::Slider& xSlider;
    juce::Slider& ySlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XYPad)
};