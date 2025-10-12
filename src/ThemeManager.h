#pragma once

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <SFML/Graphics.hpp>

class ThemeManager
{
public:
    ThemeManager();
    ~ThemeManager();

    bool loadTheme();
    void setupFont(tgui::Label::Ptr label);
    void updateResponsiveTextSizes(const sf::Vector2u &windowSize,
                                   tgui::Label::Ptr title,
                                   tgui::Label::Ptr speedLabel,
                                   tgui::Label::Ptr statusTitle);

    // Color scheme
    struct Colors
    {
        static const tgui::Color Background;
        static const tgui::Color PanelBackground;
        static const tgui::Color BorderPrimary;
        static const tgui::Color BorderSecondary;
        static const tgui::Color TextPrimary;
        static const tgui::Color TextSecondary;
        static const tgui::Color ButtonStart;
        static const tgui::Color ButtonStop;
        static const tgui::Color ButtonDownload;
        static const tgui::Color InputBackground;
    };

private:
    bool m_themeLoaded;
    bool m_fontLoaded;
};