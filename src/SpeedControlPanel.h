#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/Widgets/FileDialog.hpp>
#include <asio.hpp>
#include <functional>
#include <fstream>
#include <sstream>
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

    // Serial communication
    bool initializeSerial(const std::string &portName = "COM3", unsigned int baudRate = 9600);
    void sendCommand(const std::string &command);
    bool isSerialConnected() const;

    // Command parsing and management
    bool parseSpeedCommands();
    const std::vector<MotorCommand> &getMotorCommands() const;
    void clearMotorCommands();
    size_t getCommandCount() const;

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
    void openSaveFileDialog();
    std::string readFileContent(const std::string &filepath);
    void readSpeedCommands();
    std::string generateConfigContent() const;
    void writeFileContent(const std::string &filepath, const std::string &content);

    tgui::Panel::Ptr m_panel;
    tgui::Label::Ptr m_speedLabel;
    tgui::TextArea::Ptr m_speedInput;
    tgui::Button::Ptr m_startButton;
    tgui::Button::Ptr m_stopButton;
    tgui::Button::Ptr m_uploadSpeedButton;
    tgui::Button::Ptr m_downloadSpeedButton;
    tgui::Button::Ptr m_downloadDataButton;
    tgui::FileDialog::Ptr m_fileDialog;

    std::unique_ptr<asio::io_context> m_ioContext;
    std::unique_ptr<asio::serial_port> m_arduinoConnection;

    std::vector<MotorCommand> m_motorCommands;

    // Callbacks
    std::function<void(const std::string &)> m_startCallback;
    std::function<void()> m_stopCallback;
    std::function<void()> m_uploadSpeedButtonCallback;
    std::function<void(const std::string &, const std::string &)> m_uploadFileCallback;
    std::function<void()> m_downloadSpeedButtonCallback;
};