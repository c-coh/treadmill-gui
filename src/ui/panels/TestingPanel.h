#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <functional>

class TestingPanel
{
public:
    TestingPanel();
    ~TestingPanel();

    void initialize(tgui::Gui &gui);

    void setDebug1Callback(std::function<void()> callback);
    void setDebug2Callback(std::function<void()> callback);
    void setDebug3Callback(std::function<void()> callback);

    tgui::Label::Ptr getdebugLabel() const { return m_debugLabel; }

private:
    void setupStyling();
    void connectEvents();

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_debugLabel;
    tgui::Button::Ptr m_debug1Button;
    tgui::Button::Ptr m_debug2Button;
    tgui::Button::Ptr m_debug3Button;

    // Callbacks
    std::function<void()> m_debug1ButtonCallback;
    std::function<void()> m_debug2ButtonCallback;
    std::function<void()> m_debug3ButtonCallback;
};