#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/FileDialog.hpp>
#include "utils/SerialManager.h"
#include <functional>
#include <memory>
#include <vector>
#include <string>
#include <regex>

// Struct for motor commands
struct MotorCommand
{
    double leftSpeed;
    double rightSpeed;
    double time;

    MotorCommand(double left = 0.0, double right = 0.0, double duration = 0.0)
        : leftSpeed(left), rightSpeed(right), time(duration) {}
};

class SpeedControlPanel
{
public:
    SpeedControlPanel();
    ~SpeedControlPanel();

    void initialize(tgui::Gui &gui);

    // Command parsing and management
    bool parseSpeedCommands();
    const std::vector<MotorCommand> &getMotorCommands() const;

    // Event callbacks
    void setStartCallback(std::function<void(const std::string &)> callback);
    void setStopCallback(std::function<void()> callback);
    void setUploadFileCallback(std::function<void(const std::string &, const std::string &)> callback); // filename, content

    // Getters for file operations
    tgui::TextArea::Ptr getSpeedInput() const { return m_speedInput; }
    SerialManager *getSerialManager() const { return m_serialManager.get(); }

private:
    void setupStyling();
    void connectEvents();
    void openFileDialog(bool isLoadDialog = true);
    void setupFileDialog();
    void cleanupFileDialog();
    std::string formatMotorCommand(const MotorCommand &cmd) const;
    std::string generateConfigContent() const;

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_speedLabel;
    tgui::TextArea::Ptr m_speedInput;
    tgui::Button::Ptr m_startButton;
    tgui::Button::Ptr m_stopButton;
    tgui::Button::Ptr m_uploadSpeedButton;
    tgui::Button::Ptr m_downloadSpeedButton;
    tgui::FileDialog::Ptr m_fileDialog;

    std::unique_ptr<SerialManager> m_serialManager;
    std::vector<MotorCommand> m_motorCommands;

    // Callbacks
    std::function<void(const std::string &)> m_startCallback;
    std::function<void()> m_stopCallback;
    std::function<void(const std::string &, const std::string &)> m_uploadFileCallback;
};