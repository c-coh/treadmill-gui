#include "DataPanel.h"
#include "ui/ThemeManager.h"

// Shorter aliases for ThemeManager members
using Layout = ThemeManager::Layout;
using Colors = ThemeManager::Colors;
using TextSizes = ThemeManager::TextSizes;
using Borders = ThemeManager::Borders;

DataPanel::DataPanel() = default;
DataPanel::~DataPanel() = default;

void DataPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize(Layout::HALF_PANEL_WIDTH, Layout::DATA_PANEL_HEIGHT);
    m_panel->setPosition(Layout::DATA_PANEL_X, Layout::DATA_PANEL_Y);

    // Status title
    m_statusTitle = tgui::Label::create("OUTPUT CONSOLE");
    m_statusTitle->setTextSize(TextSizes::LABEL_STANDARD);
    m_statusTitle->setPosition(Layout::MARGIN_SMALL, Layout::MARGIN_SMALL);

    // Status text area
    m_statusText = tgui::TextArea::create();
    m_statusText->setSize("90%", "80%");
    m_statusText->setPosition(Layout::MARGIN_SMALL, "15%");
    m_statusText->setText("System initialized\n");
    m_statusText->setReadOnly(true);

    // Download Data button
    m_downloadDataButton = tgui::Button::create("DOWNLOAD DATA");
    m_downloadDataButton->setSize("30%", Layout::DATA_BUTTON_HEIGHT);
    m_downloadDataButton->setPosition("65%", Layout::MARGIN_SMALL);

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
    m_panel->getRenderer()->setBackgroundColor(Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(Colors::BorderSecondary);
    m_panel->getRenderer()->setBorders({Borders::PANEL_WIDTH});
    m_panel->getRenderer()->setRoundedBorderRadius(Borders::PANEL_RADIUS);

    // Title styling
    m_statusTitle->getRenderer()->setTextColor(Colors::TextPrimary);

    // Text area styling
    m_statusText->getRenderer()->setBackgroundColor(Colors::TextAreaBackground);
    m_statusText->getRenderer()->setTextColor(Colors::TextPrimary);
    m_statusText->getRenderer()->setBorderColor(Colors::TextAreaBorder);
    m_statusText->getRenderer()->setBorders({Borders::ELEMENT_WIDTH});
    m_statusText->getRenderer()->setRoundedBorderRadius(Borders::INPUT_RADIUS);
    m_statusText->getRenderer()->setScrollbarWidth(Borders::SCROLLBAR_WIDTH);

    ThemeManager::styleButton(m_downloadDataButton, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder);
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