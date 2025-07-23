// Класс кнопки интерфейса
// Представляет собой интерактивный элемент управления

#pragma once
#include <string>
#include <memory>
#include <SFML/Graphics.hpp>

class Button : public sf::Drawable, public sf::Transformable
{
public:
    // Конструктор кнопки
    Button(std::string text, sf::Font &font, unsigned int characterSize,
           sf::Vector2f position, sf::Vector2f size, sf::Color buttonColor)
        : m_rect{std::make_unique<sf::RectangleShape>(size)},
          m_text{std::make_unique<sf::Text>(text, font, characterSize)},
          originalColor(buttonColor)
    {
        // Настройка геометрии кнопки
        m_rect->setPosition(position);
        m_rect->setFillColor(buttonColor);
        m_rect->setOutlineColor(sf::Color::Black);
        m_rect->setOutlineThickness(2);

        pressedColor = sf::Color(
            std::max(buttonColor.r - 50, 0),
            std::max(buttonColor.g - 50, 0),
            std::max(buttonColor.b - 50, 0),
            buttonColor.a);

        // Центрирование текста
        sf::FloatRect textBounds = m_text->getLocalBounds();
        m_text->setOrigin(textBounds.left + textBounds.width / 2.0f,
                          textBounds.top + textBounds.height / 2.0f);
        m_text->setPosition(position.x + size.x / 2.0f,
                            position.y + size.y / 2.0f);
        m_text->setFillColor(sf::Color::White);
    }

    // Эффект нажатия
    void pressEffect()
    {
        isPressed = true;
        alpha = 1.0f;
        m_rect->setOutlineColor(sf::Color(150, 150, 150));
        m_text->setFillColor(sf::Color::Black);
    }

    // Эффект отпускания
    void releaseEffect()
    {
        isPressed = false;
        alpha = 1.0f;
        m_rect->setOutlineColor(sf::Color::Black);
        m_text->setFillColor(sf::Color::White);
    }

    // Обновление фона кнопки
    void update()
    {
        if (isPressed)
        {
            alpha += 0.1f;
            if (alpha > 1.0f)
                alpha = 1.0f;
        }
        else
        {
            alpha -= 0.1f;
            if (alpha < 0.0f)
                alpha = 0.0f;
        }

        // Текущий цвет
        sf::Color currentColor = sf::Color(
            originalColor.r + (pressedColor.r - originalColor.r) * alpha,
            originalColor.g + (pressedColor.g - originalColor.g) * alpha,
            originalColor.b + (pressedColor.b - originalColor.b) * alpha,
            originalColor.a);

        m_rect->setFillColor(currentColor);
    }

    // Получение текста кнопки
    std::string getText() const
    {
        return m_text->getString();
    }

    // Получение границ кнопки
    sf::FloatRect getGlobalBounds() const
    {
        return getTransform().transformRect(m_rect->getGlobalBounds());
    }

private:
    // Метод отрисовки кнопки
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override
    {
        states.transform *= getTransform();
        target.draw(*m_rect, states); // Отрисовываем прямоугольник
        target.draw(*m_text, states); // Отрисовываем текст
    }

    // Компоненты кнопки
    std::unique_ptr<sf::RectangleShape> m_rect; // Прямоугольник кнопки
    std::unique_ptr<sf::Text> m_text;           // Текст на кнопке
    sf::Color originalColor;                    // Исходный цвет
    sf::Color pressedColor;                     // Цвет при нажатии
    bool isPressed = false;                     // Состояние нажатия
    float alpha = 1.0f;                         // Прозрачность
};
