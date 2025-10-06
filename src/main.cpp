#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1000, 1000)), "Treadmill GUI");
    window.setFramerateLimit(120);

    tgui::Gui gui(window); // TGUI GUI object

    // Title label
    auto title = tgui::Label::create("Treadmill Control");
    title->setTextSize(48);
    title->setPosition(50, 20);
    gui.add(title);

    // Text box (edit box)
    auto inputBox = tgui::EditBox::create();
    inputBox->setSize(400, 50);
    inputBox->setPosition(50, 100);
    gui.add(inputBox);

    // Button
    auto submitButton = tgui::Button::create("Submit");
    submitButton->setSize(150, 50);
    submitButton->setPosition(50, 180);
    gui.add(submitButton);

    // Button callback
    submitButton->onPress([inputBox]()
                          { std::cout << "Input text: " << inputBox->getText().toStdString() << "\n"; });

    while (window.isOpen())
    {
        // sf::Event event;
        // while (window.pollEvent(event))
        // {
        //     if (event.type == sf::Event::Closed)
        //         window.close();

        //     gui.handleEvent(event); // Let TGUI handle the event
        // }

        window.clear(sf::Color::White);
        gui.draw(); // Draw all TGUI widgets
        window.display();
    }

    return 0;
}
