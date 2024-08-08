#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class FMSynthVoice : public juce::MPESynthesiserVoice
{
    void setCurrentSampleRate (double newRate) override 
    {
        if (!juce::approximatelyEqual (currentSampleRate, newRate))
        {
            noteStopped (false);
            currentSampleRate = newRate;
            carrierFrequency.reset (newRate, 0.01);
            amplitude.reset (newRate, 0.04);
            modulatorFrequency.reset (newRate, 0.01);
            modulatorDepth.reset (newRate, 0.04);
        }
    }
    void noteStarted() override
    {
        auto note = getCurrentlyPlayingNote();
        carrierFrequency.setTargetValue (static_cast<float> (note.getFrequencyInHertz()));
        modulatorFrequency.setTargetValue (static_cast<float> (note.getFrequencyInHertz()));
        modulatorDepth.setTargetValue (static_cast<float> (note.timbre.asUnsignedFloat() * note.getFrequencyInHertz() * 14.0f));
        amplitude.setTargetValue (note.pressure.asUnsignedFloat());
        carrierPhase = 0.0;
        readyForRemoval = false;
    }
    void noteStopped (bool allowTailOff) override
    {
        if (!allowTailOff) 
        {
            clearCurrentNote();
            return;
        }
        amplitude.setTargetValue (0.0f);
        readyForRemoval = true;
    }
    void notePressureChanged() override
    {
        auto note = getCurrentlyPlayingNote();
        amplitude.setTargetValue (note.pressure.asUnsignedFloat());
    }
    void notePitchbendChanged() override
    {
        auto note = getCurrentlyPlayingNote();
        carrierFrequency.setTargetValue (static_cast<float> (note.getFrequencyInHertz()));
        modulatorFrequency.setTargetValue (static_cast<float> (note.getFrequencyInHertz()));
    }
    void noteTimbreChanged() override 
    {
        auto note = getCurrentlyPlayingNote();
        modulatorDepth.setTargetValue (static_cast<float> (note.timbre.asUnsignedFloat() * note.getFrequencyInHertz() * 14.0f));
    }
    void noteKeyStateChanged() override {}
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer,
                          int startSample,
                          int numSamples) override
    {
        juce::ignoreUnused (outputBuffer, startSample, numSamples);
        auto* b = outputBuffer.getWritePointer (0);
        for (int i = startSample; i < startSample + numSamples; i++)
        {
            setModulatorFrequency (modulatorFrequency.getNextValue());
            setCarrierFrequency (carrierFrequency.getNextValue() + 
                                (static_cast<float> (std::sin (modulatorPhase)) * modulatorDepth.getNextValue()));
            b[i] += static_cast<float> (std::sin (carrierPhase)) * amplitude.getNextValue(); 
            
            carrierPhase = std::fmod (carrierPhase + carrierPhaseIncrement, juce::MathConstants<double>::twoPi);
            modulatorPhase = std::fmod (modulatorPhase + modulatorPhaseIncrement, juce::MathConstants<double>::twoPi);
            if (readyForRemoval)
                if (juce::approximatelyEqual (amplitude.getCurrentValue(), 0.0f)) 
                    clearCurrentNote();
        }
    }
private:
    double carrierPhase, carrierPhaseIncrement;
    juce::SmoothedValue<float> carrierFrequency;
    double modulatorPhase, modulatorPhaseIncrement;
    juce::SmoothedValue<float> modulatorFrequency;
    juce::SmoothedValue<float> modulatorDepth;
    juce::SmoothedValue<float> amplitude;

    bool readyForRemoval = false;

    void setCarrierFrequency (float newFrequency)
    {
        carrierPhaseIncrement = (newFrequency * juce::MathConstants<double>::twoPi) / currentSampleRate;
    }
    void setModulatorFrequency (float newFrequency)
    {
        modulatorPhaseIncrement = (newFrequency * juce::MathConstants<double>::twoPi) / currentSampleRate;
    }
};

class FMSynth : public juce::MPESynthesiser
{
public:
    FMSynth (juce::MPEInstrument& instrument)
      : juce::MPESynthesiser (instrument)
    {
        for (int i = 0; i < 15; i++)
            addVoice (new FMSynthVoice());
    }
};

//==============================================================================
class MainProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    MainProcessor();
    ~MainProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    juce::MPEInstrument& getInstrument() { return instrument; }
private:
    //==============================================================================
    juce::MPEInstrument instrument;
    FMSynth fmSynth;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainProcessor)
};
