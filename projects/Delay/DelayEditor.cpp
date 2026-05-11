#include "DelayEditor.h"

#include "DelayProcessor.h"

// Width of the whole GUI
static constexpr int WIDTH { 250 };

// Height of each paramter knob on the paramEditor
static const int PARAM_HEIGHT { 100 };

// Width of Meter component
static constexpr int METER_WIDTH { 40 };

DelayEditor::DelayEditor(mrta::BaseProcessor& p) :
    juce::AudioProcessorEditor(p),
    processor { p },
    paramEditor(processor.getParameterManager(), PARAM_HEIGHT),
    meterComponent(static_cast<DelayProcessor&>(processor).getMeter())
{
    addAndMakeVisible(paramEditor);
    addAndMakeVisible(meterComponent);

    // Calculate window height based on number of parameters
    const auto height { processor.getParameterManager().getParameters().size() * PARAM_HEIGHT };
    setSize(WIDTH, height);
}

DelayEditor::~DelayEditor()
{
}

void DelayEditor::paint(juce::Graphics&)
{
}

void DelayEditor::resized()
{
    juce::Rectangle<int> area = getLocalBounds();
    meterComponent.setBounds(area.removeFromRight(METER_WIDTH));
    paramEditor.setBounds(area);
}
