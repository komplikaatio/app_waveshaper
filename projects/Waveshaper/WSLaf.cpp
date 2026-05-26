#include "WSLaf.h"

void WSLaf::drawDocumentWindowBackground(
    juce::Graphics& g,
    juce::Rectangle<int> bounds
)
{
    /*
    // Create a linear gradient between the first two colors
    juce::ColourGradient gradient (COLOUR_0, 0.0f, 0.0f, 
                                   COLOUR_1, (float)bounds.getWidth(), 0.0f, false);

    // Add the third color at the 50% mark (0.5 proportion)
    gradient.addColour (0.5, COLOUR_0);

    // Apply the gradient and fill the area
    g.setGradientFill (gradient);
    g.fillRect (bounds.toFloat());
    */
   g.fillAll(COLOUR_0);
}

void WSLaf::drawDisplay(
    juce::Graphics& g,
    juce::Rectangle<int> bounds,
    std::vector<double> x,
    std::vector<double> y
)
{
    const float bX = (float)bounds.getX();
    const float bY = (float)bounds.getY();
    const float bW = (float)bounds.getWidth();
    const float bH = (float)bounds.getHeight();

    // Map a value in [-1, 1] to a pixel X coordinate
    auto toPixelX = [&](double v) {
        return bX + (float)juce::jmap(v, -1.0, 1.0, 0.0, (double)bW);
    };
    // Map a value in [-1, 1] to a pixel Y coordinate (flipped: +1 = top)
    auto toPixelY = [&](double v) {
        return bY + (float)juce::jmap(v, -1.0, 1.0, (double)bH, 0.0);
    };

    // -------------------------------------------------------------------------
    // Background
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_3);
    g.fillRect(bounds);

    // -------------------------------------------------------------------------
    // Grid lines
    // -------------------------------------------------------------------------
    const std::vector<double> majorLines  = { -1.0, -0.5, 0.0, 0.5, 1.0 };
    const std::vector<double> minorLines  = { -0.75, -0.25, 0.25, 0.75 };
    const float               labelHeight = 12.f;
    const juce::Font          labelFont   (juce::FontOptions(labelHeight));

    // --- Minor grid lines (subtle, no labels) --------------------------------
    g.setColour(COLOUR_2.withAlpha(0.35f));
    for (double v : minorLines)
    {
        // vertical
        float px = toPixelX(v);
        g.drawLine(px, bY, px, bY + bH, 0.5f);

        // horizontal
        float py = toPixelY(v);
        g.drawLine(bX, py, bX + bW, py, 0.5f);
    }

    // --- Major grid lines + labels -------------------------------------------
    for (double v : majorLines)
    {
        const bool  isCenter = (v == 0.0);
        const float thickness = isCenter ? 1.5f : 0.75f;
        g.setColour(COLOUR_2.withAlpha(isCenter ? 0.9f : 0.55f));

        // Vertical line
        float px = toPixelX(v);
        g.drawLine(px, bY, px, bY + bH, thickness);

        // Horizontal line
        float py = toPixelY(v);
        g.drawLine(bX, py, bX + bW, py, thickness);

        // --- Labels ----------------------------------------------------------
        g.setColour(COLOUR_2.withAlpha(0.75f));
        g.setFont(labelFont);

        juce::String label = (v == 0.0)  ? "0"
                           : (v ==  1.0) ? "+1"
                           : (v == -1.0) ? "-1"
                           : juce::String(v, 2);  // e.g. "-0.50"

        const float labelW = labelFont.getStringWidthFloat(label) + 4.f;

        // X-axis label — below the bottom edge, centred on the vertical line
        g.drawText(label,
                   juce::Rectangle<float>(px - labelW * 0.5f,
                                          bY + bH + 2.f,
                                          labelW, labelHeight),
                   juce::Justification::centred, false);

        // Y-axis label — left of the left edge, centred on the horizontal line
        // (skip 0 to avoid doubling up with the x-axis label at the origin)
        if (v != 0.0)
            g.drawText(label,
                       juce::Rectangle<float>(bX - labelW - 4.f,
                                              py - labelHeight * 0.5f,
                                              labelW, labelHeight),
                       juce::Justification::centredRight, false);
    }

    // -------------------------------------------------------------------------
    // Border around the display
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_2);
    g.drawRect(bounds, 1);

    // -------------------------------------------------------------------------
    // Line joining points
    // -------------------------------------------------------------------------
    juce::Path path;
    path.startNewSubPath((float)x[0], (float)y[0]);
    for (auto pointIdx = 1; pointIdx < x.size(); ++pointIdx)
        path.lineTo((float)x[pointIdx], (float)y[pointIdx]);

    g.setColour(COLOUR_4);
    g.strokePath(path, juce::PathStrokeType(3.f));
}


