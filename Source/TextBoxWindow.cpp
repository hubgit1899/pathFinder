#include "Headers/TextBoxWindow.h"
#include <iostream>

TextBoxWindow::TextBoxWindow() : textBoxWindow(sf::VideoMode(400, 200), "hehehehehehe")
{
    if (!font.loadFromFile("Resources/Futura.ttc"))
    {
        std::cerr << "Failed to load font!" << std::endl;
    }
    box.setSize(sf::Vector2f(350, 50));
    box.setFillColor(sf::Color(60, 60, 95));
    box.setOutlineColor(sf::Color::White);
    box.setOutlineThickness(2);
    box.setPosition(25, 75);

    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    text.setPosition(35, 85);

    promptText.setFont(font); // Initialize promptText with the font
    promptText.setCharacterSize(24);
    promptText.setFillColor(sf::Color::Black);
}

void TextBoxWindow::handleEvents()
{
    sf::Event event;
    while (textBoxWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            textBoxWindow.close();
        }
        if (event.type == sf::Event::TextEntered)
        {
            if (event.text.unicode == '\b') // Handle backspace
            {
                if (!input.empty())
                {
                    input.pop_back();
                }
            }
            else if (event.text.unicode < 128) // Handle regular input
            {
                input += static_cast<char>(event.text.unicode);
            }
            text.setString(input); // Update the text object with the new input
        }
    }
}

void TextBoxWindow::render()
{
    textBoxWindow.clear(sf::Color(86, 86, 135));

    // Draw the prompt text above the box
    textBoxWindow.draw(promptText);

    // Draw the input box and input text
    textBoxWindow.draw(box);
    textBoxWindow.draw(text);

    textBoxWindow.display();
}

void TextBoxWindow::run(std::string &userInput, const std::string &prompt)
{
    promptText.setString(prompt);
    promptText.setPosition(10, 30);
    // If userInput is not empty, set the displayed text to it
    if (!userInput.empty())
    {
        input = userInput; // Display the userInput if it is not empty
    }

    sf::Clock clock;
    while (textBoxWindow.isOpen())
    {
        handleEvents();
        render();

        // Exit the loop and save the user input if Enter is pressed after 2 seconds
        if (clock.getElapsedTime().asSeconds() > 2)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            {
                userInput = input;     // Save the entered text to userInput
                textBoxWindow.close(); // Close the text box window
            }
        }
    }
}