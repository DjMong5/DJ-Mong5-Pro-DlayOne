#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

    // Função auxiliar para configurar Knobs
    auto setupKnob = [this](juce::Slider& slider, juce::Label& label, const juce::String& name)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
        addAndMakeVisible(slider);

        label.setText(name, juce::dontSendNotification);
        label.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(label);
    };

    // Função auxiliar para configurar Botões
    auto setupButton = [this](juce::ToggleButton& button)
    {
        button.setClickingTogglesState(true);
        addAndMakeVisible(button);
    };

    // 1. Configuração dos Knobs
    setupKnob(inputDriveSlider, inputDriveLabel, "Input Drive");
    setupKnob(delayTimeSlider, delayTimeLabel, "Delay Time");
    setupKnob(feedbackSlider, feedbackLabel, "Feedback");
    setupKnob(mixSlider, mixLabel, "Mix");
    setupKnob(hpfSlider, hpfLabel, "HPF");
    setupKnob(lpfSlider, lpfLabel, "LPF");
    setupKnob(rateModSlider, rateModLabel, "Rate Mod");
    setupKnob(outputGainSlider, outputGainLabel, "Output Gain");

    // 2. Configuração dos Botões
    setupButton(powerBypassButton);
    setupButton(pingPongButton);
    setupButton(tempoSyncButton);
    setupButton(saturationButton);

    // 3. Vínculos com o APVTS
    auto& apvts = processorRef.getAPVTS();

    inputDriveAttachment = std::make_unique<SliderAttachment>(apvts, "INPUT_DRIVE", inputDriveSlider);
    delayTimeAttachment  = std::make_unique<SliderAttachment>(apvts, "DELAY_TIME", delayTimeSlider);
    feedbackAttachment   = std::make_unique<SliderAttachment>(apvts, "FEEDBACK", feedbackSlider);
    mixAttachment        = std::make_unique<SliderAttachment>(apvts, "MIX", mixSlider);
    hpfAttachment        = std::make_unique<SliderAttachment>(apvts, "HPF", hpfSlider);
    lpfAttachment        = std::make_unique<SliderAttachment>(apvts, "LPF", lpfSlider);
    rateModAttachment    = std::make_unique<SliderAttachment>(apvts, "RATE_MOD", rateModSlider);
    outputGainAttachment = std::make_unique<SliderAttachment>(apvts, "OUTPUT_GAIN", outputGainSlider);

    powerBypassAttachment = std::make_unique<ButtonAttachment>(apvts, "POWER_BYPASS", powerBypassButton);
    pingPongAttachment    = std::make_unique<ButtonAttachment>(apvts, "PING_PONG", pingPongButton);
    tempoSyncAttachment   = std::make_unique<ButtonAttachment>(apvts, "TEMPO_SYNC", tempoSyncButton);
    saturationAttachment  = std::make_unique<ButtonAttachment>(apvts, "SATURATION_TAPE", saturationButton);

    // Melatonin Inspector
    addAndMakeVisible (inspectButton);
    inspectButton.onClick = [&] {
        if (!inspector)
        {
            inspector = std::make_unique<melatonin::Inspector> (*this);
            inspector->onClose = [this]() { inspector.reset(); };
        }
        inspector->setVisible (true);
    };

    setSize (800, 420);
}

PluginEditor::~PluginEditor() {}

void PluginEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff2b2d31));

    g.setColour (juce::Colours::white);
    g.setFont (22.0f);
    g.drawText ("DJ MONG5 PRO DLAYONE", getLocalBounds().removeFromTop(45), juce::Justification::centred, false);
}

void PluginEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(45);

    auto bottomArea = area.removeFromBottom(40);
    inspectButton.setBounds (bottomArea.withSizeKeepingCentre(120, 28));

    // Linha inferior de botões
    auto buttonArea = area.removeFromBottom(50);
    int buttonWidth = buttonArea.getWidth() / 4;

    powerBypassButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(10, 5));
    pingPongButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(10, 5));
    tempoSyncButton.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(10, 5));
    saturationButton.setBounds(buttonArea.reduced(10, 5));

    // Espaço central do visor gráfico
    auto displayArea = area.removeFromTop(70).reduced(150, 5);

    // Grid dos Knobs (2x4)
    int colWidth = area.getWidth() / 4;
    int rowHeight = area.getHeight() / 2;

    auto row1 = area.removeFromTop(rowHeight);
    auto row2 = area;

    auto placeKnob = [](juce::Rectangle<int> bounds, juce::Label& label, juce::Slider& slider) {
        label.setBounds(bounds.removeFromTop(18));
        slider.setBounds(bounds);
    };

    // Linha 1
    placeKnob(row1.removeFromLeft(colWidth).reduced(5), inputDriveLabel, inputDriveSlider);
    placeKnob(row1.removeFromLeft(colWidth).reduced(5), delayTimeLabel, delayTimeSlider);
    placeKnob(row1.removeFromLeft(colWidth).reduced(5), feedbackLabel, feedbackSlider);
    placeKnob(row1.reduced(5), mixLabel, mixSlider);

    // Linha 2
    placeKnob(row2.removeFromLeft(colWidth).reduced(5), hpfLabel, hpfSlider);
    placeKnob(row2.removeFromLeft(colWidth).reduced(5), lpfLabel, lpfSlider);
    placeKnob(row2.removeFromLeft(colWidth).reduced(5), rateModLabel, rateModSlider);
    placeKnob(row2.reduced(5), outputGainLabel, outputGainSlider);
}
