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

    // --- PARÂMETROS ALINHADOS COM O PLUGIN EDITOR ---
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "rateMod", 1 }, "Rate Modulation", 
        juce::NormalisableRange<float> (0.1f, 20.0f, 0.1f, 1.0f), 1.0f));

    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "outputGain", 1 }, "Output Gain", 
        juce::NormalisableRange<float> (-24.0f, 24.0f, 0.1f), 0.0f));
    // -----------------------------------------------

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
