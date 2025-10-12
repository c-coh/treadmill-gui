#pragma once

#include <TGUI/TGUI.hpp>
#include <functional>

class TestingPanel
{
public:
    TestingPanel();
    ~TestingPanel();

    void initialize(tgui::Gui &gui);
    void updateResponsiveSize(const sf::Vector2u &windowSize);

    // Event callbacks
    void setStartCallback(std::function<void(const std::string &)> callback);
    void setStopCallback(std::function<void()> callback);
    void setDownloadCallback(std::function<void()> callback);

    // Getters for responsive text sizing
    tgui::Label::Ptr getSpeedLabel() const { return m_speedLabel; }
    tgui::TextArea::Ptr getSpeedInput() const { return m_speedInput; }

private:
    void setupStyling();
    void connectEvents();

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_speedLabel;
    tgui::TextArea::Ptr m_speedInput;
    tgui::Button::Ptr m_startButton;
    tgui::Button::Ptr m_stopButton;
    tgui::Button::Ptr m_downloadButton;

    // Callbacks
    std::function<void(const std::string &)> m_startCallback;
    std::function<void()> m_stopCallback;
    std::function<void()> m_downloadCallback;
};