#pragma once

#include "XYPad.h"

XYPad::XYPad(juce::Slider& sliderX, juce::Slider& sliderY)
    : xSlider(sliderX), ySlider(sliderY)
{
    setMouseCursor(juce::MouseCursor::CrosshairCursor);
}


void XYPad::paint(juce::Graphics& g)
{
    if (hover)
    {
        g.setColour(juce::Colour(0x05ffffff));
        g.fillRect(getLocalBounds());
    }
}

void XYPad::mouseDown(const juce::MouseEvent& e)
{
    updateFromMouse(e.position);
}


void XYPad::mouseDrag(const juce::MouseEvent& e)
{
    updateFromMouse(e.position);
}

void XYPad::mouseEnter(const juce::MouseEvent &event)
{
    hover = true;
    repaint();
}

void XYPad::mouseExit(const juce::MouseEvent &event)
{
    hover = false;
    repaint();
}

void XYPad::mouseDoubleClick(const juce::MouseEvent &event)
{
    // Just transfer event to both sliders
    xSlider.mouseDoubleClick(event);
    ySlider.mouseDoubleClick(event);
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
}