// -------------------------------------------------------------------------
// SLIDERS
// -------------------------------------------------------------------------
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

    // -------------------------------------------------------------------------
    // Outer ring — thin, sharp, full arc
    // -------------------------------------------------------------------------
    {
        const float ringW = 2.0f;
        juce::Path ring;
        ring.addCentredArc(cx, cy, radius, radius,
                           0.f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(COLOUR_2.withAlpha(0.4f));
        g.strokePath(ring, juce::PathStrokeType(ringW,
                                                juce::PathStrokeType::butt,
                                                juce::PathStrokeType::rounded));
    }

    // -------------------------------------------------------------------------
    // Value arc — fills from start to current angle in accent colour
    // -------------------------------------------------------------------------
    {
        const float arcW = 2.5f;
        juce::Path valueArc;
        valueArc.addCentredArc(cx, cy, radius, radius,
                               0.f, rotaryStartAngle, angle, true);
        g.setColour(COLOUR_4);
        g.strokePath(valueArc, juce::PathStrokeType(arcW,
                                                     juce::PathStrokeType::butt,
                                                     juce::PathStrokeType::rounded));
    }

    // -------------------------------------------------------------------------
    // Tick marks at 0%, 25%, 50%, 75%, 100%
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_2.withAlpha(0.5f));
    for (int i = 0; i <= 4; ++i)
    {
        const float t   = (float)i / 4.f;
        const float a   = rotaryStartAngle + t * (rotaryEndAngle - rotaryStartAngle);
        const float cos_a = std::cos(a);
        const float sin_a = std::sin(a);

        const float innerR = radius + 5.f;
        const float outerR = radius + 9.f;

        g.drawLine(cx + cos_a * innerR, cy + sin_a * innerR,
                   cx + cos_a * outerR, cy + sin_a * outerR, 1.2f);
    }

    // -------------------------------------------------------------------------
    // Inner filled circle — the "body"
    // -------------------------------------------------------------------------
    const float bodyRadius = radius * 0.62f;
    g.setColour(COLOUR_3);
    g.fillEllipse(cx - bodyRadius, cy - bodyRadius,
                  bodyRadius * 2.f, bodyRadius * 2.f);

    // Subtle inner border
    g.setColour(COLOUR_2.withAlpha(0.25f));
    g.drawEllipse(cx - bodyRadius, cy - bodyRadius,
                  bodyRadius * 2.f, bodyRadius * 2.f, 1.f);

    // -------------------------------------------------------------------------
    // Pointer line — from centre toward the arc, clipped inside the body
    // -------------------------------------------------------------------------
    {
        const float cos_a    = std::cos(angle);
        const float sin_a    = std::sin(angle);
        const float pInner   = bodyRadius * 0.18f;   // starts near centre
        const float pOuter   = bodyRadius * 0.82f;   // ends inside body rim

        // Shadow/halo behind pointer
        g.setColour(COLOUR_4.withAlpha(0.18f));
        g.drawLine(cx + cos_a * pInner, cy + sin_a * pInner,
                   cx + cos_a * pOuter, cy + sin_a * pOuter, 5.f);

        // Main pointer
        g.setColour(COLOUR_4);
        g.drawLine(cx + cos_a * pInner, cy + sin_a * pInner,
                   cx + cos_a * pOuter, cy + sin_a * pOuter, 2.f);

        // Tip dot
        g.fillEllipse(cx + cos_a * pOuter - 2.5f,
                      cy + sin_a * pOuter - 2.5f,
                      5.f, 5.f);
    }

    // -------------------------------------------------------------------------
    // Centre crosshair dot
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_1.withAlpha(0.6f));
    g.fillEllipse(cx - 2.f, cy - 2.f, 4.f, 4.f);
}


void WSLaf::drawLinearSlider(
    juce::Graphics& g,
    int x, int y, int width, int height,
    float sliderPos,
    float /*minSliderPos*/,
    float /*maxSliderPos*/,
    juce::Slider::SliderStyle style,
    juce::Slider& slider)
{
    const bool isVertical = (style == juce::Slider::LinearVertical);

    // -------------------------------------------------------------------------
    // Track geometry
    // -------------------------------------------------------------------------
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
        trackX1 = (float)(x + 4);       trackY1 = cy;
        trackX2 = (float)(x + width - 4); trackY2 = cy;
    }

    // Full track
    g.setColour(COLOUR_2.withAlpha(0.35f));
    g.drawLine(trackX1, trackY1, trackX2, trackY2, trackThickness);

    // -------------------------------------------------------------------------
    // Tick marks at 0%, 25%, 50%, 75%, 100% along the track
    // -------------------------------------------------------------------------
    const float tickHalf     = 4.f;
    const float tickThickness = 1.0f;
    g.setColour(COLOUR_2.withAlpha(0.45f));

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

    // -------------------------------------------------------------------------
    // Filled value portion (from track start to thumb)
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_4.withAlpha(0.85f));
    if (isVertical)
        // sliderPos is pixel position; for vertical, start = bottom of track
        g.drawLine(trackX1, trackY2, trackX2, sliderPos, trackThickness + 1.f);
    else
        g.drawLine(trackX1, trackY1, sliderPos, trackY2, trackThickness + 1.f);

    // -------------------------------------------------------------------------
    // Thumb — a flat rectangular notch (no round pill)
    // -------------------------------------------------------------------------
    constexpr float thumbW = 14.f;  // across the track
    constexpr float thumbH =  3.f;  // along the track (very thin bar)

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

    // Halo behind thumb
    g.setColour(COLOUR_4.withAlpha(0.15f));
    g.fillRect(thumbRect.expanded(4.f, isVertical ? 4.f : 4.f));

    // Thumb body — filled, COLOUR_3, so it "cuts" across the track
    g.setColour(COLOUR_3);
    g.fillRect(thumbRect.expanded(isVertical ? 5.f : 0.f,
                                  isVertical ? 0.f : 5.f));

    // Thumb accent line
    g.setColour(COLOUR_4);
    g.fillRect(thumbRect);

    // Small end caps on the thumb
    g.setColour(COLOUR_1.withAlpha(0.55f));
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


