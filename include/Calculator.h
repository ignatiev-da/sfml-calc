// Класс калькулятора
// Объединяет все компоненты интерфейса и логику работы

#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>
#include "Constants.h"
#include "Button.h"
#include "ExpressionEvaluator.h"

class Calculator : public sf::Drawable, public sf::Transformable
{
public:
    // Конструктор калькулятора
    explicit Calculator(sf::Font &font) : isResult(false)
    {
        // Создаем область отображения
        display = std::make_unique<sf::RectangleShape>(sf::Vector2f(350, 50));
        display->setPosition(20, 20);
        display->setFillColor(sf::Color(173, 216, 230));
        display->setOutlineColor(sf::Color::Black);
        display->setOutlineThickness(2);

        // Создаем текстовое поле для отображения
        displayText = std::make_unique<sf::Text>("", font, 30);
        displayText->setPosition(30, 30);
        displayText->setFillColor(sf::Color::Black);

        // Создаем фон окна
        windowBackground = std::make_unique<sf::RectangleShape>(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
        windowBackground->setFillColor(sf::Color::White);

        // Цвета фона разных типов кнопок
        const sf::Color defaultColor = sf::Color(200, 200, 200);  // Кнопки цифр
        const sf::Color operationColor = sf::Color(90, 160, 245); // Кнопки операций
        const sf::Color clearColor = sf::Color(255, 120, 120);    // Кнопки очистки и удаления
        const sf::Color resultColor = sf::Color(90, 210, 150);    // Кнопка результата

        // Определяем метки для кнопок
        const std::vector<std::string> labels = {
            "7", "8", "9", "/",
            "4", "5", "6", "*",
            "1", "2", "3", "-",
            "0", "(", ")", "+",
            "C", "<", "="};

        // Создаем кнопки
        buttons.reserve(labels.size());
        for (size_t i = 0; i < labels.size(); ++i)
        {
            sf::Color color = defaultColor;

            // Цвет фона кнопки в зависимости от текста
            if (labels[i] == "=")
            {
                color = resultColor;
            }
            else if (labels[i] == "C" || labels[i] == "<")
            {
                color = clearColor;
            }
            else if (labels[i] == "/" || labels[i] == "*" ||
                     labels[i] == "-" || labels[i] == "+" ||
                     labels[i] == "(" || labels[i] == ")")
            {
                color = operationColor;
            }

            buttons.emplace_back(std::make_unique<Button>(
                labels[i], font, 24,
                sf::Vector2f(20 + (i % 4) * 90, 100 + (i / 4) * 90),
                sf::Vector2f(80, 80), color));
        }

        // Загружаем текстуру логотипа
        logoTexture = std::make_unique<sf::Texture>();
        if (!logoTexture->loadFromFile("../resources/images/logo.png"))
        {
            std::cerr << "Ошибка при загрузке логотипа!" << std::endl;
        }

        // Создаем спрайт и устанавливаем его позицию
        logoSprite.setTexture(*logoTexture);
        logoSprite.setPosition(305, 475);
    }

    // Обработка событий мыши и клавиатуры
    void handleEvent(const sf::Event &event, sf::RenderWindow &window)
    {
        if (event.type == sf::Event::MouseButtonPressed &&
            event.mouseButton.button == sf::Mouse::Left)
        {
            // Проверяем нажатие на кнопки
            for (auto &button : buttons)
            {
                if (button->getGlobalBounds().contains(
                        static_cast<sf::Vector2f>(sf::Mouse::getPosition(window))))
                {
                    button->pressEffect();           // Применяем эффект нажатия
                    processInput(button->getText()); // Обрабатываем ввод
                    displayText->setString(input);   // Обновляем отображение
                }
            }
        }
        else if (event.type == sf::Event::MouseButtonReleased &&
                 event.mouseButton.button == sf::Mouse::Left)
        {
            // Снимаем эффекты нажатия
            for (auto &button : buttons)
            {
                button->releaseEffect();
            }
        }
        else if (event.type == sf::Event::KeyPressed)
        {
            processKeyboardInput(event);
        }
        else if (event.type == sf::Event::KeyReleased)
        {
            // Снимаем эффекты нажатия
            for (auto &button : buttons)
            {
                button->releaseEffect();
            }
        }
    }

    std::vector<std::unique_ptr<Button>> &getButtons()
    {
        return buttons;
    }

private:
    // Метод отрисовки калькулятора
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(*windowBackground, states); // Отрисовываем фон
        target.draw(*display, states);          // Отрисовываем дисплей
        target.draw(*displayText, states);      // Отрисовываем текст на дисплее
        for (const auto &button : buttons)      // Отрисовываем все кнопки
        {
            target.draw(*button, states);
        }
        target.draw(logoSprite, states);
    }

