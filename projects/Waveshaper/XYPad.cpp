#pragma once

#include "XYPad.h"

XYPad::XYPad(juce::Slider& sliderX, juce::Slider& sliderY)
    : xSlider(sliderX), ySlider(sliderY)
{
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
}


void XYPad::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Map slider values to pixel position
    float px = juce::jmap((float)xSlider.getValue(),
                            (float)xSlider.getMinimum(),
                            (float)xSlider.getMaximum(),
                            bounds.getX(), bounds.getRight());

    float py = juce::jmap((float)ySlider.getValue(),
                            (float)ySlider.getMinimum(),
                            (float)ySlider.getMaximum(),
                            bounds.getBottom(), bounds.getY()); // Y is flipped

    // Draw the point
    constexpr float radius = 5.0f;
    g.setColour(juce::Colours::white);
    g.fillEllipse(px - radius, py - radius, radius * 2.0f, radius * 2.0f);
}

void XYPad::mouseDown(const juce::MouseEvent& e)
{
    updateFromMouse(e.position);
}


void XYPad::mouseDrag(const juce::MouseEvent& e)
{
    updateFromMouse(e.position);
}

void XYPad::updateFromMouse(juce::Point<float> pos)
{
    auto bounds = getLocalBounds().toFloat();

    // Clamp position within bounds
    float clampedX = juce::jlimit(bounds.getX(),    bounds.getRight(),  pos.x);
    float clampedY = juce::jlimit(bounds.getY(),    bounds.getBottom(), pos.y);

    // Map pixel position to slider range
    double newX = juce::jmap(clampedX,
                                bounds.getX(), bounds.getRight(),
                                (float)xSlider.getMinimum(),
                                (float)xSlider.getMaximum());

    double newY = juce::jmap(clampedY,
                                bounds.getBottom(), bounds.getY(), // Y is flipped
                                (float)ySlider.getMinimum(),
                                (float)ySlider.getMaximum());

    xSlider.setValue(newX, juce::sendNotificationSync);
    ySlider.setValue(newY, juce::sendNotificationSync);

    repaint();
}