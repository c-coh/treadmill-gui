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
    void setDownloadDataButtonCallback(std::function<void(const std::string &)> callback);

    // Getters for data operations
    tgui::TextArea::Ptr getStatusText() const { return m_statusText; }

private:
    void setupStyling();
    void openSaveDialog();
    void cleanupFileDialog();

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_statusTitle;
    tgui::TextArea::Ptr m_statusText;
    tgui::Button::Ptr m_downloadDataButton;
    tgui::FileDialog::Ptr m_fileDialog;
    std::function<void(const std::string &)> m_downloadDataButtonCallback;
};