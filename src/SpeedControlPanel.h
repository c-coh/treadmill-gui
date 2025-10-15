#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/FileDialog.hpp>
#include <functional>
#include <fstream>
#include <sstream>

class SpeedControlPanel
{
public:
    SpeedControlPanel();
    ~SpeedControlPanel();

    void initialize(tgui::Gui &gui);

    // Event callbacks
    void setStartCallback(std::function<void(const std::string &)> callback);
    void setStopCallback(std::function<void()> callback);
    void setUploadSpeedButtonCallback(std::function<void()> callback);
    void setUploadFileCallback(std::function<void(const std::string &, const std::string &)> callback); // filename, content
    void setDownloadSpeedButtonCallback(std::function<void()> callback);

    // Getters for responsive text sizing
    tgui::Label::Ptr getSpeedLabel() const { return m_speedLabel; }
    tgui::TextArea::Ptr getSpeedInput() const { return m_speedInput; }

private:
    void setupStyling();
    void connectEvents();
    void openFileDialog();
    std::string readFileContent(const std::string &filepath);
    void readSpeedCommands();

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_speedLabel;
    tgui::TextArea::Ptr m_speedInput;
    tgui::Button::Ptr m_startButton;
    tgui::Button::Ptr m_stopButton;
    tgui::Button::Ptr m_uploadSpeedButton;
    tgui::Button::Ptr m_downloadSpeedButton;
    tgui::Button::Ptr m_downloadDataButton;
    tgui::FileDialog::Ptr m_fileDialog;

    // Callbacks
    std::function<void(const std::string &)> m_startCallback;
    std::function<void()> m_stopCallback;
    std::function<void()> m_uploadSpeedButtonCallback;
    std::function<void(const std::string &, const std::string &)> m_uploadFileCallback;
    std::function<void()> m_downloadSpeedButtonCallback;
};