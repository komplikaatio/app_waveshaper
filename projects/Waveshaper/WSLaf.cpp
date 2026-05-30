#include "WSLaf.h"
#include <cassert>

void WSLaf::drawDocumentWindowBackground(
    juce::Graphics& g,
    juce::Rectangle<int> bounds
)
{
    // Create a linear gradient between the first two colors
    juce::ColourGradient gradient (COLOUR_0, 0.0f, 0.0f, 
                                    COLOUR_0, 0.0f, (float)bounds.getX() + (float)bounds.getHeight(), false);

    // Add the third color at the 50% mark (0.5 proportion)
    gradient.addColour (0.5, COLOUR_0);

    // Apply the gradient and fill the area
    g.setGradientFill (gradient);
    g.fillRect (bounds.toFloat());
}

void WSLaf::drawDisplay(
    juce::Graphics & g,
    juce::Rectangle<int> bounds,
    std::vector<float> xPos,
    std::vector<float> yPos,
    float yLeft,
    float yRight
)
{
    assert(xPos.size() == yPos.size());

    g.setColour(COLOUR_1);
    g.fillRect(bounds);

    const auto numPoints    = xPos.size();
    const auto pointRadius  = 5.f;
    const auto height       = bounds.getHeight();
    const auto width        = bounds.getWidth();
    const auto pointWidth   = width / numPoints;
    const auto thickness    = 2.f;
    const auto gridThickness= 0.5f;

    // Draw  grid lines
    juce::Path gridLines;
    // Vertical lines
    for (auto pointIdx = 0; pointIdx <= numPoints; ++pointIdx)
    {
        auto x = bounds.getX() + pointWidth * pointIdx;
        gridLines.startNewSubPath(x, bounds.getY());
        gridLines.lineTo(x, bounds.getY() + height);
    }

    // Middle vertical line
    gridLines.startNewSubPath(bounds.getX() + width * 0.5f, bounds.getY());
    gridLines.lineTo(bounds.getX() + width * 0.5f, bounds.getY() + height);

    // Horizontal lines
    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.5f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.5f);

    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.25f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.25f);

    gridLines.startNewSubPath(bounds.getX(), bounds.getY() + height * 0.75f);
    gridLines.lineTo(bounds.getX() + width, bounds.getY() + height * 0.75f);

    g.setColour(COLOUR_2);
    g.strokePath(gridLines, juce::PathStrokeType(gridThickness));

    // Draw line conecting points
    juce::Path path;
    path.startNewSubPath(bounds.getX(), bounds.getY() + yLeft);

    g.setColour(COLOUR_3);

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
            const float t2 = t  * t;
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
    g.strokePath(path, juce::PathStrokeType(thickness));

    // Draw points on top of line
    g.setColour(COLOUR_4);
    for (auto pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
        auto x = xPos[pointIdx];
        auto y = yPos[pointIdx];

        g.drawEllipse(bounds.getX() + x - pointRadius * 0.5f,
                    bounds.getY() + y - pointRadius * 0.5f,
                    pointRadius, pointRadius, thickness);
    }

}



// ************************************ AI BELOW ************************************

