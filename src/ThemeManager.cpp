#include "ThemeManager.h"
#include <iostream>

// Define static colors
const tgui::Color ThemeManager::Colors::Background{30, 30, 35};
const tgui::Color ThemeManager::Colors::PanelBackground{45, 45, 50};
const tgui::Color ThemeManager::Colors::BorderPrimary{70, 130, 180};
const tgui::Color ThemeManager::Colors::BorderSecondary{180, 130, 70};
const tgui::Color ThemeManager::Colors::TextPrimary{220, 220, 220};
const tgui::Color ThemeManager::Colors::TextSecondary{200, 200, 200};
const tgui::Color ThemeManager::Colors::ButtonStart{70, 130, 180};
const tgui::Color ThemeManager::Colors::ButtonStop{180, 70, 70};
const tgui::Color ThemeManager::Colors::ButtonDownload{120, 180, 70};
const tgui::Color ThemeManager::Colors::InputBackground{60, 60, 65};

ThemeManager::ThemeManager() : m_themeLoaded(false), m_fontLoaded(false)
{
}

ThemeManager::~ThemeManager() = default;

bool ThemeManager::loadTheme()
{
    try
    {
        auto theme = tgui::Theme::create("themes/Black.txt");
        if (theme)
        {
            std::cout << "Black theme loaded successfully!" << std::endl;
            m_themeLoaded = true;
        }
        else
        {
            std::cout << "Could not load Black theme, using default styling" << std::endl;
            m_themeLoaded = false;
        }
    }
    catch (const std::exception &e)
    {
        std::cout << "Theme loading error: " << e.what() << std::endl;
        m_themeLoaded = false;
    }

    return m_themeLoaded;
}

void ThemeManager::setupFont(tgui::Label::Ptr label)
{
    try
    {
        // From build/bin/Debug/ we need to go up 3 levels to project root, then into src/
        label->getRenderer()->setFont("../../../src/arial.ttf");
        std::cout << "Arial font loaded successfully!" << std::endl;
        m_fontLoaded = true;
    }
    catch (const std::exception &e)
    {
        // Try alternative path
        try
        {
            label->getRenderer()->setFont("arial.ttf");
            std::cout << "Arial font loaded from current directory!" << std::endl;
            m_fontLoaded = true;
        }
        catch (const std::exception &e2)
        {
            std::cout << "Font loading error (using default): " << e.what() << std::endl;
            m_fontLoaded = false;
        }
    }
}

void ThemeManager::updateResponsiveTextSizes(const sf::Vector2u &windowSize,
                                             tgui::Label::Ptr title,
                                             tgui::Label::Ptr speedLabel,
                                             tgui::Label::Ptr statusTitle)
{
    // Calculate responsive text sizes based on window dimensions
    unsigned int titleSize = static_cast<unsigned int>(windowSize.y * 0.045f); // 4.5% of window height
    unsigned int labelSize = static_cast<unsigned int>(windowSize.y * 0.025f); // 2.5% of window height
    unsigned int statusSize = static_cast<unsigned int>(windowSize.y * 0.03f); // 3% of window height

    // Ensure minimum sizes
    titleSize = std::max(titleSize, 20u);
    labelSize = std::max(labelSize, 14u);
    statusSize = std::max(statusSize, 16u);

    // Apply the sizes
    if (title)
        title->setTextSize(titleSize);
    if (speedLabel)
        speedLabel->setTextSize(labelSize);
    if (statusTitle)
        statusTitle->setTextSize(statusSize);
}