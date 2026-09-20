#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff00d2ff));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff15181c));
    }

    // --- DESENHO DOS KNOBS (SLIDERS ROTATIVOS) ---
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width, height) / 2.0f - 6.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // 1. Sombra do Knob
        g.setColour (juce::Colours::black.withAlpha (0.5f));
        g.fillEllipse (rx + 2.0f, ry + 3.0f, rw, rw);

        // 2. Corpo Metálico (Gradiente)
        juce::ColourGradient knobGrad (juce::Colour (0xff50545a), centreX, centreY - radius,
                                       juce::Colour (0xff1e2024), centreX, centreY + radius, false);
        g.setGradientFill (knobGrad);
        g.fillEllipse (rx, ry, rw, rw);

        // 3. Bisel e Anéis Metálicos
        g.setColour (juce::Colour (0xff70757d));
        g.drawEllipse (rx, ry, rw, rw, 1.5f);
        g.setColour (juce::Colour (0xff121315));
        g.drawEllipse (rx + 2.0f, ry + 2.0f, rw - 4.0f, rw - 4.0f, 1.0f);

        // 4. Marcador de Posição Iluminado (Linha Neon)
        juce::Path p;
        auto pointerLength = radius * 0.55f;
        auto pointerThickness = 3.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius + 4.0f, pointerThickness, pointerLength);

        g.setColour (juce::Colour (0xff00e5ff));
        g.fillPath (p, juce::AffineTransform::rotation (angle).translated (centreX, centreY));
    }

    // --- DESENHO DOS BOTÕES ILUMINADOS (TOGGLES) ---
    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced (2.0f);
        bool isOn = button.getToggleState();

        juce::Colour activeColour = juce::Colour (0xff00e5ff);
        if (button.getButtonText().containsIgnoreCase ("POWER"))
            activeColour = juce::Colour (0xffa3ff00); // Verde
        else if (button.getButtonText().containsIgnoreCase ("PING"))
            activeColour = juce::Colour (0xffe5e5ff); // Branco
        else if (button.getButtonText().containsIgnoreCase ("TEMPO"))
            activeColour = juce::Colour (0xff00d2ff); // Azul
        else if (button.getButtonText().containsIgnoreCase ("SATURATION"))
            activeColour = juce::Colour (0xffffb700); // Laranja

        // Base do Botão
        g.setColour (juce::Colour (0xff121416));
        g.fillRoundedRectangle (bounds, 4.0f);
        g.setColour (juce::Colour (0xff3a3d42));
        g.drawRoundedRectangle (bounds, 4.0f, 1.5f);

        // Estado Ligado (Luz Neon acesa)
        if (isOn)
        {
            g.setColour (activeColour.withAlpha (0.35f));
            g.fillRoundedRectangle (bounds.reduced (3.0f), 3.0f);

            g.setColour (activeColour);
            g.drawRoundedRectangle (bounds.reduced (3.0f), 3.0f, 2.0f);
        }

        // Texto do Botão
        g.setColour (isOn ? juce::Colours::white : juce::Colour (0xff8a8e94));
        g.setFont (juce::Font (11.0f, juce::Font::bold));
        g.drawText (button.getButtonText(), bounds, juce::Justification::centred, true);
    }
};
