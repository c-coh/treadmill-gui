#include "TreadmillApp.h"
#include "ui/panels/SpeedControlPanel.h"
#include "ui/panels/TestingPanel.h"
#include "ui/panels/DataPanel.h"
#include "ui/ThemeManager.h"
#include <iostream>

// Shorter aliases for ThemeManager members
using Colors = ThemeManager::Colors;
using TextSizes = ThemeManager::TextSizes;

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

        // Initialize core systems
        m_treadmillController = std::make_shared<TreadmillController>();

        // Create background panel
        m_backgroundPanel = tgui::Panel::create();
        m_backgroundPanel->setSize("100%", "100%");
        m_backgroundPanel->getRenderer()->setBackgroundColor(Colors::Background);
        m_gui.add(m_backgroundPanel);

        // Create title label
        m_titleLabel = tgui::Label::create("TREADMILL CONTROLLER");
        m_titleLabel->setTextSize(TextSizes::TITLE_LARGE);
        m_titleLabel->setPosition("4%", "4%");
        m_titleLabel->getRenderer()->setTextColor(Colors::TextPrimary);
        m_themeManager->setupFont(m_titleLabel);
        m_gui.add(m_titleLabel);

        // Initialize sub-panels
        m_speedPanel = std::make_unique<SpeedControlPanel>();
        m_speedPanel->initialize(m_gui, m_treadmillController);

        m_dataPanel = std::make_unique<DataPanel>();
        m_dataPanel->initialize(m_gui);

        m_testingPanel = std::make_unique<TestingPanel>();
        m_testingPanel->initialize(m_gui, m_treadmillController);

        // Initial status message
        m_dataPanel->addStatusMessage("System initialized. Please select COM port and connect.");

        // Set up event callbacks
        // Use queueUiUpdate to ensure thread safety for callbacks coming from background threads
        m_speedPanel->setStatusCallback([this](const std::string &message)
                                        { queueUiUpdate([this, message]()
                                                        { m_dataPanel->addStatusMessage(message); }); });

        m_speedPanel->setUploadFileCallback([this](const std::string &filename, const std::string &content)
                                            {
                                                // This callback comes from UI thread (button press), so it's safe to run directly,
                                                // but for consistency we can queue it or leave it. 
                                                // Since it's triggered by file dialog (UI), it's on main thread.
                                                std::cout << "Uploaded file: " << filename << " (" << content.length() << " characters)" << std::endl;
                                                m_dataPanel->addStatusMessage("Uploaded file: " + filename);
                                                m_dataPanel->addStatusMessage("File size: " + std::to_string(content.length()) + " characters");
                                                
                                                m_speedPanel->getSpeedInput()->setText(content);
                                                m_dataPanel->addStatusMessage("File content loaded into speed input"); });

        // Also need to handle telemetry updates which come from background thread
        // We need to intercept the telemetry callback set in SpeedControlPanel
        // But SpeedControlPanel sets it directly on the controller.
        // We should modify SpeedControlPanel to use a thread-safe wrapper or handle it here.
        // Ideally, SpeedControlPanel should not set the callback directly on the controller if it's not thread safe.
        // Let's override it here to be safe, or better, modify SpeedControlPanel to use a dispatcher.
        // Since SpeedControlPanel::initialize sets the callback, we can wrap the controller's callback mechanism.

        // Re-register the telemetry callback with thread safety
        m_treadmillController->setTelemetryCallback([this](const TelemetryData &data)
                                                    { queueUiUpdate([this, data]()
                                                                    { m_speedPanel->updateTelemetryUI(data); }); });

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
        processUiUpdates(); // Process any pending UI updates from background threads
        render();
    }

    std::cout << "Application closing normally" << std::endl;
}

void TreadmillApp::queueUiUpdate(std::function<void()> updateFunc)
{
    std::lock_guard<std::mutex> lock(m_uiQueueMutex);
    m_uiQueue.push(updateFunc);
}

void TreadmillApp::processUiUpdates()
{
    std::lock_guard<std::mutex> lock(m_uiQueueMutex);
    while (!m_uiQueue.empty())
    {
        auto func = m_uiQueue.front();
        m_uiQueue.pop();
        if (func)
        {
            func();
        }
    }
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
    m_window.clear(Colors::WindowBackground);
    m_gui.draw();
    m_window.display();
}