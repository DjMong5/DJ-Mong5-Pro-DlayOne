#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Define a dimensão proporção estilo rack metálico (16:9 estendido)
    setSize (1000, 500);

    // Função lambda auxiliar para configurar Knobs
    auto setupKnob = [this](juce::Slider& slider, juce::Label& label)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 20);
        addAndMakeVisible (slider);

        label.setFont (juce::FontOptions (12.0f, juce::Font::bold));
        label.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (label);
    };

    setupKnob (inputDriveSlider, inputDriveLabel);
    setupKnob (delayTimeSlider, delayTimeLabel);
    setupKnob (feedbackSlider, feedbackLabel);
    setupKnob (hpfCutoffSlider, hpfCutoffLabel);
    setupKnob (lpfCutoffSlider, lpfCutoffLabel);
    setupKnob (mixSlider, mixLabel);

    // Configuração dos Botões
    auto setupButton = [this](juce::ToggleButton& button)
    {
        button.setClickingTogglesState (true);
        addAndMakeVisible (button);
    };

    setupButton (powerBypassButton);
    setupButton (pingPongButton);
    setupButton (tempoSyncButton);
    setupButton (saturationButton);

    // Conectar com APVTS
    auto& apvts = audioProcessor.getAPVTS();

    inputDriveAttach = std::make_unique<SliderAttachment> (apvts, "inputDrive", inputDriveSlider);
    delayTimeAttach  = std::make_unique<SliderAttachment> (apvts, "delayTime",  delayTimeSlider);
    feedbackAttach   = std::make_unique<SliderAttachment> (apvts, "feedback",   feedbackSlider);
    hpfCutoffAttach  = std::make_unique<SliderAttachment> (apvts, "hpfCutoff",  hpfCutoffSlider);
    lpfCutoffAttach  = std::make_unique<SliderAttachment> (apvts, "lpfCutoff",  lpfCutoffSlider);
    mixAttach        = std::make_unique<SliderAttachment> (apvts, "mix",        mixSlider);

    powerBypassAttach = std::make_unique<ButtonAttachment> (apvts, "powerBypass",     powerBypassButton);
    pingPongAttach    = std::make_unique<ButtonAttachment> (apvts, "pingPong",        pingPongButton);
    tempoSyncAttach   = std::make_unique<ButtonAttachment> (apvts, "tempoSync",       tempoSyncButton);
    saturationAttach  = std::make_unique<ButtonAttachment> (apvts, "saturationTape", saturationButton);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // Fundo metálico escuro do rack
    g.fillAll (juce::Colour (0xff1b1d20));

    // Moldura externa
    g.setColour (juce::Colour (0xff3a3d42));
    g.drawRect (getLocalBounds(), 4);

    // Título Principal "DJ MONG5 PRO DLAYONE"
    g.setColour (juce::Colours::whitesmoke);
    g.setFont (juce::FontOptions (28.0f, juce::Font::bold));
    g.drawText ("DJ MONG5 PRO DLAYONE", getLocalBounds().removeFromTop (60), juce::Justification::centred, true);

    // Visor Central LCD (Ecrã de informações do Delay)
    auto displayArea = juce::Rectangle<int> (280, 70, 440, 150);
    g.setColour (juce::Colours::black);
    g.fillRect (displayArea);
    g.setColour (juce::Colour (0xff4a4e54));
    g.drawRect (displayArea, 2);

    // Textos informativos no ecrã visor (BPM, TIME, SYNC)
    g.setColour (juce::Colours::cyan);
    g.setFont (juce::FontOptions (14.0f));
    g.drawText ("BPM: 128", displayArea.reduced (10), juce::Justification::topRight, true);
    g.drawText ("TIME: 500ms", displayArea.reduced (10), juce::Justification::bottomLeft, true);
    g.drawText ("SYNC: 1/4", displayArea.reduced (10), juce::Justification::bottomRight, true);
}

void PluginEditor::resized()
{
    // Disposição dos Knobs
    int knobWidth = 90;
    int knobHeight = 90;
    int labelHeight = 20;
    int startY = 240;

    // Fileira Superior de Controles
    inputDriveSlider.setBounds (60,  startY, knobWidth, knobHeight);
    inputDriveLabel.setBounds  (60,  startY + knobHeight, knobWidth, labelHeight);

    delayTimeSlider.setBounds  (190, startY, knobWidth, knobHeight);
    delayTimeLabel.setBounds   (190, startY + knobHeight, knobWidth, labelHeight);

    feedbackSlider.setBounds   (320, startY, knobWidth, knobHeight);
    feedbackLabel.setBounds    (320, startY + knobHeight, knobWidth, labelHeight);

    hpfCutoffSlider.setBounds  (590, startY, knobWidth, knobHeight);
    hpfCutoffLabel.setBounds   (590, startY + knobHeight, knobWidth, labelHeight);

    lpfCutoffSlider.setBounds  (720, startY, knobWidth, knobHeight);
    lpfCutoffLabel.setBounds   (720, startY + knobHeight, knobWidth, labelHeight);

    mixSlider.setBounds        (850, startY, knobWidth, knobHeight);
    mixLabel.setBounds         (850, startY + knobHeight, knobWidth, labelHeight);

    // Fileira Inferior de Botões Alternadores (Switches)
    int btnWidth = 140;
    int btnHeight = 40;
    int btnY = 420;

    powerBypassButton.setBounds (100, btnY, btnWidth, btnHeight);
    pingPongButton.setBounds    (310, btnY, btnWidth, btnHeight);
    tempoSyncButton.setBounds   (520, btnY, btnWidth, btnHeight);
    saturationButton.setBounds  (730, btnY, btnWidth, btnHeight);
}
