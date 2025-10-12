#include "TreadmillApp.h"
#include "SpeedControlPanel.h"
#include "DataPanel.h"
#include "ThemeManager.h"
#include <iostream>

TreadmillApp::TreadmillApp()
    : m_window(sf::VideoMode(sf::Vector2u(1200, 800)), "Treadmill Control System"), m_gui(m_window), m_running(false)
{
    m_window.setFramerateLimit(60);
}

TreadmillApp::~TreadmillApp() = default;

bool TreadmillApp::initialize()
{
    try
    {
        std::cout << "Initializing Treadmill GUI..." << std::endl;
        std::cout << "Window created successfully" << std::endl;
        std::cout << "GUI initialized successfully" << std::endl;

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
        m_titleLabel->setTextSize(36);
        m_titleLabel->setPosition("4%", "4%");
        m_titleLabel->getRenderer()->setTextColor(ThemeManager::Colors::TextPrimary);
        m_themeManager->setupFont(m_titleLabel);
        m_gui.add(m_titleLabel);

        // Initialize panels
        m_speedPanel = std::make_unique<SpeedControlPanel>();
        m_speedPanel->initialize(m_gui);

        m_dataPanel = std::make_unique<DataPanel>();
        m_dataPanel->initialize(m_gui);

        // Setup event callbacks
        m_speedPanel->setStartCallback([this](const std::string &speed)
                                       {
            std::cout << "Starting treadmill at speed: " << speed << " mph" << std::endl;
            m_dataPanel->addStatusMessage("Starting at " + speed + " mph"); });

        m_speedPanel->setStopCallback([this]()
                                      {
            std::cout << "STOP" << std::endl;
            m_dataPanel->addStatusMessage("EMERGENCY STOP ACTIVATED"); });

        m_speedPanel->setDownloadCallback([this]()
                                          {
            std::cout << "Downloading force feedback..." << std::endl;
            m_dataPanel->addStatusMessage("Downloading data..."); });

        updateResponsiveSizes();

        std::cout << "All components initialized. Ready to start..." << std::endl;
        m_running = true;
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        return false;
    }
}

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
    // Update the GUI view to match new window size - SFML 3.x syntax
    m_gui.setAbsoluteView(tgui::FloatRect{0.f, 0.f,
                                        static_cast<float>(resizeEvent.size.x),
                                        static_cast<float>(resizeEvent.size.y)});
    updateResponsiveSizes();

    std::cout << "Window resized to: " << resizeEvent.size.x
              << "x" << resizeEvent.size.y << std::endl;
}

void TreadmillApp::updateResponsiveSizes()
{
    if (m_themeManager && m_titleLabel && m_speedPanel && m_dataPanel)
    {
        m_themeManager->updateResponsiveTextSizes(
            m_window.getSize(),
            m_titleLabel,
            m_speedPanel->getSpeedLabel(),
            m_dataPanel->getStatusTitle());
    }
}

void TreadmillApp::render()
{
    m_window.clear(sf::Color{20, 20, 25});
    m_gui.draw();
    m_window.display();
}