#pragma once

#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <memory>

class SpeedControlPanel;
class DataPanel;
class TestingPanel;
class ThemeManager;

class TreadmillApp
{
public:
    TreadmillApp();
    ~TreadmillApp();

    bool initialize();
    void run();

private:
    void handleEvents();
    void handleWindowResize(const sf::Event::Resized &resizeEvent);
    void render();

    sf::RenderWindow m_window;
    tgui::Gui m_gui;

    // UI Components
    std::unique_ptr<SpeedControlPanel> m_speedPanel;
    std::unique_ptr<TestingPanel> m_testingPanel;
    std::unique_ptr<DataPanel> m_dataPanel;
    std::unique_ptr<ThemeManager> m_themeManager;

    // Main UI elements
    tgui::Panel::Ptr m_backgroundPanel;
    tgui::Label::Ptr m_titleLabel;

    bool m_running;
};