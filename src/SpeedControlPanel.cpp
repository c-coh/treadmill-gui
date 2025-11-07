#include "SpeedControlPanel.h"
#include "ThemeManager.h"
#include <iostream>
#include <algorithm>

SpeedControlPanel::SpeedControlPanel()
    : m_ioContext(std::make_unique<asio::io_context>())
{
}

SpeedControlPanel::~SpeedControlPanel() = default;

bool SpeedControlPanel::initializeSerial(const std::string &portName, unsigned int baudRate)
{
    try
    {
        // Configure serial connection
        m_arduinoConnection = std::make_unique<asio::serial_port>(*m_ioContext, portName);

        m_arduinoConnection->set_option(asio::serial_port_base::baud_rate(baudRate));
        m_arduinoConnection->set_option(asio::serial_port_base::character_size(8));
        m_arduinoConnection->set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
        m_arduinoConnection->set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
        m_arduinoConnection->set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));

        // Check if connection is available
        if (m_arduinoConnection->is_open())
        {
            std::cout << "Serial connection established on " << portName << " at " << baudRate << " baud" << std::endl;
            return true;
        }
        else
        {
            std::cerr << "Failed to open serial port " << portName << std::endl;
            return false;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Serial connection error: " << e.what() << std::endl;
        m_arduinoConnection.reset();
        return false;
    }
}

void SpeedControlPanel::sendCommand(const std::string &command)
{
    if (m_arduinoConnection && m_arduinoConnection->is_open())
    {
        try
        {
            // Send command
            std::string commandWithNewline = command + "\n";
            asio::write(*m_arduinoConnection, asio::buffer(commandWithNewline));
            std::cout << "Sent to Arduino: " << command << std::endl;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error sending command: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cerr << "Serial connection not available. Command not sent: " << command << std::endl;
    }
}

bool SpeedControlPanel::isSerialConnected() const
{
    return m_arduinoConnection && m_arduinoConnection->is_open();
}

void SpeedControlPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize("45%", "60%");
    m_panel->setPosition("4%", "10%");

    // Speed label
    m_speedLabel = tgui::Label::create("Speed Control");
    m_speedLabel->setTextSize(18);
    m_speedLabel->setPosition("5%", "8%");

    // Speed input
    m_speedInput = tgui::TextArea::create();
    m_speedInput->setSize("85%", "45%");
    m_speedInput->setPosition("5%", "20%");
    m_speedInput->setDefaultText("Enter speed");

    // Start button
    m_startButton = tgui::Button::create("START");
    m_startButton->setSize("20%", "10%");
    m_startButton->setPosition("5%", "70%");

    // Stop button
    m_stopButton = tgui::Button::create("STOP");
    m_stopButton->setSize("20%", "10%");
    m_stopButton->setPosition("30%", "70%");

    // Upload Speed button
    m_uploadSpeedButton = tgui::Button::create("UPLOAD SPEED CONFIG");
    m_uploadSpeedButton->setSize("35%", "10%");
    m_uploadSpeedButton->setPosition("5%", "85%");

    // Download Speed button
    m_downloadSpeedButton = tgui::Button::create("DOWNLOAD SPEED CONFIG");
    m_downloadSpeedButton->setSize("35%", "10%");
    m_downloadSpeedButton->setPosition("45%", "85%");

    // File dialog will be created fresh each time it's needed

    // Setup styling and add to panel
    setupStyling();
    connectEvents();

    // Add widgets to panel
    m_panel->add(m_speedLabel);
    m_panel->add(m_speedInput);
    m_panel->add(m_startButton);
    m_panel->add(m_stopButton);
    m_panel->add(m_uploadSpeedButton);
    m_panel->add(m_downloadSpeedButton);

    // Add panel to GUI
    gui.add(m_panel);

    // File dialog will be created and added when needed
}

void SpeedControlPanel::setupStyling()
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

    m_uploadSpeedButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonDownload);
    m_uploadSpeedButton->getRenderer()->setBackgroundColorHover(tgui::Color{140, 200, 90});
    m_uploadSpeedButton->getRenderer()->setBackgroundColorDown(tgui::Color{100, 160, 50});
    m_uploadSpeedButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_uploadSpeedButton->getRenderer()->setBorderColor(tgui::Color{130, 190, 80});
    m_uploadSpeedButton->getRenderer()->setBorders({1});
    m_uploadSpeedButton->getRenderer()->setRoundedBorderRadius(8);

    m_downloadSpeedButton->getRenderer()->setBackgroundColor(ThemeManager::Colors::ButtonDownload);
    m_downloadSpeedButton->getRenderer()->setBackgroundColorHover(tgui::Color{140, 200, 90});
    m_downloadSpeedButton->getRenderer()->setBackgroundColorDown(tgui::Color{100, 160, 50});
    m_downloadSpeedButton->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_downloadSpeedButton->getRenderer()->setBorderColor(tgui::Color{130, 190, 80});
    m_downloadSpeedButton->getRenderer()->setBorders({1});
    m_downloadSpeedButton->getRenderer()->setRoundedBorderRadius(8);
}

