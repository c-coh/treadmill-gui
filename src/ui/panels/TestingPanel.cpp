#include "TestingPanel.h"
#include "ui/ThemeManager.h"
#include <iostream>

TestingPanel::TestingPanel() = default;
TestingPanel::~TestingPanel() = default;

void TestingPanel::initialize(tgui::Gui &gui)
{
    // Main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize(ThemeManager::Layout::HALF_PANEL_WIDTH, ThemeManager::Layout::TESTING_PANEL_HEIGHT);
    m_panel->setPosition(ThemeManager::Layout::TESTING_PANEL_X, ThemeManager::Layout::TESTING_PANEL_Y);

    // Debugging panel
    m_debugLabel = tgui::Label::create("TESTING PANEL");
    m_debugLabel->setTextSize(ThemeManager::TextSizes::LABEL_STANDARD);
    m_debugLabel->setPosition(ThemeManager::Layout::MARGIN, "8%");

    // Debugging buttons
    m_debug1Button = tgui::Button::create("DEBUG SPEED");
    m_debug1Button->setSize("28%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_debug1Button->setPosition(ThemeManager::Layout::MARGIN, "50%");

    m_debug2Button = tgui::Button::create("DEBUG FORCE DATA");
    m_debug2Button->setSize("28%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_debug2Button->setPosition("36%", "50%");

    m_debug3Button = tgui::Button::create("DEBUG DATA");
    m_debug3Button->setSize("28%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_debug3Button->setPosition("67%", "50%");

    // Set up styling
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
    m_panel->getRenderer()->setBorders({ThemeManager::Borders::PANEL_WIDTH});
    m_panel->getRenderer()->setRoundedBorderRadius(ThemeManager::Borders::PANEL_RADIUS);

    // Label styling
    m_debugLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);

    // Button styling using shared utility
    ThemeManager::styleButton(m_debug1Button, ThemeManager::Colors::ButtonDefault,
                              ThemeManager::Colors::DefaultButtonHover, ThemeManager::Colors::DefaultButtonDown,
                              ThemeManager::Colors::DefaultButtonBorder);
                              
    ThemeManager::styleButton(m_debug2Button, ThemeManager::Colors::ButtonDefault,
                              ThemeManager::Colors::DefaultButtonHover, ThemeManager::Colors::DefaultButtonDown,
                              ThemeManager::Colors::DefaultButtonBorder);

    ThemeManager::styleButton(m_debug3Button, ThemeManager::Colors::ButtonDefault,
                              ThemeManager::Colors::DefaultButtonHover, ThemeManager::Colors::DefaultButtonDown,
                              ThemeManager::Colors::DefaultButtonBorder);
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
