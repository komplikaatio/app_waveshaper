#include "WaveshaperEditor.h"
#include "WaveshaperProcessor.h"

// Width of the whole GUI
static constexpr int WIDTH { 600 };

// Height of the whole GUI
static constexpr int HEIGHT { 400 };

WaveshaperEditor::WaveshaperEditor(mrta::BaseProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor   { p },
    waveshaper  { p.getWaveshaper() },

    // InitializeSliders
    enabledButton   ( Param::ID::Enabled        , processor.getParameterManager().getAPVTS() ),
    dryWetSlider    ( Param::ID::DryWet         , processor.getParameterManager().getAPVTS() ),
    inputGainSlider ( Param::ID::InputGain      , processor.getParameterManager().getAPVTS() ),
    outputGainSlider( Param::ID::OutputGain     , processor.getParameterManager().getAPVTS() ),
    yLeftSlider     ( Param::ID::OutputGain     , processor.getParameterManager().getAPVTS() ),
    yRightSlider    ( Param::ID::OutputGain     , processor.getParameterManager().getAPVTS() )
{
    auto& apvts = processor.getParameterManager().getAPVTS();

    // Set button text
    enabledButton.setButtonText(Param::Name::Enabled);

    // Initialize points
    for (auto pointIdx = 0; pointIdx < DSP::WaveShaper::WS_POINTS)
    {
        pointComponents.push_back({
            pointIdx,
            apvts,
            waveshaper
        });
    }

    // Set sliders style
    dryWetSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    inputGainSlider .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yLeftSlider     .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yRightSlider    .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    // Add components
    addAndMakeVisible(dryWetSlider);
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(yLeftSlider);
    addAndMakeVisible(yRightSlider);
    for (auto& point : pointComponents)
    {
        addAndMakeVisible(point);
    }

    // Set window size
    setSize(WIDTH, HEIGHT);
}

WaveshaperEditor::~WaveshaperEditor()
{
}

void WaveshaperEditor::paint(juce::Graphics& g)
{
    // Get custom Look and Feel
    auto* laf = dynamic_cast<WSLaf*>(&getLookAndFeel());
    if (!laf) return;

    // Draw background
    laf->drawDocumentWindowBackground(g, getLocalBounds());

    // Get point positions
    std::vector<double> x;
    std::vector<double> y;
    for (auto pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
        auto& xSlider = pointXSliders[pointIdx];
        auto& ySlider = pointYSliders[pointIdx];
        x.push_back(xSlider.getValue());
        y.push_back(ySlider.getValue());
    }

    // Draw display
    laf->drawDisplay(
        g,
        displayArea,
        x,
        y
    );
}

void WaveshaperEditor::resized()
{
    auto bounds = getLocalBounds();

    // Dry wet / Enabled
    const auto dryWetWidth = bounds.getWidth() / 10;
    auto dryWetBounds = bounds.removeFromRight(dryWetWidth);
    const auto enabledHeigth     = dryWetBounds.getHeight() / 8;
    const auto dryWetLabelHeight = (dryWetBounds.getHeight() - enabledHeight) / 8;
    enabledButton   .setBounds(dryWetBounds.removeFromTop(enabledHeight));
    dryWetLabel     .setBounds(dryWetBounds.removeFromTop(dryWetLabelHeight));
    dryWetSlider    .setBounds(dryWetBounds);

    // Input / output gain
    const auto gainWidth = bounds.getWidth() / 10;

    auto inputGainBounds  = bounds.removeFromLeft (gainWidth);
    auto outputGainBounds = bounds.removeFromRight(gainWidth);

    // Title bar
    const auto titleHeight = bounds.getHeight() / 8;
    auto titleBounds = inputGainBounds.removeFromTop(titleHeight);
    outputGainBounds.removeFromTop(titleHeight);

    
    // Input / output gain labels
    const auto labelHeight = inputGainBounds.getHeight() / 8;

    // Set bounds
    titleLabel      .setBounds(titleBounds);
    inputGainLabel  .setBounds(inputGainBounds  .removeFromTop(labelHeight));
    outputGainLabel .setBounds(outputGainBounds .removeFromTop(labelHeight));
    inputGainSlider .setBounds(inputGainBounds);
    outputGainSlider.setBounds(outputGainBounds);

    // Points
    const auto numPoints        = pointXSliders.size();
    const auto xyLabelsWidth    = bounds.getWidth() / numPoints / 3;
    const auto pointWidth       = (bounds.getWidth() - xyLabelsWidth) / numPoints;
    const auto displayHeight    = bounds.getHeight() / 3 * 2;
    const auto sliderHeight     = (bounds.getHeight() - displayHeight) / 2;

    auto xLabelBounds = bounds.removeFromLeft(xyLabelsWidth).removeFromTop(displayHeight);
    yLabel.setBounds(xLabelBounds.removeFromTop(xLabelBounds.getHeight() / 2));
    xLabel.setBounds(xLabelBounds);

    auto displayBounds = bounds; // Copy
    displayArea = displayBounds.removeFromTop(displayHeight);

    for (auto pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
        auto& xSlider = pointXSliders[pointIdx];
        auto& ySlider = pointYSliders[pointIdx];
        auto pointBounds    = bounds        .removeFromLeft(pointWidth);
        auto displayBounds  = pointBounds   .removeFromTop(displayHeight);
        ySlider.setBounds(pointBounds.removeFromTop(sliderHeight));
        xSlider.setBounds(pointBounds);
    }
}
