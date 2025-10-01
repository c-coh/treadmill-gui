#include <SFML/Graphics.hpp>

int main()
{
    unsigned int width = 1000;
    unsigned int height = 1000;
    
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(width, height)), "Treadmill GUI");
    window.setFramerateLimit(120);

    sf::Font font;
    if (!font.openFromFile("C:/Users/cecic/Desktop/Projects/Class_Projects/treadmill-gui/src/arial.ttf"))
        return -1;

    // title text
    sf::Text title(font, "Treadmill Control", 100);
    title.setFillColor(sf::Color::Black);
    title.setPosition(sf::Vector2f(0.1f * width, 0.05f * height));

    // text box mockup
    sf::RectangleShape inputBox(sf::Vector2f(400.f, 60.f));
    inputBox.setPosition(sf::Vector2f(0.1f * width, 0.3f * height));
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setOutlineThickness(3.f);

    // button mockup
    sf::RectangleShape submitButton(sf::Vector2f(150.f, 60.f));
    submitButton.setPosition(sf::Vector2f(0.1f * width, 0.45f * height));
    submitButton.setFillColor(sf::Color::White);
    submitButton.setOutlineColor(sf::Color::Black);
    submitButton.setOutlineThickness(3.f);

    // button text
    sf::Text buttonText(font, "Submit", 30);
    buttonText.setFillColor(sf::Color::Black);
    buttonText.setPosition(sf::Vector2f(
        submitButton.getPosition().x + 20.f,
        submitButton.getPosition().y + 10.f
    ));

    // rendering
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear(sf::Color::White);
        window.draw(title);
        window.draw(inputBox);
        window.draw(submitButton);
        window.draw(buttonText);
        window.display();
    }

    return 0;
}