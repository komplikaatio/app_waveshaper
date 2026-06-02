#include "WSLaf.h"
#include <cassert>

WSLaf::WSLaf()
{
    // Make all slider text boxes invisible (no outline, no background)
    // The value text itself remains visible.
    setColour(juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxTextColourId,       UI_PALE);

    // Label text colour
    setColour(juce::Label::textColourId, UI_LIME);

    juce::String svgText (BinaryData::ws_bg_svg, BinaryData::ws_bg_svgSize);

    if (auto xml = juce::XmlDocument::parse(svgText))  
    {
        bgImg = juce::Drawable::createFromSVG(*xml);
    }
    else
    {
        bgImg = nullptr;  
    }            
}

void WSLaf::drawDocumentWindowBackground(
    juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    g.setColour(UI_DARK);
    g.fillRect(bounds.toFloat());

    if (bgImg != nullptr)
    {
        bgImg->drawWithin(g, bounds.toFloat(), juce::RectanglePlacement::stretchToFit, 1.0f);
    }
    else
    {
        g.setColour(UI_DARK);
        g.fillRect(bounds.toFloat());
    }
}

void WSLaf::drawDisplay(
    juce::Graphics& g,
    juce::Rectangle<int> bounds,
    std::vector<float> xPos,
    std::vector<float> yPos,
    float yLeft,
    float yRight)
{
    assert(xPos.size() == yPos.size());

    g.setColour(PAD_NAVY);
    g.fillRect(bounds);

    const auto numPoints     = xPos.size();
    const auto pointRadius   = 5.f;
    const auto height        = bounds.getHeight();
    const auto width         = bounds.getWidth();
    const auto pointWidth    = width / numPoints;
    const auto thickness     = 2.f;
    const auto gridThickness = 0.5f;

    // Grid lines
    juce::Path gridLines;
    for (auto pointIdx = 0; pointIdx <= numPoints; ++pointIdx)
    {
        auto x = bounds.getX() + pointWidth * pointIdx;
        gridLines.startNewSubPath(x, bounds.getY());
        gridLines.lineTo(x, bounds.getY() + height);
    }

    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.5f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.5f);

    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.25f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.25f);

    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.75f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.75f);

    g.setColour(PAD_BLUE.withAlpha(0.5f));
    g.strokePath(gridLines, juce::PathStrokeType(gridThickness));

    // Catmull-Rom curve
    juce::Path path;
    path.startNewSubPath(bounds.getX(), bounds.getY() + yLeft);

    const int samplesPerSegment = 16;

    for (auto pointIdx = 0; pointIdx < numPoints - 1; ++pointIdx)
    {
        const float y0 = (pointIdx > 0) ? yPos[pointIdx - 1] : yLeft;
        const float y1 = yPos[pointIdx];
        const float y2 = yPos[pointIdx + 1];
        const float y3 = (pointIdx < numPoints - 2) ? yPos[pointIdx + 2] : yRight;

        const float x1 = xPos[pointIdx];
        const float x2 = xPos[pointIdx + 1];

        for (auto s = 0; s <= samplesPerSegment; ++s)
        {
            const float t  = static_cast<float>(s) / samplesPerSegment;
            const float t2 = t * t;
            const float t3 = t2 * t;

            const float y = 0.5f * (
                (-t3 + 2.f*t2 -        t) * y0 +
                ( 3.f*t3 - 5.f*t2 + 2.f) * y1 +
                (-3.f*t3 + 4.f*t2 +    t) * y2 +
                (        t3 - t2        ) * y3
            );
            const float x = x1 + t * (x2 - x1);

            path.lineTo(bounds.getX() + x, bounds.getY() + y);
        }
    }

    path.lineTo(bounds.getX() + width, bounds.getY() + yRight);
    g.setColour(PAD_TAN);
    g.strokePath(path, juce::PathStrokeType(thickness));

    // Points
    g.setColour(PAD_ORANGE);
    for (auto pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
        auto x = xPos[pointIdx];
        auto y = yPos[pointIdx];

        g.drawEllipse(bounds.getX() + x - pointRadius * 0.5f,
                      bounds.getY() + y - pointRadius * 0.5f,
                      pointRadius, pointRadius, thickness);
    }
}


void WSLaf::drawTitle(
    juce::Graphics& g,
    juce::Rectangle<int> bounds)
{

}


