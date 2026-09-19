#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginProcessor::PluginProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

PluginProcessor::~PluginProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "inputDrive", 1 }, "Input Drive", 
        juce::NormalisableRange<float> (0.0f, 24.0f, 0.1f), 0.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "delayTime", 1 }, "Delay Time", 
        juce::NormalisableRange<float> (0.0f, 2000.0f, 1.0f), 500.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "feedback", 1 }, "Feedback", 
        juce::NormalisableRange<float> (0.0f, 100.0f, 0.1f), 30.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "hpfCutoff", 1 }, "HPF Cutoff", 
        juce::NormalisableRange<float> (20.0f, 2000.0f, 1.0f, 0.4f), 20.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "lpfCutoff", 1 }, "LPF Cutoff", 
        juce::NormalisableRange<float> (500.0f, 20000.0f, 1.0f, 0.4f), 20000.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "mix", 1 }, "Mix", 
        juce::NormalisableRange<float> (0.0f, 100.0f, 1.0f), 50.0f));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "powerBypass", 1 }, "Power/Bypass", true));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "pingPong", 1 }, "Ping Pong", false));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "tempoSync", 1 }, "Tempo Sync", false));

    params.push_back (std::make_unique<juce::AudioParameterBool> (
        juce::ParameterID { "saturationTape", 1 }, "Saturation/Tape", false));

    return { params.begin(), params.end() };
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

    hpfLeft.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    hpfRight.setType (juce::dsp::StateVariableTPTFilterType::highpass);
    lpfLeft.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
    lpfRight.setType (juce::dsp::StateVariableTPTFilterType::lowpass);

    hpfLeft.reset();
    hpfRight.reset();
    lpfLeft.reset();
    lpfRight.reset();

    // Reset de segurança do feedback
    feedbackLeftSample = 0.0f;
    feedbackRightSample = 0.0f;
}

void PluginProcessor::releaseResources()
{
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

    return true;
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // --- POWER / BYPASS ---
    bool isPowered = apvts.getRawParameterValue ("powerBypass")->load() > 0.5f;
    if (!isPowered)
        return; 

    // --- CARREGAMENTO DOS PARÂMETROS ---
    float inputDriveDb = apvts.getRawParameterValue ("inputDrive")->load();
    float delayTimeMs  = apvts.getRawParameterValue ("delayTime")->load();
    float feedbackPct  = apvts.getRawParameterValue ("feedback")->load() / 100.0f;
    float hpfFreq      = apvts.getRawParameterValue ("hpfCutoff")->load();
    float lpfFreq      = apvts.getRawParameterValue ("lpfCutoff")->load();
    float mixPct       = apvts.getRawParameterValue ("mix")->load() / 100.0f;

    bool isPingPong   = apvts.getRawParameterValue ("pingPong")->load() > 0.5f;
    bool isTempoSync  = apvts.getRawParameterValue ("tempoSync")->load() > 0.5f;
    bool isSatTape    = apvts.getRawParameterValue ("saturationTape")->load() > 0.5f;

    // --- SYNC MANAGER (MS / BPM / TEMPO SYNC) ---
    if (isTempoSync)
    {
        if (auto* playHead = getPlayHead())
        {
            if (auto position = playHead->getPosition())
            {
                if (auto bpm = position->getBpm())
                {
                    // Sincronia de tempo padrão em semínima (1/4 Note)
                    double quarterNoteMs = (60.0 / *bpm) * 1000.0;
                    delayTimeMs = static_cast<float>(quarterNoteMs);
                }
            }
        }
    }

    // Atualização dos parâmetros do DSP
    float delaySamples = (delayTimeMs / 1000.0f) * static_cast<float>(getSampleRate());
    delayLineLeft.setDelay (delaySamples);
    delayLineRight.setDelay (delaySamples);

    hpfLeft.setCutoffFrequency (hpfFreq);
    hpfRight.setCutoffFrequency (hpfFreq);
    lpfLeft.setCutoffFrequency (lpfFreq);
    lpfRight.setCutoffFrequency (lpfFreq);

    float driveGain = juce::Decibels::decibelsToGain (inputDriveDb);

    auto* leftChannel  = buffer.getWritePointer (0);
    auto* rightChannel = (totalNumInputChannels > 1) ? buffer.getWritePointer (1) : leftChannel;

    // Loop de Amostras (DSP Pipeline)
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // 1. InputGainBlock (INPUT DRIVE)
        float inL = leftChannel[sample] * driveGain;
        float inR = rightChannel[sample] * driveGain;

        // 2. Delay Line Execution
        delayLineLeft.pushSample (0, inL + feedbackLeftSample);
        delayLineRight.pushSample (0, inR + feedbackRightSample);

        float delayedL = delayLineLeft.popSample (0);
        float delayedR = delayLineRight.popSample (0);

        // 3. Feedback Processing & PingPong Logic
        float fbL = delayedL * feedbackPct;
        float fbR = delayedR * feedbackPct;

        if (isPingPong)
            std::swap (fbL, fbR); // Troca os canais L/R para o efeito de Ping-Pong

        // 4. SaturationTapeFX
        if (isSatTape)
        {
            fbL = std::tanh (fbL * 1.5f);
            fbR = std::tanh (fbR * 1.5f);
        }

        // 5. HPF & LPF Filters (Filtros no Feedback Loop)
        fbL = lpfLeft.processSample (0, hpfLeft.processSample (0, fbL));
        fbR = lpfRight.processSample (0, hpfRight.processSample (0, fbR));

        // Atualização da memória do Feedback
        feedbackLeftSample  = fbL;
        feedbackRightSample = fbR;

        // 6. MixStage (Dry / Wet Output)
        leftChannel[sample]  = inL * (1.0f - mixPct) + delayedL * mixPct;
        if (totalNumInputChannels > 1)
            rightChannel[sample] = inR * (1.0f - mixPct) + delayedR * mixPct;
    }
}

bool PluginProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* PluginProcessor::createEditor() { return new juce::GenericAudioProcessorEditor (*this); }

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
