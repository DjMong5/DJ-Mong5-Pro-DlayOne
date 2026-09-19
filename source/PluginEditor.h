#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class PluginEditor  : public juce::AudioProcessorEditor
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PluginProcessor& audioProcessor;

    // --- Sliders (Knobs) ---
    juce::Slider inputDriveSlider;
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider hpfCutoffSlider;
    juce::Slider lpfCutoffSlider;
    juce::Slider mixSlider;

    // --- Labels ---
    juce::Label inputDriveLabel { {}, "INPUT DRIVE" };
    juce::Label delayTimeLabel  { {}, "DELAY TIME" };
    juce::Label feedbackLabel   { {}, "FEEDBACK" };
    juce::Label hpfCutoffLabel  { {}, "HPF CUTOFF" };
    juce::Label lpfCutoffLabel  { {}, "LPF CUTOFF" };
    juce::Label mixLabel        { {}, "MIX" };

    // --- Botões do Painel Inferior ---
    juce::ToggleButton powerBypassButton  { "POWER/BYPASS" };
    juce::ToggleButton pingPongButton     { "PING PONG" };
    juce::ToggleButton tempoSyncButton    { "TEMPO SYNC" };
    juce::ToggleButton saturationButton   { "SATURATION/TAPE" };

    // --- Attachments APVTS ---
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> inputDriveAttach;
    std::unique_ptr<SliderAttachment> delayTimeAttach;
    std::unique_ptr<SliderAttachment> feedbackAttach;
    std::unique_ptr<SliderAttachment> hpfCutoffAttach;
    std::unique_ptr<SliderAttachment> lpfCutoffAttach;
    std::unique_ptr<SliderAttachment> mixAttach;

    std::unique_ptr<ButtonAttachment> powerBypassAttach;
    std::unique_ptr<ButtonAttachment> pingPongAttach;
    std::unique_ptr<ButtonAttachment> tempoSyncAttach;
    std::unique_ptr<ButtonAttachment> saturationAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