// Rotary slider
void WSLaf::drawRotarySlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPosProportional,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider& slider)
{
    const float cx = x + width  * 0.5f;
    const float cy = y + height * 0.5f;
    const float radius = juce::jmin(width, height) * 0.5f - 4.f;

    const float angle = rotaryStartAngle
                      + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // Outer ring
    {
        const float ringW = 2.0f;
        juce::Path ring;
        ring.addCentredArc(cx, cy, radius, radius,
                           0.f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(UI_MID.withAlpha(0.4f));
        g.strokePath(ring, juce::PathStrokeType(ringW,
                                                juce::PathStrokeType::JointStyle::curved,
                                                juce::PathStrokeType::EndCapStyle::rounded));
    }

    // Value arc
    {
        const float arcW = 2.5f;
        juce::Path valueArc;
        valueArc.addCentredArc(cx, cy, radius, radius,
                               0.f, rotaryStartAngle, angle, true);
        g.setColour(PAD_ORANGE);
        g.strokePath(valueArc, juce::PathStrokeType(arcW,
                                                     juce::PathStrokeType::JointStyle::curved,
                                                     juce::PathStrokeType::EndCapStyle::rounded));
    }

    // Tick marks
    g.setColour(UI_MID.withAlpha(0.5f));
    for (int i = 0; i <= 4; ++i)
    {
        const float t     = (float)i / 4.f;
        const float a     = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
        const float cos_a = std::cos(a);
        const float sin_a = std::sin(a);

        const float innerR = radius + 5.f;
        const float outerR = radius + 9.f;

        g.drawLine(cx + cos_a * innerR, cy + sin_a * innerR,
                   cx + cos_a * outerR, cy + sin_a * outerR, 1.2f);
    }

    // Knob body
    const float bodyRadius = radius * 0.62f;
    g.setColour(PAD_BLUE.withAlpha(0.5f));
    g.fillEllipse(cx - bodyRadius, cy - bodyRadius,
                  bodyRadius * 2.f, bodyRadius * 2.f);

    g.setColour(UI_MID.withAlpha(0.25f));
    g.drawEllipse(cx - bodyRadius, cy - bodyRadius,
                  bodyRadius * 2.f, bodyRadius * 2.f, 1.f);

    // Pointer
    {
        const float cos_a  = std::cos(angle);
        const float sin_a  = std::sin(angle);
        const float pInner = bodyRadius * 0.18f;
        const float pOuter = bodyRadius * 0.82f;

        g.setColour(PAD_ORANGE.withAlpha(0.18f));
        g.drawLine(cx + cos_a * pInner, cy + sin_a * pInner,
                   cx + cos_a * pOuter, cy + sin_a * pOuter, 5.f);

        g.setColour(PAD_ORANGE);
        g.drawLine(cx + cos_a * pInner, cy + sin_a * pInner,
                   cx + cos_a * pOuter, cy + sin_a * pOuter, 2.f);

        g.fillEllipse(cx + cos_a * pOuter - 2.5f,
                      cy + sin_a * pOuter - 2.5f,
                      5.f, 5.f);
    }

    // Centre dot
    g.setColour(UI_DARK.withAlpha(0.6f));
    g.fillEllipse(cx - 2.f, cy - 2.f, 4.f, 4.f);
}


// Linear slider
void WSLaf::drawLinearSlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    const bool isVertical = (style == juce::Slider::LinearVertical);

    // Track
    constexpr float trackThickness = 2.f;
    float trackX1, trackY1, trackX2, trackY2;

    if (isVertical)
    {
        const float cx = x + width * 0.5f;
        trackX1 = cx;  trackY1 = (float)(y + 4);
        trackX2 = cx;  trackY2 = (float)(y + height - 4);
    }
    else
    {
        const float cy = y + height * 0.5f;
        trackX1 = (float)(x + 4);         trackY1 = cy;
        trackX2 = (float)(x + width - 4); trackY2 = cy;
    }

    g.setColour(UI_MID.withAlpha(0.35f));
    g.drawLine(trackX1, trackY1, trackX2, trackY2, trackThickness);

    // Tick marks
    const float tickHalf      = 4.f;
    const float tickThickness = 1.0f;
    g.setColour(UI_MID.withAlpha(0.45f));

    for (int i = 0; i <= 4; ++i)
    {
        const float t = (float)i / 4.f;
        if (isVertical)
        {
            const float ty = trackY1 + t * (trackY2 - trackY1);
            const float cx = x + width * 0.5f;
            g.drawLine(cx - tickHalf, ty, cx + tickHalf, ty, tickThickness);
        }
        else
        {
            const float tx = trackX1 + t * (trackX2 - trackX1);
            const float cy = y + height * 0.5f;
            g.drawLine(tx, cy - tickHalf, tx, cy + tickHalf, tickThickness);
        }
    }

    // Filled value portion
    g.setColour(PAD_ORANGE.withAlpha(0.85f));
    if (isVertical)
        g.drawLine(trackX1, trackY2, trackX2, sliderPos, trackThickness + 1.f);
    else
        g.drawLine(trackX1, trackY1, sliderPos, trackY2, trackThickness + 1.f);

    // Thumb
    constexpr float thumbW = 10.f;
    constexpr float thumbH =  3.f;

    float thumbX, thumbY, thumbRectW, thumbRectH;
    if (isVertical)
    {
        thumbX     = x + width * 0.5f - thumbW * 0.5f;
        thumbY     = sliderPos        - thumbH * 0.5f;
        thumbRectW = thumbW;
        thumbRectH = thumbH;
    }
    else
    {
        thumbX     = sliderPos        - thumbH * 0.5f;
        thumbY     = y + height * 0.5f - thumbW * 0.5f;
        thumbRectW = thumbH;
        thumbRectH = thumbW;
    }

    const juce::Rectangle<float> thumbRect(thumbX, thumbY, thumbRectW, thumbRectH);

    // Halo
    g.setColour(PAD_ORANGE.withAlpha(0.15f));
    g.fillRect(thumbRect.expanded(4.f, 4.f));

    // Body
    g.setColour(PAD_NAVY);
    g.fillRect(thumbRect.expanded(isVertical ? 3.f : 0.f,
                                  isVertical ? 0.f : 3.f));

    // Accent line
    g.setColour(PAD_ORANGE);
    g.fillRect(thumbRect);

    // End caps
    g.setColour(UI_DARK.withAlpha(0.55f));
    if (isVertical)
    {
        g.fillRect(thumbX,               thumbY, 2.f, thumbH);
        g.fillRect(thumbX + thumbW - 2.f, thumbY, 2.f, thumbH);
    }
    else
    {
        g.fillRect(thumbX, thumbY,               thumbRectW, 2.f);
        g.fillRect(thumbX, thumbY + thumbW - 2.f, thumbRectW, 2.f);
    }
}


