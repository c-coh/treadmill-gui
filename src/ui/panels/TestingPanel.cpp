#include "TestingPanel.h"
#include "ui/ThemeManager.h"
#include <iostream>
#include <thread>
#include <vector>

// Shorter aliases for ThemeManager members
using Layout = ThemeManager::Layout;
using Colors = ThemeManager::Colors;
using TextSizes = ThemeManager::TextSizes;
using Borders = ThemeManager::Borders;

TestingPanel::TestingPanel() = default;
TestingPanel::~TestingPanel() = default;

void TestingPanel::initialize(tgui::Gui &gui, std::shared_ptr<TreadmillController> treadmillController)
{
    m_treadmillController = treadmillController;

    // Main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize(Layout::HALF_PANEL_WIDTH, Layout::TESTING_PANEL_HEIGHT);
    m_panel->setPosition(Layout::TESTING_PANEL_X, Layout::TESTING_PANEL_Y);

    // Debugging panel
    m_debugLabel = tgui::Label::create("TESTING PANEL");
    m_debugLabel->setTextSize(TextSizes::LABEL_STANDARD);
    m_debugLabel->setPosition(Layout::MARGIN_SMALL, "8%");

    // Debugging buttons
    m_debug1Button = tgui::Button::create("DEBUG 1");
    m_debug1Button->setSize("28%", Layout::TESTING_BUTTON_HEIGHT);
    m_debug1Button->setPosition(Layout::MARGIN_SMALL, "50%");

    m_debug2Button = tgui::Button::create("DEBUG 2");
    m_debug2Button->setSize("28%", Layout::TESTING_BUTTON_HEIGHT);
    m_debug2Button->setPosition("36%", "50%");

    m_debug3Button = tgui::Button::create("DEBUG 3");
    m_debug3Button->setSize("28%", Layout::TESTING_BUTTON_HEIGHT);
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
    m_panel->getRenderer()->setBackgroundColor(Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(Colors::BorderTertiary);
    m_panel->getRenderer()->setBorders({Borders::PANEL_WIDTH});
    m_panel->getRenderer()->setRoundedBorderRadius(Borders::PANEL_RADIUS);

    // Label styling
    m_debugLabel->getRenderer()->setTextColor(Colors::TextPrimary);

    // Button styling
    ThemeManager::styleButton(m_debug1Button, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder);

    ThemeManager::styleButton(m_debug2Button, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder);

    ThemeManager::styleButton(m_debug3Button, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder);
}

void TestingPanel::connectEvents()
{
    m_debug1Button->onPress([this]()
                            {
        // Debug 1: Run command L:20 R:20 T:10
        std::vector<std::string> commands = {"L:20 R:20 T:10"};
        auto controller = m_treadmillController;
        
        std::thread([controller, commands]() {
            controller->runTreadmill(commands);
        }).detach();

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