void WSLaf::drawTitle(
    juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    const auto  b    = bounds.toFloat();
    const float cx   = b.getCentreX();
    const float cy   = b.getCentreY();

    const juce::String  title    = "waveshaper";
    const float         fontSize = b.getHeight() * 0.52f;
    const juce::Font    font { juce::FontOptions(fontSize).withStyle("Bold") };
    const float         textW = bounds.getWidth() / 3 * 2 ; // font.getStringWidthFloat(title);

    // -------------------------------------------------------------------------
    // Background: diagonal slash fragments — electric interference pattern
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_4.withAlpha(0.08f));
    const float slashSpacing = b.getWidth() / 9.f;
    for (int i = 0; i <= 9; ++i)
    {
        const float x = b.getX() + slashSpacing * i;
        g.drawLine(x + 8.f, b.getY(), x - 8.f, b.getBottom(), 0.7f);
    }

    // -------------------------------------------------------------------------
    // Glow bloom — concentric text passes, fading outward
    // -------------------------------------------------------------------------
    g.setFont(font);
    for (int pass = 6; pass >= 1; --pass)
    {
        const float spread = pass * 1.4f;
        const float alpha  = 0.022f * (7 - pass);
        g.setColour(COLOUR_4.withAlpha(alpha));
        for (float dx : { -spread, 0.f, spread })
            for (float dy : { -spread, 0.f, spread })
                g.drawText(title, b.translated(dx, dy).toNearestInt(),
                           juce::Justification::centred, false);
    }

    // -------------------------------------------------------------------------
    // Chromatic aberration — split RGB ghost layers
    // -------------------------------------------------------------------------
    g.setFont(font);
    g.setColour(juce::Colour(0xffff1a3a).withAlpha(0.22f));   // red, left
    g.drawText(title, b.translated(-2.5f, 0.f).toNearestInt(),
               juce::Justification::centred, false);

    g.setColour(juce::Colour(0xff1a3aff).withAlpha(0.22f));   // blue, right
    g.drawText(title, b.translated(2.5f, 0.f).toNearestInt(),
               juce::Justification::centred, false);

    // -------------------------------------------------------------------------
    // Main text body — lavender base
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_5);                                     // 0xff9e7ef9
    g.drawText(title, b.toNearestInt(), juce::Justification::centred, false);

    // -------------------------------------------------------------------------
    // Top-half specular highlight — makes it feel lit from above
    // -------------------------------------------------------------------------
    {
        g.saveState();
        g.reduceClipRegion(b.withHeight(b.getHeight() * 0.48f).toNearestInt());
        g.setColour(juce::Colours::white.withAlpha(0.13f));
        g.drawText(title, b.toNearestInt(), juce::Justification::centred, false);
        g.restoreState();
    }

    // -------------------------------------------------------------------------
    // Electric underline — jagged waveform-like stroke beneath the text
    // -------------------------------------------------------------------------
    {
        const float lineY  = cy + fontSize * 0.58f;
        const float lineX0 = cx - textW * 0.5f;
        const float lineX1 = cx + textW * 0.5f;
        const float jag    = 2.4f;
        const float step   = 5.f;

        juce::Path wave;
        wave.startNewSubPath(lineX0, lineY);
        bool up = true;
        for (float x = lineX0 + step; x <= lineX1; x += step, up = !up)
            wave.lineTo(x, lineY + (up ? -jag : jag));
        wave.lineTo(lineX1, lineY);

        g.setColour(juce::Colour(0xffecf97e).withAlpha(0.7f));          // electric yellow
        g.strokePath(wave, juce::PathStrokeType(1.1f));

        // Glow duplicate underneath
        g.setColour(juce::Colour(0xffecf97e).withAlpha(0.15f));
        g.strokePath(wave, juce::PathStrokeType(3.5f));
    }

    // -------------------------------------------------------------------------
    // Flanking oscilloscope lines with tick marks
    // -------------------------------------------------------------------------
    const float ruleY   = cy;
    const float gap     = 8.f;
    const float ruleX0  = cx - textW * 0.5f - gap;
    const float ruleX1  = cx + textW * 0.5f + gap;

    g.setColour(COLOUR_4.withAlpha(0.55f));
    g.drawLine(b.getX() + 6.f, ruleY, ruleX0, ruleY, 1.f);
    g.drawLine(ruleX1, ruleY, b.getRight() - 6.f, ruleY, 1.f);

    g.setColour(COLOUR_5.withAlpha(0.35f));
    for (float x = b.getX() + 10.f; x < ruleX0 - 4.f; x += 5.f)
        g.drawLine(x, ruleY - 2.5f, x, ruleY + 2.5f, 0.8f);
    for (float x = ruleX1 + 4.f; x < b.getRight() - 10.f; x += 5.f)
        g.drawLine(x, ruleY - 2.5f, x, ruleY + 2.5f, 0.8f);

    // -------------------------------------------------------------------------
    // Corner L-ticks — all four corners, matching LAF language
    // -------------------------------------------------------------------------
    const float tkL = 5.f;
    const float tkW = 1.2f;
    g.setColour(COLOUR_4.withAlpha(0.65f));

    // Top-left
    g.drawLine(b.getX(),            b.getY(), b.getX() + tkL, b.getY(),        tkW);
    g.drawLine(b.getX(),            b.getY(), b.getX(),        b.getY() + tkL, tkW);
    // Top-right
    g.drawLine(b.getRight() - tkL,  b.getY(), b.getRight(),   b.getY(),        tkW);
    g.drawLine(b.getRight(),         b.getY(), b.getRight(),   b.getY() + tkL, tkW);
    // Bottom-left
    g.drawLine(b.getX(),            b.getBottom(), b.getX() + tkL, b.getBottom(),        tkW);
    g.drawLine(b.getX(),            b.getBottom() - tkL, b.getX(), b.getBottom(),        tkW);
    // Bottom-right
    g.drawLine(b.getRight() - tkL,  b.getBottom(), b.getRight(), b.getBottom(),          tkW);
    g.drawLine(b.getRight(),         b.getBottom() - tkL, b.getRight(), b.getBottom(),   tkW);
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
                                                juce::PathStrokeType::JointStyle::curved,
                                                juce::PathStrokeType::EndCapStyle::rounded));
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
                                                     juce::PathStrokeType::JointStyle::curved,
                                                     juce::PathStrokeType::EndCapStyle::rounded));
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
    float minSliderPos,
    float maxSliderPos,
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
void WSLaf::drawButtonBackground(
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour& backgroundColour,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto bounds = button.getLocalBounds().toFloat();
    const bool isOn   = button.getToggleState();

    // -------------------------------------------------------------------------
    // Hover / press state — matches toggle box hover language
    // -------------------------------------------------------------------------
    if (shouldDrawButtonAsDown)
    {
        g.setColour(COLOUR_4.withAlpha(0.12f));
        g.fillRect(bounds);
    }
    else if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(COLOUR_2.withAlpha(0.10f));
        g.fillRect(bounds);
    }

    // -------------------------------------------------------------------------
    // Body fill
    // -------------------------------------------------------------------------
    g.setColour(COLOUR_3);
    g.fillRect(bounds);

    // -------------------------------------------------------------------------
    // ON state: accent fill + horizontal bar (mirrors toggle indicator)
    // -------------------------------------------------------------------------
    if (isOn)
    {
        g.setColour(COLOUR_4.withAlpha(0.18f));
        g.fillRect(bounds);

        const float barH = 2.5f;
        const float barY = bounds.getCentreY() - barH * 0.5f;
        g.setColour(COLOUR_4);
        g.fillRect(juce::Rectangle<float>(bounds.getX(), barY, bounds.getWidth(), barH));

        // Ice-blue end caps
        g.setColour(COLOUR_1.withAlpha(0.7f));
        g.fillRect(juce::Rectangle<float>(bounds.getX(),                        barY, 2.f, barH));
        g.fillRect(juce::Rectangle<float>(bounds.getX() + bounds.getWidth() - 2.f, barY, 2.f, barH));

        g.setColour(COLOUR_4.withAlpha(0.75f));
        g.drawRect(bounds, 1.f);
    }
    // -------------------------------------------------------------------------
    // OFF state: dim border
    // -------------------------------------------------------------------------
    else
    {
        g.setColour(COLOUR_2.withAlpha(0.4f));
        g.drawRect(bounds, 1.f);
    }

    // -------------------------------------------------------------------------
    // Corner ticks — same L-shaped marks at two opposite corners
    // -------------------------------------------------------------------------
    const float tickLen = 4.f;
    const float tickW   = 1.2f;
    const juce::Colour tickCol = isOn
                               ? COLOUR_1.withAlpha(0.55f)
                               : COLOUR_2.withAlpha(0.30f);
    g.setColour(tickCol);

    // Top-left
    g.drawLine(bounds.getX(),              bounds.getY(), bounds.getX() + tickLen, bounds.getY(),        tickW);
    g.drawLine(bounds.getX(),              bounds.getY(), bounds.getX(),           bounds.getY() + tickLen, tickW);

    // Bottom-right
    const float brX = bounds.getRight();
    const float brY = bounds.getBottom();
    g.drawLine(brX - tickLen, brY, brX, brY,             tickW);
    g.drawLine(brX,           brY - tickLen, brX, brY,   tickW);
}

void WSLaf::drawButtonText(
    juce::Graphics& g,
    juce::TextButton& button,
    bool shouldDrawButtonAsHighlighted,
    bool shouldDrawButtonAsDown)
{
    const auto  bounds = button.getLocalBounds().toFloat();
    const bool  isOn   = button.getToggleState();

    const juce::Colour textCol = isOn
                               ? COLOUR_0
                               : COLOUR_2.withAlpha(0.65f);

    g.setColour(textCol);
    g.setFont(juce::Font(juce::FontOptions(12.f).withStyle("Regular")));
    g.drawText(button.getButtonText(), bounds, juce::Justification::centred, true);
}