#include "WaveshaperEditor.h"

static constexpr int WIDTH  { 1000 };
static constexpr int HEIGHT { 550 };

WaveshaperEditor::WaveshaperEditor(WaveshaperProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor   { p },
    waveshaper  { p.getWaveshaper() },

    // Initialize sliders
    enabledButton   ( Param::ID::Enabled        , processor.getParameterManager().getAPVTS() ),
    dryWetSlider    ( Param::ID::DryWet         , processor.getParameterManager().getAPVTS() ),
    inputGainSlider ( Param::ID::InputGain      , processor.getParameterManager().getAPVTS() ),
    outputGainSlider( Param::ID::OutputGain     , processor.getParameterManager().getAPVTS() ),
    yLeftSlider     ( Param::ID::LeftY          , processor.getParameterManager().getAPVTS() ),
    yRightSlider    ( Param::ID::RightY         , processor.getParameterManager().getAPVTS() )
{
    auto& apvts = processor.getParameterManager().getAPVTS();

    enabledButton.setButtonText("");
    enabledButton.setClickingTogglesState(true);

    // Initialize point components + pads
    for (auto pointIdx = 0; pointIdx < NumPoints; ++pointIdx)
    {
        auto point = std::make_unique<WSPointComponent>(
            pointIdx, apvts, waveshaper
        );

        pads.push_back(std::make_unique<XYPad>(
            point->getXSlider(), point->getYSlider()
        ));

        pointComponents.push_back(std::move(point));
    }

    // Drive, Mix, Output
    dryWetSlider    .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    inputGainSlider .setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    outputGainSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);

    // yLeft / yRight vertical sliders 
    yLeftSlider     .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    yRightSlider    .setSliderStyle(juce::Slider::SliderStyle::LinearVertical);

    // Text boxes
    dryWetSlider    .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 15);
    inputGainSlider .setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 15);
    outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 48, 15);
    yLeftSlider     .setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 14);
    yRightSlider    .setTextBoxStyle(juce::Slider::TextBoxBelow, true, 40, 14);

    // Force invisible text box borders directly on each slider
    auto hideTextBox = [](juce::Slider& s)
    {
        s.setColour(juce::Slider::textBoxOutlineColourId,    juce::Colours::transparentBlack);
        s.setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
    };
    hideTextBox(dryWetSlider);
    hideTextBox(inputGainSlider);
    hideTextBox(outputGainSlider);
    hideTextBox(yLeftSlider);
    hideTextBox(yRightSlider);

    // Force digit formatting 
    auto fmtDb  = [](double v) { return juce::String(v, 1); };   
    auto fmtMix = [](double v) { return juce::String(v, 2); };   
    auto fmtPos = [](double v) { return juce::String(v, 2); }; 

    inputGainSlider .textFromValueFunction = fmtDb;
    outputGainSlider.textFromValueFunction = fmtDb;
    dryWetSlider    .textFromValueFunction = fmtMix;
    yLeftSlider     .textFromValueFunction = fmtPos;
    yRightSlider    .textFromValueFunction = fmtPos;

    // Label styling
    auto styleLabel = [](juce::Label& label, float fontSize = 11.f)
    {
        label.setJustificationType(juce::Justification::centred);
        label.setFont(juce::Font(juce::FontOptions(fontSize)));
    };
    styleLabel(dryWetLabel);
    styleLabel(inputGainLabel);
    styleLabel(outputGainLabel);
    styleLabel(xLabel, 9.f);
    styleLabel(yLabel, 9.f);
    styleLabel(controlYLabel, 12.f);
    styleLabel(controlXLabel, 12.f);

    // Add components
    addAndMakeVisible(enabledButton);
    addAndMakeVisible(dryWetSlider);
    addAndMakeVisible(inputGainSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(yLeftSlider);
    addAndMakeVisible(yRightSlider);
    addAndMakeVisible(dryWetLabel);
    addAndMakeVisible(inputGainLabel);
    addAndMakeVisible(outputGainLabel);
    addAndMakeVisible(xLabel);
    addAndMakeVisible(yLabel);
    addAndMakeVisible(controlYLabel);
    addAndMakeVisible(controlXLabel);

    for (auto& point : pointComponents) { addAndMakeVisible(*point); }
    for (auto& pad : pads)              { addAndMakeVisible(*pad); }

    setLookAndFeel(&wsLaf);
    startTimerHz(30);

    currentXs.resize(NumPoints);
    currentYs.resize(NumPoints);

    setSize(WIDTH, HEIGHT);
}

