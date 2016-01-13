/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.2.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright (c) 2015 - ROLI Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_9F090381585CF062__
#define __JUCE_HEADER_9F090381585CF062__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "PanelBase.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class LfoPanel  : public PanelBase,
                  public SliderListener,
                  public ButtonListener
{
public:
    //==============================================================================
    LfoPanel (SynthParams &p);
    ~LfoPanel();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void sliderValueChanged (Slider* sliderThatWasMoved);
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<MouseOverKnob> freq;
    ScopedPointer<MouseOverKnob> wave;
    ScopedPointer<ToggleButton> tempoSyncSwitch;
    ScopedPointer<Slider> notelength;
    ScopedPointer<Label> label4;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LfoPanel)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_9F090381585CF062__