// Buttons
void WSLaf::drawButtonBackground(
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const bool isOn   = button.getToggleState();

    // Body fill
    g.setColour(isOn ? UI_MID.brighter(0.15f) : UI_MID.darker(0.3f));
    g.fillRect(bounds);

    // Hover / press
    if (shouldDrawButtonAsDown)
    {
        g.setColour(PAD_ORANGE.withAlpha(0.15f));
        g.fillRect(bounds);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(UI_MID.withAlpha(0.12f));
        g.fillRect(bounds);
    }

    // Border — accent when ON, dim when OFF
    g.setColour(isOn ? PAD_ORANGE : UI_MID.withAlpha(0.9f));
    g.drawRect(bounds, 1.f);

    // Power indicator dot in the centre
    const float dotR = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.14f;
    const float cx   = bounds.getCentreX();
    const float cy   = bounds.getCentreY();

    if (isOn)
    {
        // Glow
        g.setColour(PAD_ORANGE.withAlpha(0.25f));
        g.fillEllipse(cx - dotR * 1.8f, cy - dotR * 1.8f, dotR * 3.6f, dotR * 3.6f);
        // Dot
        g.setColour(PAD_ORANGE);
        g.fillEllipse(cx - dotR, cy - dotR, dotR * 2.f, dotR * 2.f);
    }
    else
    {
        g.setColour(UI_MID.withAlpha(0.5f));
        g.drawEllipse(cx - dotR, cy - dotR, dotR * 2.f, dotR * 2.f, 1.f);
    }
}

void WSLaf::drawButtonText(
    juce::Graphics& g,
    juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto  bounds = button.getLocalBounds().toFloat();
    const bool  isOn   = button.getToggleState();

    const juce::Colour textCol = isOn ? UI_CREAM : UI_MID.withAlpha(0.9f);

    g.setColour(textCol);
    g.setFont(juce::Font(juce::FontOptions(12.f).withStyle("Regular")));
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
}