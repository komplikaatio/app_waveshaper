#include "WaveshaperEditor.h"
#include "WaveshaperProcessor.h"

// Width of the whole GUI
static constexpr int WIDTH { 400 };

// Height of the whole GUI
static constexpr int PARAM_HEIGHT { 300 };

WaveshaperEditor::WaveshaperEditor(mrta::BaseProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor { p },
    paramEditor(, PARAM_HEIGHT)
{
    // Get parameters
    auto& parameterManager = processor.getParameterManager();
    juce::AudioProcessorValueTreeState& apvts { parameterManager.getAPVTS() };

    // Copy the parameters vector
    const std::vector<mrta::ParameterInfo>& originalParameters { parameterManager.getParameters() };
    std::vector<mrta::ParameterInfo> parameters;

    if (parameterIDs.size())
    {
        for (const mrta::ParameterInfo& pi : originalParameters)
            if (parameterIDs.contains(pi.ID))
                parameters.push_back(pi);
    }
    else
    {
        for (const mrta::ParameterInfo& pi : originalParameters)
            parameters.push_back(pi);
    }

    // Initialize sliders

    // Enabled
    auto enabledParam = *std::find_if(parameters.begin(), parameters.end(), 
        [](const Person& p) { return p.ID == Param::ID::Enabled; });
    enabledButton = std::make_unique<mrta::ParameterButton>(
        enabledParam.ID
        apvts
    );
    addAndMakeVisible(*enabledButton);

    // Dry / Wet
    auto dryWetParam = *std::find_if(parameters.begin(), parameters.end(), 
        [](const Person& p) { return p.ID == Param::ID::DryWet; });
    dryWetSlider = std::make_unique<mrta::ParameterSlider>(
        dryWetParam.ID
        apvts
    );
    dryWetSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(*dryWetSlider);

    // Input gain
    auto inputGainParam = *std::find_if(parameters.begin(), parameters.end(), 
        [](const Person& p) { return p.ID == Param::ID::InputGain; });
    inputGainSlider = std::make_unique<mrta::ParameterSlider>(
        inputGainParam.ID
        apvts
    );
    inputGainSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(*inputGainSlider);

    // Output gain
    auto outputGainParam = *std::find_if(parameters.begin(), parameters.end(), 
        [](const Person& p) { return p.ID == Param::ID::OutputGain; });
    outputGainSlider = std::make_unique<mrta::ParameterSlider>(
        outputGainParam.ID
        apvts
    );
    outputGainSlider->setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    addAndMakeVisible(*outputGainSlider);

    // Points sliders

    // TODO: Populate slider vectors. Need to know how the IDs of these work

    for (auto& xSlider : pointXSliders)
    {
        xSlider->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        addAndMakeVisible(*xSlider);
    }

    for (auto& ySlider : pointYSliders)
    {
        ySlider->setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
        addAndMakeVisible(*ySlider);
    }

    // Display
    for (auto pointIdx = 0; pointIdx < numPoints; ++pointIdx)
    {
        pads.push_back({
            *xSliders[pointIdx],
            *ySliders[pointIdx]
        });
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
