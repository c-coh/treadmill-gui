#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/FileDialog.hpp>
#include "utils/TreadmillController.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <regex>

class SpeedControlPanel
{
public:
    SpeedControlPanel();
    ~SpeedControlPanel();

    void initialize(tgui::Gui &gui, std::shared_ptr<TreadmillController> treadmillController);

    // Command parsing and management
    bool parseSpeedCommands();
    const std::vector<std::string> &getMotorCommands() const;

    // Event callbacks
    void setStatusCallback(std::function<void(const std::string &)> callback);
    void setUploadFileCallback(std::function<void(const std::string &, const std::string &)> callback);

    // Getters for file operations
    tgui::TextArea::Ptr getSpeedInput() const { return m_speedInput; }
    TreadmillController *getTreadmillController() const { return m_treadmillController.get(); }

    void updateTelemetryUI(const TelemetryData &data);

private:
    void setupStyling();
    void connectEvents();
    void openFileDialog(bool isLoadDialog = true);
    void setupFileDialog();
    void cleanupFileDialog();
    std::string generateConfigContent() const;

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_speedLabel;
    tgui::TextArea::Ptr m_speedInput;
    tgui::Button::Ptr m_startButton;
    tgui::Button::Ptr m_stopButton;
    tgui::Button::Ptr m_uploadSpeedButton;
    tgui::Button::Ptr m_downloadSpeedButton;

    // Port selection
    tgui::Label::Ptr m_portLabel;
    tgui::EditBox::Ptr m_portInput;
    tgui::Button::Ptr m_connectButton;

    // Telemetry display
    tgui::Label::Ptr m_telemetryLabel;

    tgui::FileDialog::Ptr m_fileDialog;

    std::shared_ptr<TreadmillController> m_treadmillController;
    std::vector<std::string> m_motorCommands;

    // Callbacks
    std::function<void(const std::string &)> m_statusCallback;
    std::function<void(const std::string &, const std::string &)> m_uploadFileCallback;
};