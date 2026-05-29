#include "Waveshaper.h"
#include <cassert>

namespace DSP
{

Waveshaper::Waveshaper()
{
    // Set fixed end Y-only points
    points[0].x = X_MIN;
    points[0].y = Y_MIN;
    points[WS_POINTS + 1].x = X_MAX;
    points[WS_POINTS + 1].y = Y_MAX;
}

void Waveshaper::prepare(double newSampleRate)
{
    for (auto& point : points)
    {
        point.xRamp         .prepare(newSampleRate);
        point.yRamp         .prepare(newSampleRate);
        point.xRandomRamp   .prepare(newSampleRate);
        point.yRandomRamp   .prepare(newSampleRate);
    }
}

void Waveshaper::process(juce::AudioBuffer<float> &buffer)
{
    for (auto sampleIdx = 0; sampleIdx < buffer.getNumSamples(); ++sampleIdx)
    {
        for (auto chIdx = 0; chIdx < buffer.getNumChannels(); ++chIdx)
        {
            // Get input sample
            auto inputSample = buffer.getSample(chIdx, sampleIdx);

            // Process
            auto outputSample = applyWaveshaping(inputSample);

            // Replace
            buffer.setSample(chIdx, sampleIdx, outputSample);
        }
        step();
    }
    updateCurrentPositions();
}


void Waveshaper::setX(size_t pointIdx, float newValue)
{
    assert(pointIdx < WS_POINTS);

    // Expect normalized value
    assert(newValue <= 1.0f);
    assert(newValue >= 0.f);

    // Scale to point´s range
    newValue = SizeX * pointIdx + newValue * SizeX;

    // Here we are in range [0.0, 2.0]

    // Scale to [-1.0, 1.0]
    newValue -= 1.f;

    getPoint(pointIdx).xRamp.setTarget(newValue);
}

void Waveshaper::setY(size_t pointIdx, float newValue)
{
    assert(pointIdx < WS_POINTS);

    // Expect value in range [-1.0, 1.0]
    assert(newValue <= Y_MAX);
    assert(newValue >= Y_MIN);

    getPoint(pointIdx).yRamp.setTarget(newValue);
}

void Waveshaper::setXRandomRange(size_t pointIdx, float newValue)
{
    assert(pointIdx < WS_POINTS);

    // Expect normalized value
    assert(newValue <= 1.0f);
    assert(newValue >= 0.f);

    // Scale to point´s range size
    newValue = newValue * SizeX * 0.5f;

    auto min = -newValue;
    auto max = newValue;

    getPoint(pointIdx).xRandomRamp.setRange(min, max); 
}

void Waveshaper::setYRandomRange(size_t pointIdx, float newValue)
{
    assert(pointIdx < WS_POINTS);

    // Expect normalized value
    assert(newValue <= 1.0f);
    assert(newValue >= 0.f);

    auto min = -newValue;
    auto max = newValue;

    getPoint(pointIdx).yRandomRamp.setRange(min, max);
}

void Waveshaper::setXRandomRate(size_t pointIdx, float newValueMs)
{
    assert(pointIdx < WS_POINTS);
    assert(newValueMs >= 0);

    getPoint(pointIdx).xRandomRamp.setRampTime(newValueMs * 0.001f);
}

void Waveshaper::setYRandomRate(size_t pointIdx, float newValueMs)
{
    assert(pointIdx < WS_POINTS);
    assert(newValueMs >= 0);

    getPoint(pointIdx).yRandomRamp.setRampTime(newValueMs * 0.001f);
}

void Waveshaper::setLeftY(float newValue)
{
    assert(newValue >= Y_MIN);
    assert(newValue <= Y_MAX);

    points[0].yRamp.setTarget(newValue);
}

void Waveshaper::setRightY(float newValue)
{
    assert(newValue >= Y_MIN);
    assert(newValue <= Y_MAX);

    points[WS_POINTS + 1].yRamp.setTarget(newValue);
}

float Waveshaper::getCurrentX(size_t pointIdx)
{
    assert(pointIdx < WS_POINTS);
    return currentXs[pointIdx].load();
}

float Waveshaper::getCurrentY(size_t pointIdx)
{
    assert(pointIdx < WS_POINTS);
    return currentYs[pointIdx].load();
}

float Waveshaper::applyWaveshaping(float inputSample)
{
    // Find within which pair of points falls inputSample (first shift to range [0, 2])
    auto regionIdx = static_cast<size_t>((inputSample + 1.f) * SizeXInv);

    // Corner case inputSample = 1.0
    regionIdx -= regionIdx == WS_POINTS;

    // Array padding keeps us safe
    auto* pointA = &getPoint(regionIdx);
    auto* pointB = &getPoint(regionIdx + 1);
    if(inputSample < pointA->x)
    {
        pointA = &getPoint(regionIdx - 1);
        pointB = &getPoint(regionIdx);
    }

    // Interpolate and apply waveshaping function
    /*
    const auto prop = (inputSample - pointA->x) / (pointB->x - pointA->x);
    const auto outputSample = (1.f - prop) * pointA->y + prop * pointB->y;
    */
    // Catmull-Rom: pull in the two outer neighbours (padding ensures validity)
    const float y0 = (pointA - 1)->y;
    const float y1 = pointA->y;
    const float y2 = pointB->y;
    const float y3 = (pointB + 1)->y;

    // Evaluate cubic Hermite basis
    const float t  = (inputSample - pointA->x) / (pointB->x - pointA->x);
    const float t2 = t  * t;
    const float t3 = t2 * t;

    const float outputSample = 0.5f * (
        (-t3 + 2.f*t2 -        t) * y0 +
        ( 3.f*t3 - 5.f*t2 + 2.f) * y1 +
        (-3.f*t3 + 4.f*t2 +    t) * y2 +
        (       t3 - t2          ) * y3
    );

    return outputSample;
}

void Waveshaper::step()
{
    // Step ramps and calculates new point positions
    for (auto pointIdx = 0; pointIdx < WS_POINTS; ++pointIdx)
    {
        auto& point = getPoint(pointIdx);
        
        // Get X target and random offset
        auto xTarget = point.xRamp.getNext();
        auto xRandom = point.xRandomRamp.getNext();
        
        // Calculate allowed X range for this point
        float pointMinX = X_MIN + pointIdx * SizeX;
        float pointMaxX = pointMinX + SizeX;
        
        // Tentative X position
        float finalX = xTarget + xRandom;
        
        // Reflect X off boundaries (like a ball bouncing)
        if (finalX < pointMinX) {
            finalX = pointMinX + (pointMinX - finalX);
            // If reflection still out of bounds, wrap around
            if (finalX > pointMaxX) {
                finalX = pointMaxX - (finalX - pointMaxX);
            }
        } else if (finalX > pointMaxX) {
            finalX = pointMaxX - (finalX - pointMaxX);
            if (finalX < pointMinX) {
                finalX = pointMinX + (pointMinX - finalX);
            }
        }
        
        // Get Y target and random offset
        auto yTarget = point.yRamp.getNext();
        auto yRandom = point.yRandomRamp.getNext();
        
        // Tentative Y position
        float finalY = yTarget + yRandom;
        
        // Reflect Y off boundaries (all points share the same Y range)
        if (finalY < Y_MIN) {
            finalY = Y_MIN + (Y_MIN - finalY);
            // If reflection still out of bounds, wrap around
            if (finalY > Y_MAX) {
                finalY = Y_MAX - (finalY - Y_MAX);
            }
        } else if (finalY > Y_MAX) {
            finalY = Y_MAX - (finalY - Y_MAX);
            if (finalY < Y_MIN) {
                finalY = Y_MIN + (Y_MIN - finalY);
            }
        }
        
        // Apply clamped positions
        point.x = finalX;
        point.y = finalY;
    }
    
    // Set ends (Y-only points)
    points[0]           .y = points[0]           .yRamp.getNext();
    points[WS_POINTS+1] .y = points[WS_POINTS+1] .yRamp.getNext();
}

void Waveshaper::updateCurrentPositions()
{
    for(auto pointIdx = 0; pointIdx < WS_POINTS; ++pointIdx)
    {
        auto& point = getPoint(pointIdx);
        currentXs[pointIdx].store(point.x);
        currentYs[pointIdx].store(point.y);
    }
}

Waveshaper::Point &Waveshaper::getPoint(size_t pointIdx)
{
    // Take padding into account
    return points[pointIdx + 1];
}
}