#include "MainProcessor.h"
#include "MainEditor.h"

MainEditor::MainEditor (MainProcessor& p)
    : AudioProcessorEditor (&p), processor (p), 
      keyboardComponent (processor.getInstrument(), juce::MPEKeyboardComponent::Orientation::horizontalKeyboard)
{

    addAndMakeVisible (keyboardComponent);

    setResizable (true, false);
    setSize (800, 300);
}
MainEditor::~MainEditor() {}
void MainEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    
}
void MainEditor::resized()
{
    auto b = getLocalBounds();
    keyboardComponent.setBounds (b);
}
