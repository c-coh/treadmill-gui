#include "SpeedControlPanel.h"
#include "ui/ThemeManager.h"
#include "utils/FileManager.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <thread> // Added for background execution

// Shorter aliases for ThemeManager members
using Layout = ThemeManager::Layout;
using Colors = ThemeManager::Colors;
using TextSizes = ThemeManager::TextSizes;
using Borders = ThemeManager::Borders;

SpeedControlPanel::SpeedControlPanel()
{
}

SpeedControlPanel::~SpeedControlPanel() = default;

void SpeedControlPanel::initialize(tgui::Gui &gui, std::shared_ptr<TreadmillController> treadmillController)
{
    m_treadmillController = treadmillController;

    // Create main panel
    m_panel = tgui::Panel::create();
    m_panel->setSize(Layout::HALF_PANEL_WIDTH, "60%");
    m_panel->setPosition(Layout::MARGIN_SMALL, "10%");

    // Speed label
    m_speedLabel = tgui::Label::create("SPEED CONTROL");
    m_speedLabel->setTextSize(TextSizes::LABEL_STANDARD);
    m_speedLabel->setPosition(Layout::MARGIN_SMALL, "8%");

    // Speed input
    m_speedInput = tgui::TextArea::create();
    m_speedInput->setSize(Layout::INPUT_WIDTH, "30%");
    m_speedInput->setPosition(Layout::MARGIN_SMALL, "20%");
    m_speedInput->setDefaultText("Enter speed");

    // Telemetry Display
    m_telemetryLabel = tgui::Label::create("Telemetry: Not Running");
    m_telemetryLabel->setTextSize(TextSizes::LABEL_SMALL);
    m_telemetryLabel->setPosition(Layout::MARGIN_SMALL, "52%");
    m_telemetryLabel->setSize(Layout::PANEL_WIDTH, "8%");

    // Port selection
    m_portLabel = tgui::Label::create("PORT:");
    m_portLabel->setTextSize(TextSizes::BUTTON_TEXT);
    m_portLabel->setPosition(Layout::MARGIN_SMALL, "60%");
    m_portLabel->setVerticalAlignment(tgui::VerticalAlignment::Center);

    m_portInput = tgui::EditBox::create();
    m_portInput->setSize("20%", Layout::SPEED_BUTTON_HEIGHT);
    m_portInput->setPosition("15%", "60%");
    m_portInput->setDefaultText("COM3");

    m_connectButton = tgui::Button::create("CONNECT");
    m_connectButton->setSize("25%", Layout::SPEED_BUTTON_HEIGHT);
    m_connectButton->setPosition("40%", "60%");

    // Start button
    m_startButton = tgui::Button::create("START");
    m_startButton->setSize("20%", Layout::SPEED_BUTTON_HEIGHT);
    m_startButton->setPosition(Layout::MARGIN_SMALL, "72%");

    // Stop button
    m_stopButton = tgui::Button::create("STOP");
    m_stopButton->setSize("20%", Layout::SPEED_BUTTON_HEIGHT);
    m_stopButton->setPosition("30%", "72%");

    // Upload Speed button
    m_uploadSpeedButton = tgui::Button::create("UPLOAD SPEED CONFIG");
    m_uploadSpeedButton->setSize("35%", Layout::SPEED_BUTTON_HEIGHT);
    m_uploadSpeedButton->setPosition(Layout::MARGIN_SMALL, "85%");

    // Download Speed button
    m_downloadSpeedButton = tgui::Button::create("DOWNLOAD SPEED CONFIG");
    m_downloadSpeedButton->setSize("35%", Layout::SPEED_BUTTON_HEIGHT);
    m_downloadSpeedButton->setPosition("45%", "85%");

    // File dialog will be created fresh each time it's needed

    // Setup styling and add to panel
    setupStyling();
    connectEvents();

    // Add widgets to panel
    m_panel->add(m_speedLabel);
    m_panel->add(m_speedInput);
    m_panel->add(m_portLabel);
    m_panel->add(m_portInput);
    m_panel->add(m_connectButton);
    m_panel->add(m_telemetryLabel);
    m_panel->add(m_startButton);
    m_panel->add(m_stopButton);
    m_panel->add(m_uploadSpeedButton);
    m_panel->add(m_downloadSpeedButton);

    // Add panel to GUI
    gui.add(m_panel);

    // Register telemetry callback
    m_treadmillController->setTelemetryCallback([this](const TelemetryData &data)
                                                { updateTelemetryUI(data); });

    // File dialog will be created and added when needed
}

