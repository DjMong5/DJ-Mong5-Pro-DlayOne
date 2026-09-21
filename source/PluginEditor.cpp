#include "PluginProcessor.h"
#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Ativa o estilo de rack metálico personalizado
    setLookAndFeel (&customLookAndFeel);
    setSize (1000, 500);

    // Função lambda para configurar os Knobs
    auto setupKnob = [this](juce::Slider& slider, juce::Label& label)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible (slider);

        label.setFont (juce::Font (11.0f, juce::Font::bold));
        label.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (label);
    };

    // Fileira Superior (4 Knobs)
    setupKnob (inputDriveSlider, inputDriveLabel);
    setupKnob (delayTimeSlider, delayTimeLabel);
    setupKnob (feedbackSlider, feedbackLabel);
    setupKnob (mixSlider, mixLabel);

    // Fileira Inferior (4 Knobs)
    setupKnob (hpfCutoffSlider, hpfCutoffLabel);
    setupKnob (lpfCutoffSlider, lpfCutoffLabel);
    setupKnob (rateModSlider, rateModLabel);
    setupKnob (outputGainSlider, outputGainLabel);

    // Configuração dos Botões Iluminados
    auto setupButton = [this](juce::ToggleButton& button)
    {
        button.setClickingTogglesState (true);
        addAndMakeVisible (button);
    };

    setupButton (powerBypassButton);
    setupButton (pingPongButton);
    setupButton (tempoSyncButton);
    setupButton (saturationButton);

    // Conectar com a APVTS
    auto& apvts = audioProcessor.getAPVTS();

    inputDriveAttach = std::make_unique<SliderAttachment> (apvts, "inputDrive", inputDriveSlider);
    delayTimeAttach  = std::make_unique<SliderAttachment> (apvts, "delayTime",  delayTimeSlider);
    feedbackAttach   = std::make_unique<SliderAttachment> (apvts, "feedback",   feedbackSlider);
    mixAttach        = std::make_unique<SliderAttachment> (apvts, "mix",        mixSlider);

    hpfCutoffAttach  = std::make_unique<SliderAttachment> (apvts, "hpfCutoff",  hpfCutoffSlider);
    lpfCutoffAttach  = std::make_unique<SliderAttachment> (apvts, "lpfCutoff",  lpfCutoffSlider);
    rateModAttach    = std::make_unique<SliderAttachment> (apvts, "rateMod",    rateModSlider);
    outputGainAttach = std::make_unique<SliderAttachment> (apvts, "outputGain", outputGainSlider);

    // Inicia a atualização contínua da tela LCD a 30 Hz
    startTimerHz (30);
}

PluginEditor::~PluginEditor()
{
    stopTimer();
    setLookAndFeel (nullptr); // Limpeza de ponteiro em segurança
}

void PluginEditor::timerCallback()
{
    repaint (310, 80, 380, 120); // Redesenha a área central LCD
}

void PluginEditor::paint (juce::Graphics& g)
{
    // 1. Carrega e desenha a imagem de fundo (Dragões e chassi metálico escovado)
    auto bgImage = juce::ImageCache::getFromMemory (PluginBinaryData::background_png, PluginBinaryData::background_pngSize);

    if (bgImage.isValid())
    {
        g.drawImage (bgImage, getLocalBounds().toFloat());
    }
    else
    {
        // Fundo alternativo em caso de falha no carregamento
        g.fillAll (juce::Colour (0xff2b2d31));
        g.setColour (juce::Colour (0xff1a1b1e));
        g.drawRect (getLocalBounds(), 4);
    }

    // 2. Visor Central LCD (Ecrã de informações)
    auto displayArea = juce::Rectangle<int> (310, 80, 380, 120);
    g.setColour (juce::Colours::black);
    g.fillRect (displayArea);
    g.setColour (juce::Colour (0xff4a4e54));
    g.drawRect (displayArea, 2);

    // 3. Leituras dinâmicas do Visor
    float delayTimeMs = delayTimeSlider.getValue();
    bool isSynced = tempoSyncButton.getToggleState();

    g.setColour (juce::Colours::cyan);
    g.setFont (juce::Font (13.0f, juce::Font::plain));

    juce::String timeText = isSynced ? "SYNC: 1/4" : "TIME: " + juce::String (delayTimeMs, 1) + "ms";
    g.drawText (timeText, displayArea.reduced (12), juce::Justification::bottomLeft, true);
    g.drawText ("BPM: 128", displayArea.reduced (12), juce::Justification::topRight, true);
}

void PluginEditor::resized()
{
    int knobSize = 75;
    int labelHeight = 28;
    int startY1 = 230; // Fileira 1
    int startY2 = 340; // Fileira 2

    int col1 = 230, col2 = 380, col3 = 545, col4 = 695;

    // Fileira Superior (Input Drive, Delay Time, Feedback, Mix)
    inputDriveSlider.setBounds (col1, startY1, knobSize, knobSize);
    inputDriveLabel.setBounds  (col1 - 10, startY1 + knobSize, knobSize + 20, labelHeight);

    delayTimeSlider.setBounds  (col2, startY1, knobSize, knobSize);
    delayTimeLabel.setBounds   (col2 - 10, startY1 + knobSize, knobSize + 20, labelHeight);

    feedbackSlider.setBounds   (col3, startY1, knobSize, knobSize);
    feedbackLabel.setBounds    (col3 - 10, startY1 + knobSize, knobSize + 20, labelHeight);

    mixSlider.setBounds        (col4, startY1, knobSize, knobSize);
    mixLabel.setBounds         (col4 - 10, startY1 + knobSize, knobSize + 20, labelHeight);

    // Fileira Inferior (HPF, LPF, Rate Modulation, Output Gain)
    hpfCutoffSlider.setBounds  (col1, startY2, knobSize, knobSize);
    hpfCutoffLabel.setBounds   (col1 - 10, startY2 + knobSize, knobSize + 20, labelHeight);

    lpfCutoffSlider.setBounds  (col2, startY2, knobSize, knobSize);
    lpfCutoffLabel.setBounds   (col2 - 10, startY2 + knobSize, knobSize + 20, labelHeight);

    rateModSlider.setBounds    (col3, startY2, knobSize, knobSize);
    rateModLabel.setBounds     (col3 - 15, startY2 + knobSize, knobSize + 30, labelHeight);

    outputGainSlider.setBounds (col4, startY2, knobSize, knobSize);
    outputGainLabel.setBounds  (col4 - 10, startY2 + knobSize, knobSize + 20, labelHeight);

    // Botões Iluminados do Painel Inferior
    int btnWidth = 110;
    int btnHeight = 35;
    int btnY = 445;

    powerBypassButton.setBounds (210, btnY, btnWidth, btnHeight);
    pingPongButton.setBounds    (365, btnY, btnWidth, btnHeight);
    tempoSyncButton.setBounds   (525, btnY, btnWidth, btnHeight);
    saturationButton.setBounds  (680, btnY, btnWidth, btnHeight);
}
