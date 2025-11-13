#include "ThemeManager.h"
#include <iostream>

// Define static colors
const tgui::Color ThemeManager::Colors::Background{30, 30, 35};
const tgui::Color ThemeManager::Colors::PanelBackground{45, 45, 50};
const tgui::Color ThemeManager::Colors::TextAreaBackground{35, 35, 40};
const tgui::Color ThemeManager::Colors::TextAreaBorder{60, 60, 65};

const tgui::Color ThemeManager::Colors::BorderPrimary{70, 130, 180};
const tgui::Color ThemeManager::Colors::BorderSecondary{180, 130, 70};
const tgui::Color ThemeManager::Colors::BorderTertiary{130, 180, 120};

const tgui::Color ThemeManager::Colors::TextPrimary{220, 220, 220};
const tgui::Color ThemeManager::Colors::TextSecondary{200, 200, 200};

const tgui::Color ThemeManager::Colors::ButtonStart{70, 130, 180};
const tgui::Color ThemeManager::Colors::ButtonStop{180, 70, 70};
const tgui::Color ThemeManager::Colors::ButtonDefault{100, 160, 50};

// Button state colors
const tgui::Color ThemeManager::Colors::StartButtonHover{100, 150, 200};
const tgui::Color ThemeManager::Colors::StartButtonDown{50, 110, 160};
const tgui::Color ThemeManager::Colors::StartButtonBorder{90, 140, 190};

const tgui::Color ThemeManager::Colors::StopButtonHover{200, 90, 90};
const tgui::Color ThemeManager::Colors::StopButtonDown{160, 50, 50};
const tgui::Color ThemeManager::Colors::StopButtonBorder{190, 80, 80};

const tgui::Color ThemeManager::Colors::DefaultButtonHover{140, 200, 90};
const tgui::Color ThemeManager::Colors::DefaultButtonDown{100, 160, 50};
const tgui::Color ThemeManager::Colors::DefaultButtonBorder{130, 190, 80};

// Window background color
const sf::Color ThemeManager::Colors::WindowBackground{20, 20, 25};

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
        label->getRenderer()->setFont("../../../src/assets/arial.ttf");
        std::cout << "Arial font loaded successfully!" << std::endl;
        m_fontLoaded = true;
    }
    catch (const std::exception &e)
    {
        try
        {
            label->getRenderer()->setFont("assets/arial.ttf");
            std::cout << "Arial font loaded from assets directory!" << std::endl;
            m_fontLoaded = true;
        }
        catch (const std::exception &e2)
        {
            std::cout << "Font loading error (using default): " << e.what() << std::endl;
            m_fontLoaded = false;
        }
    }
}

void ThemeManager::styleButton(tgui::Button::Ptr button, const tgui::Color &bgColor,
                               const tgui::Color &hoverColor, const tgui::Color &downColor,
                               const tgui::Color &borderColor, int borderRadius)
{
    if (!button)
        return;

    button->getRenderer()->setBackgroundColor(bgColor);
    button->getRenderer()->setBackgroundColorHover(hoverColor);
    button->getRenderer()->setBackgroundColorDown(downColor);
    button->getRenderer()->setTextColor(Colors::TextPrimary);
    button->getRenderer()->setBorderColor(borderColor);
    button->getRenderer()->setBorders({1});
    button->getRenderer()->setRoundedBorderRadius(borderRadius);
}