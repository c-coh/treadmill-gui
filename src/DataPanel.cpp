#include "DataPanel.h"
#include "ThemeManager.h"

DataPanel::DataPanel() = default;
DataPanel::~DataPanel() = default;

void DataPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize("45%", "80%");
    m_panel->setPosition("52.5%", "10%");

    // Status title
    m_statusTitle = tgui::Label::create("DATA");
    m_statusTitle->setTextSize(20);
    m_statusTitle->setPosition("7%", "4%");

    // Status text area
    m_statusText = tgui::TextArea::create();
    m_statusText->setSize("86%", "80%");
    m_statusText->setPosition("7%", "15%");
    m_statusText->setText("Force feedback here\nSystem initialized\nReady for operation");
    m_statusText->setReadOnly(true);

    setupStyling();

    // Add widgets to panel
    m_panel->add(m_statusTitle);
    m_panel->add(m_statusText);

    // Add panel to GUI
    gui.add(m_panel);
}

void DataPanel::setupStyling()
{
    // Panel styling
    m_panel->getRenderer()->setBackgroundColor(ThemeManager::Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(ThemeManager::Colors::BorderSecondary);
    m_panel->getRenderer()->setBorders({2});
    m_panel->getRenderer()->setRoundedBorderRadius(10);

    // Title styling
    m_statusTitle->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);

    // Text area styling
    m_statusText->getRenderer()->setBackgroundColor(tgui::Color{35, 35, 40});
    m_statusText->getRenderer()->setTextColor(ThemeManager::Colors::TextSecondary);
    m_statusText->getRenderer()->setBorderColor(tgui::Color{60, 60, 65});
    m_statusText->getRenderer()->setBorders({1});
    m_statusText->getRenderer()->setRoundedBorderRadius(5);
    m_statusText->getRenderer()->setScrollbarWidth(10);
}

void DataPanel::addStatusMessage(const std::string &message)
{
    m_statusText->addText("\n" + message);

    // Auto-scroll to bottom
    auto scrollbar = m_statusText->getVerticalScrollbar();
    if (scrollbar)
    {
        scrollbar->setValue(scrollbar->getMaximum());
    }
}

void DataPanel::clearData()
{
    m_statusText->setText("Data cleared\nSystem ready");
}

void DataPanel::updateResponsiveSize(const sf::Vector2u &windowSize)
{
    // Panel is already using percentage-based sizing, so it will auto-resize
    // Any additional responsive updates can go here if needed
}