// -------------------------------------------------------------------------
// BUTTONS
// -------------------------------------------------------------------------
void WSLaf::drawToggleButton(
    juce::Graphics& g,
    juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto bounds     = button.getLocalBounds().toFloat();
    const bool isOn       = button.getToggleState();
    const float h         = bounds.getHeight();

    // -------------------------------------------------------------------------
    // Indicator box geometry — left-aligned, square, vertically centred
    // -------------------------------------------------------------------------
    constexpr float boxSize = 14.f;
    const float boxX = 4.f;
    const float boxY = (h - boxSize) * 0.5f;
    const juce::Rectangle<float> box(boxX, boxY, boxSize, boxSize);

    // -------------------------------------------------------------------------
    // Hover / press state — subtle brightening of the box area
    // -------------------------------------------------------------------------
    if (shouldDrawButtonAsDown)
    {
        g.setColour(COLOUR_4.withAlpha(0.12f));
        g.fillRect(box.expanded(4.f));
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(COLOUR_2.withAlpha(0.10f));
        g.fillRect(box.expanded(4.f));
    }

    // -------------------------------------------------------------------------
    // Box fill — dark body so the accent cuts through cleanly
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_3);
    g.fillRect(box);

    // -------------------------------------------------------------------------
    // ON state: red fill + inner cross-bar (like a lit indicator)
    // -------------------------------------------------------------------------
    if (isOn)
    {
        // Filled accent background
        g.setColour(COLOUR_4.withAlpha(0.18f));
        g.fillRect(box);

        // Horizontal bar through the centre — same language as linear thumb
        const float barH  = 2.5f;
        const float barY  = boxY + (boxSize - barH) * 0.5f;
        g.setColour(COLOUR_4);
        g.fillRect(juce::Rectangle<float>(boxX, barY, boxSize, barH));

        // Ice-blue end caps — same detail as linear slider thumb
        g.setColour(COLOUR_1.withAlpha(0.7f));
        g.fillRect(juce::Rectangle<float>(boxX,                    barY, 2.f, barH));
        g.fillRect(juce::Rectangle<float>(boxX + boxSize - 2.f,    barY, 2.f, barH));

        // Outer border — accent colour when on
        g.setColour(COLOUR_4.withAlpha(0.75f));
        g.drawRect(box, 1.f);
    }
    // -------------------------------------------------------------------------
    // OFF state: just the dim border
    // -------------------------------------------------------------------------
    else
    {
        g.setColour(COLOUR_2.withAlpha(0.4f));
        g.drawRect(box, 1.f);
    }

    // -------------------------------------------------------------------------
    // Corner ticks — small L-shaped marks at two opposite corners
    // Mirrors the tick-mark language from the rotary / linear sliders
    // -------------------------------------------------------------------------
    const float tickLen = 4.f;
    const float tickW   = 1.2f;
    const juce::Colour tickCol = isOn
                               ? COLOUR_1.withAlpha(0.55f)
                               : COLOUR_2.withAlpha(0.30f);
    g.setColour(tickCol);

    // Top-left
    g.drawLine(boxX - 3.f, boxY,        boxX - 3.f + tickLen, boxY,        tickW); // horizontal
    g.drawLine(boxX - 3.f, boxY,        boxX - 3.f,           boxY + tickLen, tickW); // vertical

    // Bottom-right
    const float brX = boxX + boxSize + 2.f;
    const float brY = boxY + boxSize;
    g.drawLine(brX - tickLen, brY, brX, brY,        tickW);
    g.drawLine(brX,           brY - tickLen, brX, brY, tickW);

    // -------------------------------------------------------------------------
    // Label text
    // -------------------------------------------------------------------------
    const float labelX = boxX + boxSize + 10.f;
    const juce::Colour textCol = isOn
                               ? COLOUR_0
                               : COLOUR_2.withAlpha(0.65f);

    g.setColour(textCol);
    g.setFont(juce::Font(juce::FontOptions(12.f).withStyle("Regular")));
    g.drawText(button.getButtonText(),
               juce::Rectangle<float>(labelX, 0.f,
                                      bounds.getWidth() - labelX, h),
               juce::Justification::centredLeft, true);
}