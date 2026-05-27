#include "WaveshaperEditor.h"

// Width of the whole GUI
static constexpr int WIDTH { 1000 };

// Height of the whole GUI
static constexpr int HEIGHT { 650 };

WaveshaperEditor::WaveshaperEditor(WaveshaperProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor   { p },
    waveshaper  { p.getWaveshaper() },

    // InitializeSliders
    enabledButton   ( Param::ID::Enabled        , processor.getParameterManager().getAPVTS() ),
    dryWetSlider    ( Param::ID::DryWet         , processor.getParameterManager().getAPVTS() ),
    inputGainSlider ( Param::ID::InputGain      , processor.getParameterManager().getAPVTS() ),
    outputGainSlider( Param::ID::OutputGain     , processor.getParameterManager().getAPVTS() ),
    yLeftSlider     ( Param::ID::LeftY          , processor.getParameterManager().getAPVTS() ),
    yRightSlider    ( Param::ID::RightY         , processor.getParameterManager().getAPVTS() )
{
    auto& apvts = processor.getParameterManager().getAPVTS();

    // Set button text (NO text better)
    enabledButton.setButtonText("");

    // Initialize points
    for (auto pointIdx = 0; pointIdx < NumPoints; ++pointIdx)
    {
        auto point = std::make_unique<WSPointComponent>(
            pointIdx,
            apvts,
            waveshaper
        );

        pads.push_back(std::make_unique<XYPad>(
            point->getXSlider(), point->getYSlider()
        ));

        pointComponents.push_back(std::move(point));
    }

    // Set sliders style
    dryWetSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    inputGainSlider .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yLeftSlider     .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yRightSlider    .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    dryWetSlider    .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    inputGainSlider .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    yLeftSlider     .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);
    yRightSlider    .setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 40, 15);

    // Set labels justification
    dryWetLabel     .setJustificationType(juce::Justification::centred);
    inputGainLabel  .setJustificationType(juce::Justification::centred);
    outputGainLabel .setJustificationType(juce::Justification::centred);
    xLabel          .setJustificationType(juce::Justification::centred);
    yLabel          .setJustificationType(juce::Justification::centred);
    xRangeLabel     .setJustificationType(juce::Justification::centred);
    yRangeLabel     .setJustificationType(juce::Justification::centred);
    xRateLabel      .setJustificationType(juce::Justification::centred);
    yRateLabel      .setJustificationType(juce::Justification::centred);

    // Add components
    addAndMakeVisible(enabledButton);
    addAndMakeVisible(dryWetSlider);
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(yLeftSlider);
    addAndMakeVisible(yRightSlider);
    addAndMakeVisible(dryWetLabel );
    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(outputGainLabel );
    addAndMakeVisible(xLabel);
    addAndMakeVisible(yLabel);
    addAndMakeVisible(xRangeLabel);
    addAndMakeVisible(yRangeLabel);
    addAndMakeVisible(xRateLabel);
    addAndMakeVisible(yRateLabel);
    for (auto& point : pointComponents)
    {
        addAndMakeVisible(*point);
    }
    for (auto& pad : pads)
    {
        addAndMakeVisible(*pad);
    }

    // Set Look and Feel
    setLookAndFeel(&wsLaf);

    // Start timer
    startTimerHz(30);

    // Alocate space for point positions
    currentXs.resize(NumPoints);
    currentYs.resize(NumPoints);

    // Set window size
    setSize(WIDTH, HEIGHT);
}

WaveshaperEditor::~WaveshaperEditor()
{
    setLookAndFeel(nullptr);
}

void WaveshaperEditor::paint(juce::Graphics& g)
{
    // Get custom Look and Feel
    auto* laf = dynamic_cast<WSLaf*>(&getLookAndFeel());
    if (!laf) return;

    // Draw background
    laf->drawDocumentWindowBackground(g, getLocalBounds());

    // Draw display

    // Get (atomically) current points position and normalize
    for(auto pointIdx = 0; pointIdx < NumPoints; ++pointIdx)
    {
        auto x = waveshaper.getCurrentX(pointIdx);
        auto y = waveshaper.getCurrentY(pointIdx);

        // Convert to position on screen
        x = displayBounds.getWidth() * ((x + 1.f) * 0.5f);
        y = displayBounds.getHeight() * (1.f - (y + 1.f) * 0.5f); // Invert

        currentXs[pointIdx] = x;
        currentYs[pointIdx] = y;
    }

    auto yLeft   = displayBounds.getHeight() * (1.f - (yLeftSlider.getValue() + 1.f) * 0.5f); // Invert
    auto yRight  = displayBounds.getHeight() * (1.f - (yRightSlider.getValue() + 1.f) * 0.5f); // Invert

    laf->drawDisplay(g, displayBounds, currentXs, currentYs, yLeft, yRight);

    // Draw title
    laf->drawTitle(g, titleBounds);
}

void WaveshaperEditor::resized()
{
    const auto margin = 5;
    auto bounds = getLocalBounds().reduced(margin);

    const auto sideWidth        = bounds.getWidth() / 9;
    const auto titleHeight      = bounds.getHeight() / 12;
    const auto displayHeight    = bounds.getHeight() / 3 - titleHeight;
    const auto yWidth           = sideWidth / 2;
    const auto labelHeight      = bounds.getHeight() / 30;
    const auto enabledHeight    = bounds.getHeight() / 20;
    const auto dryWetHeight     = bounds.getHeight() / 12;

    // Left side
    auto leftBounds     =       bounds          .removeFromLeft (sideWidth);
    auto xyLabelBounds  =       leftBounds      .removeFromRight(yWidth);
    
    yLeftSlider     .setBounds( xyLabelBounds   .removeFromTop(displayHeight + titleHeight).removeFromBottom(displayHeight));
    xLabel          .setBounds( xyLabelBounds   .removeFromTop(xyLabelBounds.getHeight() / 2));
    yLabel          .setBounds( xyLabelBounds);

    enabledButton   .setBounds( leftBounds .removeFromTop(enabledHeight));
    inputGainLabel  .setBounds( leftBounds .removeFromBottom(labelHeight));
    inputGainSlider .setBounds( leftBounds);

    // Right side
    auto rightBounds = bounds.removeFromRight(sideWidth);

    yRightSlider    .setBounds( rightBounds.removeFromLeft(yWidth).removeFromTop(displayHeight + titleHeight).removeFromBottom(displayHeight));
    
    dryWetLabel     .setBounds( rightBounds.removeFromTop(labelHeight));
    dryWetSlider    .setBounds( rightBounds.removeFromTop(dryWetHeight));
    outputGainLabel .setBounds( rightBounds.removeFromTop(labelHeight));
    outputGainSlider.setBounds( rightBounds);

    // Centre
    titleBounds     = bounds.removeFromTop(titleHeight);
    displayBounds   = bounds.removeFromTop(displayHeight);
    auto padsBounds = displayBounds;
    
    const auto pointWidth = bounds.getWidth() / NumPoints;
    for (auto pointIdx = 0; pointIdx < NumPoints - 1; ++pointIdx)
    {
        pointComponents[pointIdx]   ->setBounds(bounds      .removeFromLeft(pointWidth));
        pads[pointIdx]              ->setBounds(padsBounds  .removeFromLeft(pointWidth));
    }
    // Assign rest to last point
    pointComponents[NumPoints - 1]->setBounds(bounds);
    pads[NumPoints - 1]->setBounds(padsBounds);

}

void WaveshaperEditor::timerCallback()
{
    repaint();
}
