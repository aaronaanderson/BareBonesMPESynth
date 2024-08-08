#pragma once

#include "MainProcessor.h"
#include <juce_audio_utils/juce_audio_utils.h>

class MainEditor final : public juce::AudioProcessorEditor
{
public:
    explicit MainEditor (MainProcessor&);
    ~MainEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
private:
    MainProcessor& processor;
    juce::MPEKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainEditor)
};
