#include "TestingPanel.h"
#include "ThemeManager.h"
#include <iostream>

TestingPanel::TestingPanel() = default;
TestingPanel::~TestingPanel() = default;

void TestingPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize("45%", "45%");
    m_panel->setPosition("54%", "60%");

    // Speed label
    m_speedLabel = tgui::Label::create("Speed Control");
    m_speedLabel->setTextSize(18);
    m_speedLabel->setPosition("5%", "8%");

    // Speed input
    m_speedInput = tgui::TextArea::create();
    m_speedInput->setSize("85%", "40%");
    m_speedInput->setPosition("5%", "20%");
    m_speedInput->setDefaultText("Enter speed");

    // Start button
    m_startButton = tgui::Button::create("START TREADMILL");
    m_startButton->setSize("30%", "10%");
    m_startButton->setPosition("5%", "70%");

    // Stop button
    m_stopButton = tgui::Button::create("STOP");
    m_stopButton->setSize("20%", "10%");
    m_stopButton->setPosition("50%", "70%");

    // Download button
    m_downloadButton = tgui::Button::create("DOWNLOAD DATA");
    m_downloadButton->setSize("45%", "10%");
    m_downloadButton->setPosition("27.5%", "85%");

    // Setup styling and add to panel
    setupStyling();
    connectEvents();

    // Add widgets to panel
    m_panel->add(m_speedLabel);
    m_panel->add(m_speedInput);
    m_panel->add(m_startButton);
    m_panel->add(m_stopButton);
    m_panel->add(m_downloadButton);

    // Add panel to GUI
    gui.add(m_panel);
}

void TestingPanel::setupStyling()
{
    // Panel styling
    m_panel->getRenderer()->setBackgroundColor(ThemeManager::Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(ThemeManager::Colors::BorderPrimary);
    m_panel->getRenderer()->setBorders({2});
    m_panel->getRenderer()->setRoundedBorderRadius(10);

    // Label styling
    m_speedLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);

    // Input styling
    m_speedInput->getRenderer()->setBackgroundColor(ThemeManager::Colors::InputBackground);
    m_speedInput->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_speedInput->getRenderer()->setBorderColor(tgui::Color{100, 100, 110});
    m_speedInput->getRenderer()->setBorders({1});
    m_speedInput->getRenderer()->setRoundedBorderRadius(5);

    // Start button styling
    m_startButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonStart);
    m_startButton->getRenderer()->setBackgroundColorHover(tgui::Color{100, 150, 200});
    m_startButton->getRenderer()->setBackgroundColorDown(tgui::Color{50, 110, 160});
    m_startButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_startButton->getRenderer()->setBorderColor(tgui::Color{90, 140, 190});
    m_startButton->getRenderer()->setBorders({1});
    m_startButton->getRenderer()->setRoundedBorderRadius(8);
    m_startButton->getRenderer()->setTextSize(14);

    // Stop button styling
    m_stopButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonStop);
    m_stopButton->getRenderer()->setBackgroundColorHover(tgui::Color{200, 90, 90});
    m_stopButton->getRenderer()->setBackgroundColorDown(tgui::Color{160, 50, 50});
    m_stopButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_stopButton->getRenderer()->setBorderColor(tgui::Color{190, 80, 80});
    m_stopButton->getRenderer()->setBorders({1});
    m_stopButton->getRenderer()->setRoundedBorderRadius(8);

    // Download button styling
    m_downloadButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonDownload);
    m_downloadButton->getRenderer()->setBackgroundColorHover(tgui::Color{140, 200, 90});
    m_downloadButton->getRenderer()->setBackgroundColorDown(tgui::Color{100, 160, 50});
    m_downloadButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_downloadButton->getRenderer()->setBorderColor(tgui::Color{130, 190, 80});
    m_downloadButton->getRenderer()->setBorders({1});
    m_downloadButton->getRenderer()->setRoundedBorderRadius(8);
}

void TestingPanel::connectEvents()
{
    m_startButton->onPress([this]()
                           {
        if (m_startCallback) {
            std::string speed = m_speedInput->getText().toStdString();
            m_startCallback(speed);
        } });

    m_stopButton->onPress([this]()
                          {
        if (m_stopCallback) {
            m_stopCallback();
        } });

    m_downloadButton->onPress([this]()
                              {
        if (m_downloadCallback) {
            m_downloadCallback();
        } });
}

void TestingPanel::setStartCallback(std::function<void(const std::string &)> callback)
{
    m_startCallback = callback;
}

void TestingPanel::setStopCallback(std::function<void()> callback)
{
    m_stopCallback = callback;
}

void TestingPanel::setDownloadCallback(std::function<void()> callback)
{
    m_downloadCallback = callback;
}
