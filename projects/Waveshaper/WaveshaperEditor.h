#pragma once

#include <BaseProcessor.h>

#include <GenericParameterEditor.h>

/**
 * ================================================
 * 
 *          MOCKUP FOR DEVELOPING.
 *          MUST BE CHANGED FOR THE COPY
 *          WHENEVER POSSIBLE
 * 
 *  ================================================
 */

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

/**
 * ================================================
 * 
 *          MOCKUP FOR DEVELOPING.
 *          MUST BE CHANGED FOR THE COPY
 *          WHENEVER POSSIBLE
 * 
 *  ================================================
 */