WaveshaperEditor::~WaveshaperEditor()
{
    setLookAndFeel(nullptr);
}

void WaveshaperEditor::paint(juce::Graphics& g)
{
    auto* laf = dynamic_cast<WSLaf*>(&getLookAndFeel());
    if (!laf) return;

    laf->drawDocumentWindowBackground(g, getLocalBounds());

    for (auto pointIdx = 0; pointIdx < NumPoints; ++pointIdx)
    {
        auto x = waveshaper.getCurrentX(pointIdx);
        auto y = waveshaper.getCurrentY(pointIdx);

        x = displayBounds.getWidth()  * ((x + 1.f) * 0.5f);
        y = displayBounds.getHeight() * (1.f - (y + 1.f) * 0.5f);

        currentXs[pointIdx] = x;
        currentYs[pointIdx] = y;
    }

    auto yLeft  = displayBounds.getHeight() * (1.f - (yLeftSlider.getValue()  + 1.f) * 0.5f);
    auto yRight = displayBounds.getHeight() * (1.f - (yRightSlider.getValue() + 1.f) * 0.5f);

    laf->drawDisplay(g, displayBounds, currentXs, currentYs, yLeft, yRight);
}

void WaveshaperEditor::resized()
{
    const auto margin = 5;
    auto bounds = getLocalBounds().reduced(margin);

    // Vertical proportions
    const auto titleHeight   = bounds.getHeight() / 12;
    const auto bottomRowH    = bounds.getHeight() / 6;
    const auto labelHeight   = 14;
    const auto enabledW      = 36;
    const auto enabledH      = 24;
    const auto sideWidth     = bounds.getWidth() / 18;  

    // Bottom row: Drive, Mix, Output knobs
    auto bottomBounds = bounds.removeFromBottom(bottomRowH);

    const auto knobAreaWidth = juce::jmin(bottomBounds.getWidth() * 2 / 3, 420);
    auto knobArea = bottomBounds.withSizeKeepingCentre(knobAreaWidth, bottomBounds.getHeight());
    const auto knobColW = knobArea.getWidth() / 3;

    auto driveBounds  = knobArea.removeFromLeft(knobColW);
    auto mixBounds    = knobArea.removeFromLeft(knobColW);
    auto outputBounds = knobArea;

    inputGainLabel  .setBounds(driveBounds .removeFromTop(labelHeight));
    inputGainSlider .setBounds(driveBounds);

    dryWetLabel     .setBounds(mixBounds   .removeFromTop(labelHeight));
    dryWetSlider    .setBounds(mixBounds);

    outputGainLabel .setBounds(outputBounds.removeFromTop(labelHeight));
    outputGainSlider.setBounds(outputBounds);

    // Title row: enabled button, space for title
    auto titleRow = bounds.removeFromTop(titleHeight);
    enabledButton.setBounds(titleRow.removeFromLeft(enabledW).withSizeKeepingCentre(enabledW, enabledH));
    titleBounds = titleRow;

    // Display
    const auto displayHeight = bounds.getHeight() / 2;
    auto displayRow = bounds.removeFromTop(displayHeight);

    // yLeft and yRight
    yLeftSlider .setBounds(displayRow.removeFromLeft(sideWidth));
    yRightSlider.setBounds(displayRow.removeFromRight(sideWidth));
    displayBounds = displayRow;
    auto padsBounds = displayBounds;

    // Point components below display:
    // Align with display by trimming same side widths
    bounds.removeFromLeft(sideWidth);
    bounds.removeFromRight(sideWidth);

    const auto titleRowH = 25;

    // "Control Y"
    auto controlYRow  = bounds.removeFromTop(titleRowH);
    separatorTopY     = controlYRow.getCentreY();
    controlYLabel.setBounds(controlYRow);

    // "Control X" 
    auto controlXRow  = bounds.removeFromBottom(titleRowH);
    separatorBottomY  = controlXRow.getCentreY();
    controlXLabel.setBounds(controlXRow);

    // Point components
    const auto pointWidth = bounds.getWidth() / NumPoints;
    for (auto pointIdx = 0; pointIdx < NumPoints - 1; ++pointIdx)
    {
        pointComponents[pointIdx]->setBounds(bounds    .removeFromLeft(pointWidth));
        pads[pointIdx]           ->setBounds(padsBounds.removeFromLeft(pointWidth));
    }
    pointComponents[NumPoints - 1]->setBounds(bounds);
    pads[NumPoints - 1]           ->setBounds(padsBounds);
}

void WaveshaperEditor::timerCallback()
{
    repaint();
}