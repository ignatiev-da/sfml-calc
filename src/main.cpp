#include <SFML/Graphics.hpp>
#include <iostream>
#include "../include/Constants.h"
#include "../include/Calculator.h"

int main()
{
    // Создаем окно приложения с заданными параметрами
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), L"SFML Калькулятор");

    // Загружаем шрифт
    std::unique_ptr<sf::Font> font = std::make_unique<sf::Font>();
    if (!font->loadFromFile("../resources/fonts/arial.ttf"))
    {
        std::cerr << "Ошибка при загрузке шрифта!\n";
        return -1;
    }

    // Создаем экземпляр калькулятора, передавая ему шрифт
    Calculator calculator(*font);

    // Главный цикл приложения
    while (window.isOpen())
    {
        sf::Event event;

        // Обработка событий
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Обрабатываем события калькулятора
            calculator.handleEvent(event, window);
        }

        // Обновление кнопок
        for (auto &button : calculator.getButtons())
        {
            button->update();
        }

        // Обновляем экран
        window.clear(sf::Color::White);
        window.draw(calculator);
        window.display();
    }
    return 0;
}