void SpeedControlPanel::setupStyling()
{
    // Panel styling
    m_panel->getRenderer()->setBackgroundColor(Colors::PanelBackground);
    m_panel->getRenderer()->setBorderColor(Colors::BorderPrimary);
    m_panel->getRenderer()->setBorders({Borders::PANEL_WIDTH});
    m_panel->getRenderer()->setRoundedBorderRadius(Borders::PANEL_RADIUS);

    // Label styling
    m_speedLabel->getRenderer()->setTextColor(Colors::TextPrimary);

    // Input styling
    m_speedInput->getRenderer()->setBackgroundColor(Colors::TextAreaBackground);
    m_speedInput->getRenderer()->setTextColor(Colors::TextPrimary);
    m_speedInput->getRenderer()->setBorderColor(Colors::TextAreaBorder);
    m_speedInput->getRenderer()->setBorders({Borders::ELEMENT_WIDTH});
    m_speedInput->getRenderer()->setRoundedBorderRadius(Borders::INPUT_RADIUS);

    // Port styling
    m_portLabel->getRenderer()->setTextColor(Colors::TextPrimary);

    m_portInput->getRenderer()->setBackgroundColor(Colors::TextAreaBackground);
    m_portInput->getRenderer()->setTextColor(Colors::TextPrimary);
    m_portInput->getRenderer()->setBorderColor(Colors::TextAreaBorder);
    m_portInput->getRenderer()->setBorders({Borders::ELEMENT_WIDTH});
    m_portInput->getRenderer()->setRoundedBorderRadius(Borders::INPUT_RADIUS);

    // Telemetry styling
    m_telemetryLabel->getRenderer()->setTextColor(Colors::TextPrimary);

    ThemeManager::styleButton(m_connectButton, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder, Borders::BUTTON_RADIUS);
    m_connectButton->getRenderer()->setTextSize(TextSizes::BUTTON_TEXT);

    // Button styling using helper method
    ThemeManager::styleButton(m_startButton, Colors::ButtonStart,
                              Colors::StartButtonHover, Colors::StartButtonDown,
                              Colors::StartButtonBorder, Borders::BUTTON_RADIUS);
    m_startButton->getRenderer()->setTextSize(TextSizes::BUTTON_TEXT);

    ThemeManager::styleButton(m_stopButton, Colors::ButtonStop,
                              Colors::StopButtonHover, Colors::StopButtonDown,
                              Colors::StopButtonBorder, Borders::BUTTON_RADIUS);

    ThemeManager::styleButton(m_uploadSpeedButton, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder, Borders::BUTTON_RADIUS);

    ThemeManager::styleButton(m_downloadSpeedButton, Colors::ButtonDefault,
                              Colors::DefaultButtonHover, Colors::DefaultButtonDown,
                              Colors::DefaultButtonBorder, Borders::BUTTON_RADIUS);
}

void SpeedControlPanel::connectEvents()
{
    m_connectButton->onPress([this]()
                             {
        std::string port = m_portInput->getText().toStdString();
        if (port.empty()) {
            if (m_statusCallback) m_statusCallback("Error: Port name cannot be empty");
            return;
        }
        
        if (m_treadmillController->initialize(port, 500000)) {
            if (m_statusCallback) m_statusCallback("Successfully connected to " + port);
            m_connectButton->setText("CONNECTED");
            m_connectButton->getRenderer()->setBackgroundColor(Colors::ButtonDefault); // Keep green
        } else {
            if (m_statusCallback) m_statusCallback("Failed to connect to " + port);
            m_connectButton->setText("RETRY");
            m_connectButton->getRenderer()->setBackgroundColor(Colors::ButtonStop); // Red for error
        } });

    m_startButton->onPress([this]()
                           {
        parseSpeedCommands();
        
        // Check if we have valid commands to send
        if (!m_motorCommands.empty()) {
            // Send all commands at once using the treadmill controller
            // Status messages are now handled by TreadmillController
            
            // Run in a background thread to prevent UI freezing during protocol handshake
            std::vector<std::string> commands = m_motorCommands; // Copy commands
            auto controller = m_treadmillController; // Keep controller alive
            
            std::thread([controller, commands]() {
                controller->runTreadmill(commands);
            }).detach();
            
        } else {
            std::cerr << "No valid motor commands to send!" << std::endl;
            if (m_statusCallback) {
                m_statusCallback("ERROR: No valid commands to send");
            }
        } });

    m_stopButton->onPress([this]()
                          {
        // Send stop command using treadmill controller
        // Status messages are now handled by TreadmillController
        m_treadmillController->stopTreadmill(); });

    m_uploadSpeedButton->onPress([this]()
                                 { openFileDialog(true); });

    m_downloadSpeedButton->onPress([this]()
                                   { openFileDialog(false); });
}

