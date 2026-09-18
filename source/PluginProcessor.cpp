#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
     : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
{
}

PluginProcessor::~PluginProcessor() {}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Controles Contínuos (Knobs)
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "INPUT_DRIVE", 1 }, "Input Drive", 
        juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f), 0.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "DELAY_TIME", 1 }, "Delay Time", 
        juce::NormalisableRange<float> (1.0f, 2000.0f, 1.0f), 500.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "FEEDBACK", 1 }, "Feedback", 
        juce::NormalisableRange<float> (0.0f, 0.95f, 0.01f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "MIX", 1 }, "Mix", 
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.5f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "HPF", 1 }, "HPF Cutoff", 
        juce::NormalisableRange<float> (20.0f, 2000.0f, 1.0f, 0.4f), 20.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "LPF", 1 }, "LPF Cutoff", 
        juce::NormalisableRange<float> (500.0f, 20000.0f, 1.0f, 0.4f), 20000.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "RATE_MOD", 1 }, "Rate Modulation", 
        juce::NormalisableRange<float> (0.1f, 10.0f, 0.1f), 1.0f));

    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "OUTPUT_GAIN", 1 }, "Output Gain", 
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f));

    // Botões de Estado (Switches)
    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "POWER_BYPASS", 1 }, "Power / Bypass", true));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "PING_PONG", 1 }, "Ping Pong Mode", false));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "TEMPO_SYNC", 1 }, "Tempo Sync", false));

    layout.add (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "SATURATION_TAPE", 1 }, "Saturation / Tape", false));

    return layout;
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32> (samplesPerBlock);
    spec.numChannels = 1;

    delayLineLeft.prepare (spec);
    delayLineRight.prepare (spec);
    delayLineLeft.reset();
    delayLineRight.reset();

    hpfLeft.prepare (spec);
    hpfRight.prepare (spec);
    lpfLeft.prepare (spec);
    lpfRight.prepare (spec);

    hpfLeft.setType (juce::dsp::StateVariableFilter::Parameters::Type::highPass);
    hpfRight.setType (juce::dsp::StateVariableFilter::Parameters::Type::highPass);
    lpfLeft.setType (juce::dsp::StateVariableFilter::Parameters::Type::lowPass);
    lpfRight.setType (juce::dsp::StateVariableFilter::Parameters::Type::lowPass);

    lastFeedbackLeft = 0.0f;
    lastFeedbackRight = 0.0f;
}

void PluginProcessor::releaseResources() {}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // Leitura dos Parâmetros
    bool powerOn = apvts.getRawParameterValue ("POWER_BYPASS")->load() > 0.5f;
    if (!powerOn) return;

    float inputDriveDb = apvts.getRawParameterValue ("INPUT_DRIVE")->load();
    float delayTimeMs  = apvts.getRawParameterValue ("DELAY_TIME")->load();
    float feedbackPct  = apvts.getRawParameterValue ("FEEDBACK")->load();
    float mixPct       = apvts.getRawParameterValue ("MIX")->load();
    float hpfCutoff    = apvts.getRawParameterValue ("HPF")->load();
    float lpfCutoff    = apvts.getRawParameterValue ("LPF")->load();
    float outputGainDb = apvts.getRawParameterValue ("OUTPUT_GAIN")->load();
    bool isPingPong    = apvts.getRawParameterValue ("PING_PONG")->load() > 0.5f;
    bool isSaturation  = apvts.getRawParameterValue ("SATURATION_TAPE")->load() > 0.5f;

    float inputGain  = juce::Decibels::decibelsToGain (inputDriveDb);
    float outputGain = juce::Decibels::decibelsToGain (outputGainDb);

    float delayInSamples = (delayTimeMs / 1000.0f) * static_cast<float> (getSampleRate());
    delayLineLeft.setDelay (delayInSamples);
    delayLineRight.setDelay (delayInSamples);

    hpfLeft.setCutoffFrequency (hpfCutoff);
    hpfRight.setCutoffFrequency (hpfCutoff);
    lpfLeft.setCutoffFrequency (lpfCutoff);
    lpfRight.setCutoffFrequency (lpfCutoff);

    auto* leftChannel  = buffer.getWritePointer (0);
    auto* rightChannel = (buffer.getNumChannels() > 1) ? buffer.getWritePointer (1) : leftChannel;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float cleanLeft  = leftChannel[sample] * inputGain;
        float cleanRight = rightChannel[sample] * inputGain;

        float delayedLeft  = delayLineLeft.popSample (0);
        float delayedRight = delayLineRight.popSample (0);

        float feedbackLeftSample  = hpfLeft.processSample (0, delayedLeft);
        feedbackLeftSample        = lpfLeft.processSample (0, feedbackLeftSample);

        float feedbackRightSample = hpfRight.processSample (0, delayedRight);
        feedbackRightSample       = lpfRight.processSample (0, feedbackRightSample);

        if (isSaturation)
        {
            feedbackLeftSample  = std::tanh (feedbackLeftSample * 1.5f);
            feedbackRightSample = std::tanh (feedbackRightSample * 1.5f);
        }

        if (isPingPong)
        {
            delayLineLeft.pushSample (0, cleanLeft + (feedbackRightSample * feedbackPct));
            delayLineRight.pushSample (0, cleanRight + (feedbackLeftSample * feedbackPct));
        }
        else
        {
            delayLineLeft.pushSample (0, cleanLeft + (feedbackLeftSample * feedbackPct));
            delayLineRight.pushSample (0, cleanRight + (feedbackRightSample * feedbackPct));
        }

        leftChannel[sample]  = ((cleanLeft * (1.0f - mixPct)) + (delayedLeft * mixPct)) * outputGain;
        rightChannel[sample] = ((cleanRight * (1.0f - mixPct)) + (delayedRight * mixPct)) * outputGain;
    }
}

bool PluginProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* PluginProcessor::createEditor() { return new PluginEditor (*this); }

void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
