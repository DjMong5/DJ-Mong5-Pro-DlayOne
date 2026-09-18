#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "BinaryData.h"
#include "melatonin_inspector/melatonin_inspector.h"

class PluginEditor : public juce::AudioProcessorEditor
{
public:
    explicit PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& processorRef;

    // --- Sliders (Knobs) ---
    juce::Slider inputDriveSlider;
    juce::Label  inputDriveLabel;

    juce::Slider delayTimeSlider;
    juce::Label  delayTimeLabel;

    juce::Slider feedbackSlider;
    juce::Label  feedbackLabel;

    juce::Slider mixSlider;
    juce::Label  mixLabel;

    juce::Slider hpfSlider;
    juce::Label  hpfLabel;

    juce::Slider lpfSlider;
    juce::Label  lpfLabel;

    juce::Slider rateModSlider;
    juce::Label  rateModLabel;

    juce::Slider outputGainSlider;
    juce::Label  outputGainLabel;

    // --- Botões (Switches) ---
    juce::ToggleButton powerBypassButton { "Power / Bypass" };
    juce::ToggleButton pingPongButton    { "Ping Pong" };
    juce::ToggleButton tempoSyncButton   { "Tempo Sync" };
    juce::ToggleButton saturationButton  { "Saturation / Tape" };

    // --- Attachments do APVTS ---
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> inputDriveAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> hpfAttachment;
    std::unique_ptr<SliderAttachment> lpfAttachment;
    std::unique_ptr<SliderAttachment> rateModAttachment;
    std::unique_ptr<SliderAttachment> outputGainAttachment;

    std::unique_ptr<ButtonAttachment> powerBypassAttachment;
    std::unique_ptr<ButtonAttachment> pingPongAttachment;
    std::unique_ptr<ButtonAttachment> tempoSyncAttachment;
    std::unique_ptr<ButtonAttachment> saturationAttachment;

    // Ferramenta Melatonin Inspector
    std::unique_ptr<melatonin::Inspector> inspector;
    juce::TextButton inspectButton { "Inspect the UI" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