void SpeedControlPanel::setStatusCallback(std::function<void(const std::string &)> callback)
{
    m_statusCallback = callback;
    // Also forward the callback to TreadmillController so it can send status updates
    // Intercept to handle "FINISHED" state
    m_treadmillController->setStatusCallback([this, callback](const std::string &msg)
                                             {
        if (msg == "FINISHED") {
            // Re-enable start button, disable stop button if needed
            // For now just log it, or update UI state if we had specific state management
            if (callback) callback("Run finished successfully");
        } else {
            if (callback) callback(msg);
        } });
}

void SpeedControlPanel::updateTelemetryUI(const TelemetryData &data)
{
    std::stringstream ss;
    ss << "L: " << std::fixed << std::setprecision(1) << data.actualRpm1 << " / " << data.targetRpm1 << " RPM  |  "
       << "R: " << data.actualRpm2 << " / " << data.targetRpm2 << " RPM\n"
       << "Time: " << (data.timestamp / 1000.0) << "s  |  "
       << "Drivers: " << (data.driver1Healthy ? "OK" : "ERR") << "/" << (data.driver2Healthy ? "OK" : "ERR");

    m_telemetryLabel->setText(ss.str());
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

    try
    {
        m_fileDialog->setPath(FileManager::getDownloadsPath());
    }
    catch (...)
    {
        // Fallback to default if setting path fails
    }
}

void SpeedControlPanel::openFileDialog(bool isLoadDialog)
{
    setupFileDialog();

    if (!isLoadDialog)
    {
        // Configure for save dialog
        m_fileDialog->setFilename("speed_config.txt");
        m_fileDialog->setFilenameLabelText("Save as:");
        m_fileDialog->setFileMustExist(false);
        m_fileDialog->setConfirmButtonText("Save");
    }
    else
    {
        m_fileDialog->setFileMustExist(true);
        m_fileDialog->setConfirmButtonText("Open");
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

    // Regex to validate the speed commands format
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
                // Validate the time value
                double time = std::stod(matches[3].str());
                if (time <= 0)
                {
                    std::cerr << "Warning: Line " << lineNumber << " has invalid time value (must be > 0): " << time << std::endl;
                    continue;
                }

                m_motorCommands.push_back(line);
                hasValidCommands = true;

                std::cout << "Parsed command " << m_motorCommands.size() << ": " << line << std::endl;
            }
            catch (const std::exception &e)
            {
                std::cerr << "Error parsing line " << lineNumber << ": " << e.what() << std::endl;
            }
        }
        else
        {
            std::cerr << "Warning: Line " << lineNumber << " doesn't match expected format, skipping: " << line << std::endl;
        }
    }

    std::cout << "Successfully parsed " << m_motorCommands.size() << " motor commands." << std::endl;
    return hasValidCommands;
}

const std::vector<std::string> &SpeedControlPanel::getMotorCommands() const
{
    return m_motorCommands;
}

std::string SpeedControlPanel::generateConfigContent() const
{
    std::string inputText = m_speedInput->getText().toStdString();

    // If the input is empty or just the default text, generate a template
    if (inputText.empty() || inputText == "Enter speed")
    {
        std::stringstream content;
        content << "# Empty Speed Configuration\n";
        content << "# Add commands in format: L:{left_speed} R:{right_speed} T:{time}\n";
        content << "# Example:\n";
        content << "# L:1.5 R:2.0 T:3.0\n";
        return content.str();
    }

    // Otherwise, save exactly what is in the text area
    // This ensures that any edits made by the user (even if not yet run) are saved
    return inputText;
}