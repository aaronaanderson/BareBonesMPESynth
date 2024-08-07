#pragma once

#include "MainProcessor.h"

//==============================================================================
class MainEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MainEditor (MainProcessor&);
    ~MainEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    MainProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainEditor)
};
