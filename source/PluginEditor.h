#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "CustomLookAndFeel.h"

class PluginEditor : public juce::AudioProcessorEditor,
                     private juce::Timer
{
public:
    PluginEditor (PluginProcessor&);
    ~PluginEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    PluginProcessor& audioProcessor;
    CustomLookAndFeel customLookAndFeel;

    // --- Sliders (8 Knobs) ---
    juce::Slider inputDriveSlider;
    juce::Slider delayTimeSlider;
    juce::Slider feedbackSlider;
    juce::Slider mixSlider;
    juce::Slider hpfCutoffSlider;
    juce::Slider lpfCutoffSlider;
    juce::Slider rateModSlider;
    juce::Slider outputGainSlider;

    // --- Labels ---
    juce::Label inputDriveLabel { {}, "INPUT DRIVE" };
    juce::Label delayTimeLabel  { {}, "DELAY TIME" };
    juce::Label feedbackLabel   { {}, "FEEDBACK" };
    juce::Label mixLabel        { {}, "MIX" };
    juce::Label hpfCutoffLabel  { {}, "HPF" };
    juce::Label lpfCutoffLabel  { {}, "LPF" };
    juce::Label rateModLabel    { {}, "Rate\nMODULATION" };
    juce::Label outputGainLabel { {}, "OUTPUT GAIN" };

    // --- Botões do Painel Inferior ---
    juce::ToggleButton powerBypassButton { "POWER/BYPASS" };
    juce::ToggleButton pingPongButton    { "PING PONG" };
    juce::ToggleButton tempoSyncButton   { "TEMPO SYNC" };
    juce::ToggleButton saturationButton  { "SATURATION/TAPE" };

    // --- Attachments APVTS ---
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<SliderAttachment> inputDriveAttach;
    std::unique_ptr<SliderAttachment> delayTimeAttach;
    std::unique_ptr<SliderAttachment> feedbackAttach;
    std::unique_ptr<SliderAttachment> mixAttach;
    std::unique_ptr<SliderAttachment> hpfCutoffAttach;
    std::unique_ptr<SliderAttachment> lpfCutoffAttach;
    std::unique_ptr<SliderAttachment> rateModAttach;
    std::unique_ptr<SliderAttachment> outputGainAttach;

    std::unique_ptr<ButtonAttachment> powerBypassAttach;
    std::unique_ptr<ButtonAttachment> pingPongAttach;
    std::unique_ptr<ButtonAttachment> tempoSyncAttach;
    std::unique_ptr<ButtonAttachment> saturationAttach;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginEditor)
};
