#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

class DataPanel
{
public:
    DataPanel();
    ~DataPanel();

    void initialize(tgui::Gui &gui);

    void addStatusMessage(const std::string &message);
    void clearData();
    void setDownloadDataButtonCallback(std::function<void()> callback);

    // Getters for responsive text sizing
    tgui::Label::Ptr getStatusTitle() const { return m_statusTitle; }
    tgui::TextArea::Ptr getStatusText() const { return m_statusText; }

private:
    void setupStyling();

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_statusTitle;
    tgui::TextArea::Ptr m_statusText;
    tgui::Button::Ptr m_downloadDataButton;
    std::function<void()> m_downloadDataButtonCallback;
};