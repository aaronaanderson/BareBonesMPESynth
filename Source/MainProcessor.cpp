#include "MainProcessor.h"
#include "MainEditor.h"

//==============================================================================
MainProcessor::MainProcessor()
     : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       instrument (juce::MPEZone (juce::MPEZone::Type::lower, 15)), 
       fmSynth (instrument)
{}
MainProcessor::~MainProcessor() {}
//==============================================================================
const juce::String MainProcessor::getName() const { return JucePlugin_Name; }
bool MainProcessor::acceptsMidi() const { return true; }
bool MainProcessor::producesMidi() const { return false; }
bool MainProcessor::isMidiEffect() const { return false; }
double MainProcessor::getTailLengthSeconds() const { return 0.0; }
int MainProcessor::getNumPrograms() { return 1; }
int MainProcessor::getCurrentProgram() { return 0; }
void MainProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String MainProcessor::getProgramName (int index) { juce::ignoreUnused (index); return {}; }
void MainProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused (index, newName); }
//==============================================================================
void MainProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);
    fmSynth.setCurrentPlaybackSampleRate (sampleRate);
}
void MainProcessor::releaseResources() {}
bool MainProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
void MainProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                  juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    for (auto midiEvent : midiMessages)
        instrument.processNextMidiEvent (midiEvent.getMessage()); 

    fmSynth.renderNextBlock (buffer, midiMessages, 0, buffer.getNumSamples());
}

//==============================================================================
bool MainProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* MainProcessor::createEditor() { return new MainEditor (*this); }
//==============================================================================
void MainProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void MainProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){ return new MainProcessor(); }