#pragma once

#include <BaseProcessor.h>

#include "GenericParameterEditor.h"

#include <MeterComponent.h>

class DelayEditor final : public juce::AudioProcessorEditor
{
public:
    DelayEditor(mrta::BaseProcessor&);
    ~DelayEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;

    mrta::GenericParameterEditor paramEditor;
    GUI::MeterComponent meterComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayEditor)
};
