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

    // Button styling utility
    static void styleButton(tgui::Button::Ptr button, const tgui::Color &bgColor,
                            const tgui::Color &hoverColor, const tgui::Color &downColor,
                            const tgui::Color &borderColor, int borderRadius = 8);

    // Color scheme
    struct Colors
    {
        static const tgui::Color Background;
        static const tgui::Color PanelBackground;
        static const tgui::Color TextAreaBackground;
        static const tgui::Color TextAreaBorder;
        static const tgui::Color BorderPrimary;
        static const tgui::Color BorderSecondary;
        static const tgui::Color BorderTertiary;
        static const tgui::Color TextPrimary;
        static const tgui::Color TextSecondary;
        static const tgui::Color ButtonStart;
        static const tgui::Color ButtonStop;
        static const tgui::Color ButtonDefault;

        // Button state colors
        static const tgui::Color StartButtonHover;
        static const tgui::Color StartButtonDown;
        static const tgui::Color StartButtonBorder;

        static const tgui::Color StopButtonHover;
        static const tgui::Color StopButtonDown;
        static const tgui::Color StopButtonBorder;

        static const tgui::Color DefaultButtonHover;
        static const tgui::Color DefaultButtonDown;
        static const tgui::Color DefaultButtonBorder;

        // Window background color
        static const sf::Color WindowBackground;
    };

    // Layout dimensions
    struct Layout
    {
        // Panel dimensions
        static constexpr const char *HALF_PANEL_WIDTH = "44%";

        static constexpr const char *SPEED_PANEL_HEIGHT = "60%";
        static constexpr const char *SPEED_PANEL_X = "4%";
        static constexpr const char *SPEED_PANEL_Y = "10%";

        static constexpr const char *DATA_PANEL_HEIGHT = "85%";
        static constexpr const char *DATA_PANEL_X = "52.5%";
        static constexpr const char *DATA_PANEL_Y = "10%";

        static constexpr const char *TESTING_PANEL_HEIGHT = "20%";
        static constexpr const char *TESTING_PANEL_X = "4%";
        static constexpr const char *TESTING_PANEL_Y = "75%";

        // Common dimensions
        static constexpr const char *BUTTON_HEIGHT = "10%";
        static constexpr const char *INPUT_WIDTH = "85%";
        static constexpr const char *INPUT_HEIGHT = "45%";

        // Common spacing values
        static constexpr const char *MARGIN = "4%";
        static constexpr const char *MARGIN_MEDIUM = "7%";
        static constexpr const char *MARGIN_LARGE = "15%";
    };

    // Text sizes
    struct TextSizes
    {
        static constexpr int TITLE_LARGE = 36;
        static constexpr int TITLE_MEDIUM = 20;
        static constexpr int LABEL_STANDARD = 18;
        static constexpr int BUTTON_TEXT = 14;
    };

    // Border and radius values
    struct Borders
    {
        static constexpr int PANEL_RADIUS = 10;
        static constexpr int BUTTON_RADIUS = 8;
        static constexpr int INPUT_RADIUS = 5;
        static constexpr int PANEL_WIDTH = 2;
        static constexpr int ELEMENT_WIDTH = 1;
        static constexpr int SCROLLBAR_WIDTH = 10;
    };

private:
    bool m_themeLoaded;
    bool m_fontLoaded;
};