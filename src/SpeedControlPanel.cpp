#include "SpeedControlPanel.h"
#include "ThemeManager.h"
#include <iostream>

SpeedControlPanel::SpeedControlPanel() = default;
SpeedControlPanel::~SpeedControlPanel() = default;

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
        if (m_startCallback) {
            std::string speed = m_speedInput->getText().toStdString();
            m_startCallback(speed);
        } });

    m_stopButton->onPress([this]()
                          {
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
        if (m_downloadSpeedButtonCallback) {
            m_downloadSpeedButtonCallback();
        } });

    // File dialog events are now handled in openFileDialog() method
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
    // Remove the old file dialog if it exists
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

    // Set up the file select callback
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
        // Remove the dialog after use
        if (m_fileDialog && m_fileDialog->getParent()) {
            m_fileDialog->getParent()->remove(m_fileDialog);
        } });

    // Set up the close callback
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
    // // Get text from text area
    // std::string commands = m_speedInput->getText().toStdString();
    // std::istringstream stream(commands);
    // std::string line;
    // while (std::getline(stream, line)) {
    //     std::cout << "Running: " << line << std::endl;
    // }

}