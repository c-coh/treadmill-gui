#include "SpeedControlPanel.h"
#include "ui/ThemeManager.h"
#include "utils/FileManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>

SpeedControlPanel::SpeedControlPanel()
    : m_serialManager(std::make_unique<SerialManager>())
{
}

SpeedControlPanel::~SpeedControlPanel() = default;

void SpeedControlPanel::initialize(tgui::Gui &gui)
{
    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize(ThemeManager::Layout::HALF_PANEL_WIDTH, "60%");
    m_panel->setPosition(ThemeManager::Layout::MARGIN, "10%");

    // Speed label
    m_speedLabel = tgui::Label::create("SPEED CONTROL");
    m_speedLabel->setTextSize(ThemeManager::TextSizes::LABEL_STANDARD);
    m_speedLabel->setPosition(ThemeManager::Layout::MARGIN, "8%");

    // Speed input
    m_speedInput = tgui::TextArea::create();
    m_speedInput->setSize(ThemeManager::Layout::INPUT_WIDTH, ThemeManager::Layout::INPUT_HEIGHT);
    m_speedInput->setPosition(ThemeManager::Layout::MARGIN, "20%");
    m_speedInput->setDefaultText("Enter speed");

    // Start button
    m_startButton = tgui::Button::create("START");
    m_startButton->setSize("20%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_startButton->setPosition(ThemeManager::Layout::MARGIN, "70%");

    // Stop button
    m_stopButton = tgui::Button::create("STOP");
    m_stopButton->setSize("20%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_stopButton->setPosition("30%", "70%");

    // Upload Speed button
    m_uploadSpeedButton = tgui::Button::create("UPLOAD SPEED CONFIG");
    m_uploadSpeedButton->setSize("35%", ThemeManager::Layout::BUTTON_HEIGHT);
    m_uploadSpeedButton->setPosition(ThemeManager::Layout::MARGIN, "85%");

    // Download Speed button
    m_downloadSpeedButton = tgui::Button::create("DOWNLOAD SPEED CONFIG");
    m_downloadSpeedButton->setSize("35%", ThemeManager::Layout::BUTTON_HEIGHT);
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

std::string SpeedControlPanel::formatMotorCommand(const MotorCommand &cmd) const
{
    std::ostringstream oss;
    oss << "L:" << cmd.leftSpeed << " R:" << cmd.rightSpeed << " T:" << cmd.time;
    return oss.str();
}

void SpeedControlPanel::setupStyling()
{
    // Panel styling
    m_panel->getRenderer()->setBackgroundColor(ThemeManager::Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(ThemeManager::Colors::BorderPrimary);
    m_panel->getRenderer()->setBorders({ThemeManager::Borders::PANEL_WIDTH});
    m_panel->getRenderer()->setRoundedBorderRadius(ThemeManager::Borders::PANEL_RADIUS);

    // Label styling
    m_speedLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);

    // Input styling
    m_speedInput->getRenderer()->setBackgroundColor(ThemeManager::Colors::TextAreaBackground);
    m_speedInput->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
    m_speedInput->getRenderer()->setBorderColor(ThemeManager::Colors::TextAreaBorder);
    m_speedInput->getRenderer()->setBorders({ThemeManager::Borders::ELEMENT_WIDTH});
    m_speedInput->getRenderer()->setRoundedBorderRadius(ThemeManager::Borders::INPUT_RADIUS);

    // Button styling using helper method
    ThemeManager::styleButton(m_startButton, ThemeManager::Colors::ButtonStart,
                              ThemeManager::Colors::StartButtonHover, ThemeManager::Colors::StartButtonDown,
                              ThemeManager::Colors::StartButtonBorder, ThemeManager::Borders::BUTTON_RADIUS);
    m_startButton->getRenderer()->setTextSize(ThemeManager::TextSizes::BUTTON_TEXT);

    ThemeManager::styleButton(m_stopButton, ThemeManager::Colors::ButtonStop,
                              ThemeManager::Colors::StopButtonHover, ThemeManager::Colors::StopButtonDown,
                              ThemeManager::Colors::StopButtonBorder, ThemeManager::Borders::BUTTON_RADIUS);

    ThemeManager::styleButton(m_uploadSpeedButton, ThemeManager::Colors::ButtonDefault,
                              ThemeManager::Colors::DefaultButtonHover, ThemeManager::Colors::DefaultButtonDown,
                              ThemeManager::Colors::DefaultButtonBorder, ThemeManager::Borders::BUTTON_RADIUS);

    ThemeManager::styleButton(m_downloadSpeedButton, ThemeManager::Colors::ButtonDefault,
                              ThemeManager::Colors::DefaultButtonHover, ThemeManager::Colors::DefaultButtonDown,
                              ThemeManager::Colors::DefaultButtonBorder, ThemeManager::Borders::BUTTON_RADIUS);
}

void SpeedControlPanel::connectEvents()
{
    m_startButton->onPress([this]()
                           {
        parseSpeedCommands();
        
        // Check if we have valid commands to send
        if (!m_motorCommands.empty()) {
            
            // Send start command followed by the motor commands
            m_serialManager->sendCommand("START_TREADMILL");
            
            // Send each motor command
            for (size_t i = 0; i < m_motorCommands.size(); ++i) {
                const auto& cmd = m_motorCommands[i];
                m_serialManager->sendCommand(formatMotorCommand(cmd));
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
        m_serialManager->sendCommand("STOP_TREADMILL");
        
        if (m_stopCallback) {
            m_stopCallback();
        } });

    m_uploadSpeedButton->onPress([this]()
                                 { openFileDialog(true); });

    m_downloadSpeedButton->onPress([this]()
                                   { openFileDialog(false); });
}

void SpeedControlPanel::setStartCallback(std::function<void(const std::string &)> callback)
{
    m_startCallback = callback;
}

void SpeedControlPanel::setStopCallback(std::function<void()> callback)
{
    m_stopCallback = callback;
}

void SpeedControlPanel::setUploadFileCallback(std::function<void(const std::string &, const std::string &)> callback)
{
    m_uploadFileCallback = callback;
}

void SpeedControlPanel::cleanupFileDialog()
{
    if (m_fileDialog && m_fileDialog->getParent())
    {
        m_fileDialog->getParent()->remove(m_fileDialog);
    }
}

void SpeedControlPanel::setupFileDialog()
{
    cleanupFileDialog();

    // Create a new file dialog each time to avoid state issues
    m_fileDialog = tgui::FileDialog::create();
    m_fileDialog->setFileTypeFilters({{"Text files", {"*.txt"}},
                                      {"Config files", {"*.cfg", "*.conf"}},
                                      {"All files", {"*.*"}}});
    m_fileDialog->setSelectingDirectory(false);
    m_fileDialog->setMultiSelect(false);
}

void SpeedControlPanel::openFileDialog(bool isLoadDialog)
{
    setupFileDialog();

    if (!isLoadDialog)
    {
        // Configure for save dialog
        m_fileDialog->setFilename("speed_config.txt");
        m_fileDialog->setFilenameLabelText("Save as:");
    }

    // File select callback
    m_fileDialog->onFileSelect([this, isLoadDialog](const std::vector<tgui::Filesystem::Path> &files)
                               {
        if (!files.empty()) {
            std::string filepath = files[0].asString().toStdString();
            
            if (isLoadDialog) {
                // Load file
                if (m_uploadFileCallback) {
                    try {
                        std::string content = FileManager::readFile(filepath);
                        std::string filename = files[0].getFilename().toStdString();
                        m_uploadFileCallback(filename, content);
                    } catch (const std::exception& e) {
                        std::cerr << "Error reading file: " << e.what() << std::endl;
                    }
                }
            } else {
                // Save file
                filepath = FileManager::ensureExtension(filepath, ".txt");
                try {
                    std::string content = generateConfigContent();
                    FileManager::writeFile(filepath, content);
                    std::cout << "Successfully saved speed config to: " << filepath << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Error saving file: " << e.what() << std::endl;
                }
            }
        }
        cleanupFileDialog(); });

    // Close callback
    m_fileDialog->onClose([this]()
                          { cleanupFileDialog(); });

    // Add to GUI and show
    if (m_panel && m_panel->getParent())
    {
        m_panel->getParent()->add(m_fileDialog);
    }
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
            content << formatMotorCommand(cmd) << "\n";
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