void SpeedControlPanel::connectEvents()
{
    m_startButton->onPress([this]()
                           {
        readSpeedCommands();
        
        // Check if we have valid commands to send
        if (!m_motorCommands.empty()) {
            
            // Send start command followed by the motor commands
            sendCommand("START_TREADMILL");
            
            // Send each motor command
            for (size_t i = 0; i < m_motorCommands.size(); ++i) {
                const auto& cmd = m_motorCommands[i];
                std::string motorCommand = "L:" + std::to_string(cmd.leftSpeed) + 
                                         " R:" + std::to_string(cmd.rightSpeed) + 
                                         " T:" + std::to_string(cmd.time);
                sendCommand(motorCommand);
            }
        } else {
            std::cerr << "No valid motor commands to send!" << std::endl;
        }
        
        if (m_startCallback) {
            std::string speed = m_speedInput->getText().toStdString();
            m_startCallback(speed);
        } });

    m_stopButton->onPress([this]()
                          {
        // Send stop command to Arduino
        sendCommand("STOP_TREADMILL");
        
        if (m_stopCallback) {
            m_stopCallback();
        } });

    m_uploadSpeedButton->onPress([this]()
                                 {
        openFileDialog();
        if (m_uploadSpeedButtonCallback) {
            m_uploadSpeedButtonCallback();
        } });

    m_downloadSpeedButton->onPress([this]()
                                   {
        openSaveFileDialog();
        if (m_downloadSpeedButtonCallback) {
            m_downloadSpeedButtonCallback();
        } });

}

void SpeedControlPanel::setStartCallback(std::function<void(const std::string &)> callback)
{
    m_startCallback = callback;
}

void SpeedControlPanel::setStopCallback(std::function<void()> callback)
{
    m_stopCallback = callback;
}

void SpeedControlPanel::setUploadSpeedButtonCallback(std::function<void()> callback)
{
    m_uploadSpeedButtonCallback = callback;
}

void SpeedControlPanel::setDownloadSpeedButtonCallback(std::function<void()> callback)
{
    m_downloadSpeedButtonCallback = callback;
}

void SpeedControlPanel::setUploadFileCallback(std::function<void(const std::string &, const std::string &)> callback)
{
    m_uploadFileCallback = callback;
}

void SpeedControlPanel::openFileDialog()
{
    if (m_fileDialog && m_fileDialog->getParent())
    {
        m_fileDialog->getParent()->remove(m_fileDialog);
    }

    // Create a new file dialog each time to avoid state issues
    m_fileDialog = tgui::FileDialog::create();
    m_fileDialog->setFileTypeFilters({{"Text files", {"*.txt"}},
                                      {"Config files", {"*.cfg", "*.conf"}},
                                      {"All files", {"*.*"}}});
    m_fileDialog->setSelectingDirectory(false);
    m_fileDialog->setMultiSelect(false);

    // File select callback
    m_fileDialog->onFileSelect([this](const std::vector<tgui::Filesystem::Path> &files)
                               {
        if (!files.empty() && m_uploadFileCallback) {
            std::string filepath = files[0].asString().toStdString();
            try {
                std::string content = readFileContent(filepath);
                std::string filename = files[0].getFilename().toStdString();
                m_uploadFileCallback(filename, content);
            } catch (const std::exception& e) {
                std::cerr << "Error reading file: " << e.what() << std::endl;
            }
        }
        // Remove window after use
        if (m_fileDialog && m_fileDialog->getParent()) {
            m_fileDialog->getParent()->remove(m_fileDialog);
        } });

    // Close callback
    m_fileDialog->onClose([this]()
                          {
        if (m_fileDialog && m_fileDialog->getParent()) {
            m_fileDialog->getParent()->remove(m_fileDialog);
        } });

    // Add to GUI and show
    if (m_panel && m_panel->getParent())
    {
        m_panel->getParent()->add(m_fileDialog);
    }
}

