#ifndef TEXTBOXWINDOW_H
#define TEXTBOXWINDOW_H

#include <SFML/Graphics.hpp>
#include <string>

class TextBoxWindow {
public:
    TextBoxWindow();
    void run(std::string& userInput, const std::string& prompt);  // Accept a string for the prompt
private:
    sf::RenderWindow textBoxWindow;
    sf::RectangleShape box;
    sf::Text text;
    sf::Text promptText;  // For the prompt text (e.g., "Enter your name:")
    sf::Font font;
    std::string input;

    void handleEvents();
    void render();
};

#endif // TEXTBOXWINDOW_H