#pragma once

#include <BaseProcessor.h>

#include <GenericParameterEditor.h>

class WaveshaperEditor final : public juce::AudioProcessorEditor
{
public:
    WaveshaperEditor(mrta::BaseProcessor&);
    ~WaveshaperEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    mrta::BaseProcessor& processor;

    mrta::GenericParameterEditor paramEditor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveshaperEditor)
};
