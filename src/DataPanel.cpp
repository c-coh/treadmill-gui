#include "DataPanel.h"
#include "ThemeManager.h"

DataPanel::DataPanel() = default;
DataPanel::~DataPanel() = default;

void DataPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize("45%", "85%");
    m_panel->setPosition("52.5%", "10%");

    // Status title
    m_statusTitle = tgui::Label::create("OUTPUT CONSOLE");
    m_statusTitle->setTextSize(20);
    m_statusTitle->setPosition("7%", "4%");

    // Status text area
    m_statusText = tgui::TextArea::create();
    m_statusText->setSize("90%", "80%");
    m_statusText->setPosition("5%", "15%");
    m_statusText->setText("System initialized\n");
    m_statusText->setReadOnly(true);

    // Download Data button
    m_downloadDataButton = tgui::Button::create("DOWNLOAD DATA");
    m_downloadDataButton->setSize("30%", "8%");
    m_downloadDataButton->setPosition("65%", "4%");

    setupStyling();

    // Add widgets to panel
    m_panel->add(m_statusTitle);
    m_panel->add(m_statusText);
    m_panel->add(m_downloadDataButton);

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

    m_downloadDataButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonDownload);
    m_downloadDataButton->getRenderer()->setBackgroundColorHover(tgui::Color{140, 200, 90});
    m_downloadDataButton->getRenderer()->setBackgroundColorDown(tgui::Color{100, 160, 50});
    m_downloadDataButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_downloadDataButton->getRenderer()->setBorderColor(tgui::Color{130, 190, 80});
    m_downloadDataButton->getRenderer()->setBorders({1});
    m_downloadDataButton->getRenderer()->setRoundedBorderRadius(8);
}

void DataPanel::setDownloadDataButtonCallback(std::function<void()> callback)
{
    m_downloadDataButtonCallback = std::move(callback);
    m_downloadDataButton->onPress([this]()
                                  {
        if (m_downloadDataButtonCallback) {
            m_downloadDataButtonCallback();
        } });
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
    m_statusText->setText("Data cleared\n");
}