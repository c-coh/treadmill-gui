#include "TreadmillApp.h"
#include "ui/panels/SpeedControlPanel.h"
#include "ui/panels/TestingPanel.h"
#include "ui/panels/DataPanel.h"
#include "ui/ThemeManager.h"
#include <iostream>

TreadmillApp::TreadmillApp()
    : m_window(sf::VideoMode(sf::Vector2u(1200, 800)), "Treadmill Control System"), m_gui(m_window), m_running(false)
{
    m_window.setFramerateLimit(60);
}

TreadmillApp::~TreadmillApp() = default;

// -------- INITIALIZATIONS ---------
bool TreadmillApp::initialize()
{
    try
    {
        // Initialize theme manager
        m_themeManager = std::make_unique<ThemeManager>();
        m_themeManager->loadTheme();

        // Create background panel
        m_backgroundPanel = tgui::Panel::create();
        m_backgroundPanel->setSize("100%", "100%");
        m_backgroundPanel->getRenderer()->setBackgroundColor(ThemeManager::Colors::Background);
        m_gui.add(m_backgroundPanel);

        // Create title label
        m_titleLabel = tgui::Label::create("TREADMILL CONTROLLER");
        m_titleLabel->setTextSize(ThemeManager::TextSizes::TITLE_LARGE);
        m_titleLabel->setPosition("4%", "4%");
        m_titleLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
        m_themeManager->setupFont(m_titleLabel);
        m_gui.add(m_titleLabel);

        // Initialize sub-panels
        m_speedPanel = std::make_unique<SpeedControlPanel>();
        m_speedPanel->initialize(m_gui);

        m_dataPanel = std::make_unique<DataPanel>();
        m_dataPanel->initialize(m_gui);

        m_testingPanel = std::make_unique<TestingPanel>();
        m_testingPanel->initialize(m_gui);

        // Attempt to initialize the serial connection
        if (!m_speedPanel->getSerialManager()->initialize("COM3", 9600))
        {
            std::cout << "Warning: Could not connect to Arduino on COM3. USB commands will not be sent." << std::endl;
        }

        // Set up event callbacks for speed control
        m_speedPanel->setStartCallback([this](const std::string &speed)
                                       {
            std::cout << "Starting treadmill at speed: " << speed << std::endl;
            m_dataPanel->addStatusMessage("Starting at " + speed); });

        m_speedPanel->setStopCallback([this]()
                                      {
            std::cout << "Stop" << std::endl;
            m_dataPanel->addStatusMessage("Treadmill stopped"); });

        m_speedPanel->setUploadFileCallback([this](const std::string &filename, const std::string &content)
                                            {
                                                std::cout << "Uploaded file: " << filename << " (" << content.length() << " characters)" << std::endl;
                                                m_dataPanel->addStatusMessage("Uploaded file: " + filename);
                                                m_dataPanel->addStatusMessage("File size: " + std::to_string(content.length()) + " characters");
                                                
                                                m_speedPanel->getSpeedInput()->setText(content);
                                                m_dataPanel->addStatusMessage("File content loaded into speed input"); });

        std::cout << "All components initialized." << std::endl;
        m_running = true;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

// -------- MAIN LOOP ---------
void TreadmillApp::run()
{
    std::cout << "Starting main loop..." << std::endl;

    while (m_window.isOpen() && m_running)
    {
        handleEvents();
        render();
    }

    std::cout << "Application closing normally" << std::endl;
}

void TreadmillApp::handleEvents()
{
    while (auto event = m_window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            m_window.close();
            m_running = false;
        }

        if (event->is<sf::Event::Resized>())
        {
            handleWindowResize(*event->getIf<sf::Event::Resized>());
        }

        m_gui.handleEvent(*event);
    }
}

void TreadmillApp::handleWindowResize(const sf::Event::Resized &resizeEvent)
{
    m_gui.setAbsoluteView(tgui::FloatRect{0.f, 0.f,
                                          static_cast<float>(resizeEvent.size.x),
                                          static_cast<float>(resizeEvent.size.y)});

    std::cout << "Window resized to: " << resizeEvent.size.x
              << "x" << resizeEvent.size.y << std::endl;
}

void TreadmillApp::render()
{
    m_window.clear(ThemeManager::Colors::WindowBackground);
    m_gui.draw();
    m_window.display();
}