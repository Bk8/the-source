/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Voice.h"
#include "HostParam.h"

// UI header, should be hidden behind a factory
#include <PluginEditor.h>

//==============================================================================
PluginAudioProcessor::PluginAudioProcessor()
    : delay(*this)
    , stepSeq(*this)
    , chorus(*this)
    , clip(*this)
{
    addParameter(new HostParam<Param>(osc1fine));
    addParameter(new HostParam<Param>(osc1coarse));
    addParameter(new HostParam<ParamStepped<eOscWaves>>(osc1Waveform));

    addParameter(new HostParam<ParamStepped<eLfoWaves>>(lfo1wave));
    addParameter(new HostParam<Param>(lfo1freq));
    addParameter(new HostParam<Param>(osc1lfo1depth));
    addParameter(new HostParam<ParamStepped<eOnOffToggle>>(lfo1TempSync));
    addParameter(new HostParam<Param>(noteLength));
    addParameter(new HostParam<Param>(lfoFadein));

    addParameter(new HostParam<Param>(vol));

    addParameter(new HostParam<Param>(osc1trngAmount));
    addParameter(new HostParam<Param>(osc1pulsewidth));

    addParameter(new HostParam<Param>(lpCutoff));
    addParameter(new HostParam<Param>(hpCutoff));
    addParameter(new HostParam<Param>(biquadResonance));
    addParameter(new HostParam<Param>(passtype));

    addParameter(new HostParam<Param>(envAttack));
    addParameter(new HostParam<Param>(envDecay));
    addParameter(new HostParam<Param>(envSustain));
    addParameter(new HostParam<Param>(envRelease));

    addParameter(new HostParam<Param>(panDir));
    addParameter(new HostParam<Param>(clippingFactor));

    addParameter(new HostParam<Param>(delayFeedback));
    addParameter(new HostParam<Param>(delayDryWet));
    addParameter(new HostParam<Param>(delayTime));

    positionInfo[0].resetToDefault();
    positionInfo[1].resetToDefault();
}

PluginAudioProcessor::~PluginAudioProcessor()
{
}

//==============================================================================
const String PluginAudioProcessor::getName() const
{
#ifdef JucePlugin_Name
    return JucePlugin_Name;
#else
    // standalone
    return "plugin";
#endif
}

const String PluginAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String PluginAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool PluginAudioProcessor::isInputChannelStereoPair (int index) const
{
    ignoreUnused(index);
    return true;
}

bool PluginAudioProcessor::isOutputChannelStereoPair (int index) const
{
    ignoreUnused(index);
    return true;
}

bool PluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PluginAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PluginAudioProcessor::setCurrentProgram (int index)
{
    ignoreUnused(index);
}

const String PluginAudioProcessor::getProgramName (int index)
{
    ignoreUnused(index);
    return String();
}

void PluginAudioProcessor::changeProgramName (int index, const String& newName)
{
    ignoreUnused(index,newName);
}

//==============================================================================
void PluginAudioProcessor::prepareToPlay (double sRate, int samplesPerBlock)
{
    ignoreUnused(samplesPerBlock);
    synth.setCurrentPlaybackSampleRate(sRate);
    synth.clearVoices();
    for (int i = 8; --i >= 0;)
    {
        synth.addVoice(new Voice(*this, samplesPerBlock));
    }
    synth.clearSounds();
    synth.addSound(new Sound());

    delay.init(getNumOutputChannels(), sRate);
    chorus.init(getNumOutputChannels(), sRate);
}

void PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void PluginAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    updateHostInfo();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // I've added this to avoid people getting screaming feedback
    // when they first compile the plugin, but obviously you don't need to
    // this code if your algorithm already fills all the output channels.
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    stepSeq.runSeq(midiMessages, buffer.getNumSamples(), getSampleRate());

    // pass these messages to the keyboard state so that it can update the component
    // to show on-screen which keys are being pressed on the physical midi keyboard.
    // This call will also add midi messages to the buffer which were generated by
    // the mouse-clicking on the on-screen keyboard.
    keyboardState.processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);

    // and now get the synth to process the midi events and generate its output.
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

    if (clippingFactor.get() > 0.f) {
        clip.clipSignal(buffer, 0, buffer.getNumSamples());
    }
    // fx
    // delay
    if (delayDryWet.get() > 0.f) {
        delay.render(buffer, 0, buffer.getNumSamples()); // adds the delay to the outputBuffer
    }
    // chorus
    if (chorDryWet.get() > 0.f) {
        chorus.render(buffer, 0); // adds the chorus to the outputBuffer
    }

    //midiMessages.clear(); // NOTE: for now so debugger does not complain
                          // should we set the JucePlugin_ProducesMidiOutput macro to 1 ?
}

void PluginAudioProcessor::updateHostInfo()
{
    // currentPositionInfo used for getting the bpm.
    if (AudioPlayHead* pHead = getPlayHead())
    {
        if (pHead->getCurrentPosition (positionInfo[getAudioIndex()])) {
            positionIndex.exchange(getGUIIndex());
            return;
        }
    }
    positionInfo[getAudioIndex()].resetToDefault();
}

//==============================================================================
bool PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PluginAudioProcessor::createEditor()
{
    return new PluginAudioProcessorEditor (*this);
}

//==============================================================================
void PluginAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    SynthParams::writeXMLPatchHost(destData, eSerializationParams::eAll);
}

void PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    SynthParams::readXMLPatchHost(data, sizeInBytes, eSerializationParams::eAll);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginAudioProcessor();
}
