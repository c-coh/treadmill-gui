#pragma once

#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <functional>
#include "utils/TreadmillController.h"

class SpeedControlPanel;
class DataPanel;
class TestingPanel;
class ThemeManager;

class TreadmillApp
{
public:
    TreadmillApp();
    ~TreadmillApp();

    bool initialize();
    void run();

private:
    void handleEvents();
    void handleWindowResize(const sf::Event::Resized &resizeEvent);
    void render();

    // Thread-safe UI update queue
    void queueUiUpdate(std::function<void()> updateFunc);
    void processUiUpdates();

    // Telemetry History
    std::vector<TelemetryData> m_telemetryHistory;
    std::mutex m_dataMutex;

    // Helper to save data
    void saveTelemetryToCSV(const std::string &filename);

    sf::RenderWindow m_window;
    tgui::Gui m_gui;

    // Thread-safe queue members
    std::queue<std::function<void()>> m_uiQueue;
    std::mutex m_uiQueueMutex;

    // UI Components
    std::unique_ptr<SpeedControlPanel> m_speedPanel;
    std::unique_ptr<TestingPanel> m_testingPanel;
    std::unique_ptr<DataPanel> m_dataPanel;
    std::unique_ptr<ThemeManager> m_themeManager;

    // Core Systems
    std::shared_ptr<TreadmillController> m_treadmillController;

    // Main UI elements
    tgui::Panel::Ptr m_backgroundPanel;
    tgui::Label::Ptr m_titleLabel;

    bool m_running;
};