std::string SpeedControlPanel::readFileContent(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void SpeedControlPanel::readSpeedCommands()
{
    // Parse the commands when this method is called
    parseSpeedCommands();
}

bool SpeedControlPanel::parseSpeedCommands()
{
    m_motorCommands.clear();

    // Get text from text area
    std::string commands = m_speedInput->getText().toStdString();
    std::istringstream stream(commands);
    std::string line;

    // Regex to parse the speed commands
    std::regex commandRegex(R"(L:\s*(-?\d+(?:\.\d+)?)\s+R:\s*(-?\d+(?:\.\d+)?)\s+T:\s*(-?\d+(?:\.\d+)?))");
    std::smatch matches;

    int lineNumber = 0;
    bool hasValidCommands = false;

    while (std::getline(stream, line))
    {
        lineNumber++;

        // Skip empty lines and lines with only whitespace
        if (line.empty() || std::all_of(line.begin(), line.end(), ::isspace))
        {
            continue;
        }

        if (std::regex_search(line, matches, commandRegex))
        {
            try
            {
                double leftSpeed = std::stod(matches[1].str());
                double rightSpeed = std::stod(matches[2].str());
                double time = std::stod(matches[3].str());

                // Check for formatting errors
                if (time <= 0)
                {
                    std::cerr << "Warning: Line " << lineNumber << " has invalid time value (must be > 0): " << time << std::endl;
                    continue;
                }

                m_motorCommands.emplace_back(leftSpeed, rightSpeed, time);
                hasValidCommands = true;

                std::cout << "Parsed command " << m_motorCommands.size() << ": L:" << leftSpeed
                          << " R:" << rightSpeed << " T:" << time << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error parsing line " << lineNumber << ": " << e.what() << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Line " << lineNumber << " doesn't match expected format: " << line << std::endl;
        }
    }

    std::cout << "Successfully parsed " << m_motorCommands.size() << " motor commands." << std::endl;
    return hasValidCommands;
}

const std::vector<MotorCommand> &SpeedControlPanel::getMotorCommands() const
{
    return m_motorCommands;
}

void SpeedControlPanel::clearMotorCommands()
{
    m_motorCommands.clear();
}

size_t SpeedControlPanel::getCommandCount() const
{
    return m_motorCommands.size();
}

void SpeedControlPanel::openSaveFileDialog()
{
    if (m_fileDialog && m_fileDialog->getParent())
    {
        m_fileDialog->getParent()->remove(m_fileDialog);
    }

    // Create a new file dialog each time to avoid state issues
    m_fileDialog = tgui::FileDialog::create();
    m_fileDialog->setFileTypeFilters({{"Text files", {"*.txt"}},
                                      {"Config files", {"*.cfg", "*.conf"}},
                                      {"All files", {"*.*"}}});
    m_fileDialog->setSelectingDirectory(false);
    m_fileDialog->setMultiSelect(false);

    m_fileDialog->setFilename("speed_config.txt");
    m_fileDialog->setFilenameLabelText("Save as:");

    // File select callback for saving
    m_fileDialog->onFileSelect([this](const std::vector<tgui::Filesystem::Path> &files)
                               {
        if (!files.empty()) {
            std::string filepath = files[0].asString().toStdString();
            
            // Ensure the file has an extension
            if (filepath.find_last_of('.') == std::string::npos) {
                filepath += ".txt"; 
            }
            
            try {
                std::string content = generateConfigContent();
                writeFileContent(filepath, content);
                
                std::cout << "Successfully saved speed config to: " << filepath << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error saving file: " << e.what() << std::endl;
            }
        }
        // Remove window after use
        if (m_fileDialog && m_fileDialog->getParent()) {
            m_fileDialog->getParent()->remove(m_fileDialog);
        } });

    // Close callback
    m_fileDialog->onClose([this]()
                          {
        if (m_fileDialog && m_fileDialog->getParent()) {
            m_fileDialog->getParent()->remove(m_fileDialog);
        } });

    // Add to GUI and show
    if (m_panel && m_panel->getParent())
    {
        m_panel->getParent()->add(m_fileDialog);
    }
}

std::string SpeedControlPanel::generateConfigContent() const
{
    std::stringstream content;

    // If we have parsed motor commands, use those
    if (!m_motorCommands.empty())
    {
        content << "# Generated Speed Configuration\n";
        content << "# Format: L:{left_speed} R:{right_speed} T:{time}\n\n";

        for (const auto &cmd : m_motorCommands)
        {
            content << "L:" << cmd.leftSpeed << " R:" << cmd.rightSpeed << " T:" << cmd.time << "\n";
        }
    }
    // Otherwise, use the raw text from the input area
    else
    {
        std::string inputText = m_speedInput->getText().toStdString();
        if (inputText.empty() || inputText == "Enter speed")
        {
            content << "# Empty Speed Configuration\n";
            content << "# Add commands in format: L:{left_speed} R:{right_speed} T:{time}\n";
            content << "# Example:\n";
            content << "# L:1.5 R:2.0 T:3.0\n";
        }
        else
        {
            content << inputText;
        }
    }

    return content.str();
}

void SpeedControlPanel::writeFileContent(const std::string &filepath, const std::string &content)
{
    std::ofstream file(filepath);
    if (!file.is_open())
    {
        throw std::runtime_error("Could not create file: " + filepath);
    }

    file << content;
    file.close();

    if (file.fail())
    {
        throw std::runtime_error("Error writing to file: " + filepath);
    }
}