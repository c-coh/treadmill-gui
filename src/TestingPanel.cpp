#include "TestingPanel.h"
#include "ThemeManager.h"
#include <iostream>

TestingPanel::TestingPanel() = default;
TestingPanel::~TestingPanel() = default;

void TestingPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize("45%", "20%");
    m_panel->setPosition("4%", "75%");

    // Debugging panel
    m_debugLabel = tgui::Label::create("Debugging Panel");
    m_debugLabel->setTextSize(18);
    m_debugLabel->setPosition("5%", "8%");

    // Debug 1
    m_debug1Button = tgui::Button::create("DEBUG SPEED");
    m_debug1Button->setSize("28%", "25%");
    m_debug1Button->setPosition("5%", "50%");

    m_debug2Button = tgui::Button::create("DEBUG FORCE DATA");
    m_debug2Button->setSize("28%", "25%");
    m_debug2Button->setPosition("36%", "50%");

    m_debug3Button = tgui::Button::create("DEBUG DATA");
    m_debug3Button->setSize("28%", "25%");
    m_debug3Button->setPosition("67%", "50%");

    // Setup styling and add to panel
    setupStyling();
    connectEvents();

    // Add widgets to panel
    m_panel->add(m_debugLabel);
    m_panel->add(m_debug1Button);
    m_panel->add(m_debug2Button);
    m_panel->add(m_debug3Button);

    // Add panel to GUI
    gui.add(m_panel);
}

void TestingPanel::setupStyling()
{
    // Panel styling
    m_panel->getRenderer()->setBackgroundColor(ThemeManager::Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(ThemeManager::Colors::BorderTertiary);
    m_panel->getRenderer()->setBorders({2});
    m_panel->getRenderer()->setRoundedBorderRadius(10);

    // Label styling
    m_debugLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);

    // Start button styling
    m_debug1Button->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonStart);
    m_debug1Button->getRenderer()->setBackgroundColorHover(tgui::Color{100, 150, 200});
    m_debug1Button->getRenderer()->setBackgroundColorDown(tgui::Color{50, 110, 160});
    m_debug1Button->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_debug1Button->getRenderer()->setBorderColor(tgui::Color{90, 140, 190});
    m_debug1Button->getRenderer()->setBorders({1});
    m_debug1Button->getRenderer()->setRoundedBorderRadius(8);
    m_debug1Button->getRenderer()->setTextSize(14);

    // Stop button styling
    m_debug2Button->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonStart);
    m_debug2Button->getRenderer()->setBackgroundColorHover(tgui::Color{200, 90, 90});
    m_debug2Button->getRenderer()->setBackgroundColorDown(tgui::Color{160, 50, 50});
    m_debug2Button->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_debug2Button->getRenderer()->setBorderColor(tgui::Color{90, 140, 190});
    m_debug2Button->getRenderer()->setBorders({1});
    m_debug2Button->getRenderer()->setRoundedBorderRadius(8);

    // Download button styling
    m_debug3Button->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonStart);
    m_debug3Button->getRenderer()->setBackgroundColorHover(tgui::Color{140, 200, 90});
    m_debug3Button->getRenderer()->setBackgroundColorDown(tgui::Color{100, 160, 50});
    m_debug3Button->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_debug3Button->getRenderer()->setBorderColor(tgui::Color{90, 140, 190});
    m_debug3Button->getRenderer()->setBorders({1});
    m_debug3Button->getRenderer()->setRoundedBorderRadius(8);
}

void TestingPanel::connectEvents()
{
    m_debug1Button->onPress([this]()
                            {
        if (m_debug1ButtonCallback) {
            m_debug1ButtonCallback();
        } });

    m_debug2Button->onPress([this]()
                            {
        if (m_debug2ButtonCallback) {
            m_debug2ButtonCallback();
        } });

    m_debug3Button->onPress([this]()
                            {
        if (m_debug3ButtonCallback) {
            m_debug3ButtonCallback();
        } });
}

void TestingPanel::setDebug1Callback(std::function<void()> callback)
{
    m_debug1ButtonCallback = callback;
}

void TestingPanel::setDebug2Callback(std::function<void()> callback)
{
    m_debug2ButtonCallback = callback;
}

void TestingPanel::setDebug3Callback(std::function<void()> callback)
{
    m_debug3ButtonCallback = callback;
}

void TestingPanel::updateResponsiveSize(const sf::Vector2u &windowSize)
{
    // Panel is already using percentage-based sizing, so it will auto-resize
    // Any additional responsive updates can go here if needed
}