    // Обработка ввода данных
    void processInput(const std::string &text)
    {
        // Проверяем, если был результат и нажата цифра или скобки
        if (isResult && (std::isdigit(text[0]) || text == "(" || text == ")"))
        {
            input.clear();
            isResult = false;
        }

        if (text == "C") // Очистка ввода
        {
            input.clear();
            isResult = false;
        }
        else if (text == "=") // Вычисление результата
        {
            try
            {
                double result = ExpressionEvaluator::evaluate(input);
                input = std::to_string(result);
                // Убираем лишние нули после точки
                /* if (input.ends_with(".000000"))
                {
                    input = input.substr(0, input.find('.'));
                } */
                if (input.find('.') != std::string::npos)
                {
                    input.erase(input.find_last_not_of('0') + 1, std::string::npos);
                    if (input.back() == '.')
                        input.pop_back();
                }
                isResult = true;
            }
            catch (const std::exception &)
            {
                input = "Error"; // Обработка ошибок
                isResult = false;
            }
        }
        else if (text == "<") // Удаление последнего символа
        {
            if (!input.empty() && input != "Error")
            {
                input.pop_back();
                isResult = false;
            }
        }
        else if (text == "+" || text == "-" || text == "*" || text == "/")
        {
            // Обработка операторов
            std::string_view ops = "+-*/";
            if (!input.empty() && input != "Error" && ops.find(input.back()) == std::string_view::npos && input.length() < 19)
            {
                input += text;
                isResult = false;
            }
        }
        else if (text == "(" || text == ")")
        {
            // Обработка скобок
            if (input != "Error" && input.length() < 19)
            {
                input += text;
                isResult = false;
            }
        }
        else if (text.length() == 1 && std::isdigit(text[0]))
        {
            // Добавление цифр
            if (input != "Error" && input.length() < 19)
            {
                input += text;
            }
        }
        displayText->setString(input); // Обновление отображения
    }

    // Обработка ввода данных от клавиатуры
    void processKeyboardInput(const sf::Event &event)
    {
        switch (event.key.code)
        {
        case sf::Keyboard::Num0:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[14]->pressEffect();
                processInput(")");
            }
            else
            {
                buttons[12]->pressEffect();
                processInput("0");
            }
            break;

        case sf::Keyboard::Numpad0:
            buttons[12]->pressEffect();
            processInput("0");
            break;

        case sf::Keyboard::Num1:
        case sf::Keyboard::Numpad1:
            buttons[8]->pressEffect();
            processInput("1");
            break;

        case sf::Keyboard::Num2:
        case sf::Keyboard::Numpad2:
            buttons[9]->pressEffect();
            processInput("2");
            break;

        case sf::Keyboard::Num3:
        case sf::Keyboard::Numpad3:
            buttons[10]->pressEffect();
            processInput("3");
            break;

        case sf::Keyboard::Num4:
        case sf::Keyboard::Numpad4:
            buttons[4]->pressEffect();
            processInput("4");
            break;

        case sf::Keyboard::Num5:
        case sf::Keyboard::Numpad5:
            buttons[5]->pressEffect();
            processInput("5");
            break;

        case sf::Keyboard::Num6:
        case sf::Keyboard::Numpad6:
            buttons[6]->pressEffect();
            processInput("6");
            break;

        case sf::Keyboard::Num7:
        case sf::Keyboard::Numpad7:
            buttons[0]->pressEffect();
            processInput("7");
            break;

        case sf::Keyboard::Num8:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[7]->pressEffect();
                processInput("*");
            }
            else
            {
                buttons[1]->pressEffect();
                processInput("8");
            }
            break;

        case sf::Keyboard::Numpad8:
            buttons[1]->pressEffect();
            processInput("8");
            break;

        case sf::Keyboard::Num9:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[13]->pressEffect();
                processInput("(");
            }
            else
            {
                buttons[2]->pressEffect();
                processInput("9");
            }
            break;

        case sf::Keyboard::Numpad9:
            buttons[2]->pressEffect();
            processInput("9");
            break;

        case sf::Keyboard::Add:
            buttons[15]->pressEffect();
            processInput("+");
            break;

        case sf::Keyboard::Equal:
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[15]->pressEffect();
                processInput("+");
            }
            break;

        case sf::Keyboard::Subtract:
            buttons[11]->pressEffect();
            processInput("-");
            break;

        case sf::Keyboard::Hyphen:
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[11]->pressEffect();
                processInput("-");
            }
            break;

        case sf::Keyboard::Multiply:
            buttons[7]->pressEffect();
            processInput("*");
            break;

        case sf::Keyboard::Divide:
            buttons[3]->pressEffect();
            processInput("/");
            break;

        case sf::Keyboard::Slash:
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && !sf::Keyboard::isKeyPressed(sf::Keyboard::RShift))
            {
                buttons[3]->pressEffect();
                processInput("/");
            }
            break;

        case sf::Keyboard::Escape:
            buttons[16]->pressEffect();
            processInput("C");
            break;

        case sf::Keyboard::BackSpace:
            buttons[17]->pressEffect();
            processInput("<");
            break;

        case sf::Keyboard::Return:
            buttons[18]->pressEffect();
            processInput("=");
            break;

        default:
            break;
        }
    }

    std::unique_ptr<sf::RectangleShape> windowBackground; // Фон окна
    std::unique_ptr<sf::RectangleShape> display;          // Дисплей
    std::unique_ptr<sf::Text> displayText;                // Текст на дисплее
    std::vector<std::unique_ptr<Button>> buttons;         // Вектор кнопок
    std::unique_ptr<sf::Texture> logoTexture;             // Текстура для лого
    sf::Sprite logoSprite;                                // Спрайт для лого

    std::string input; // Состояние калькулятора
    bool isResult;     // Флаг состояния результата
};
