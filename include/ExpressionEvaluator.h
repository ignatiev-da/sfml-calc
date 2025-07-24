// Класс для вычисления математических выражений
// Реализует парсер математических формул

#pragma once
#include <string>
#include <string_view>
#include <stdexcept>
#include <charconv>
#include <cctype>

class ExpressionEvaluator
{
public:
    // Основная функция вычисления выражения
    static double evaluate(std::string_view expression)
    {
        size_t pos = 0;
        double result = parseExpression(expression, pos);

        // Проверяем, что выражение обработано полностью
        skipWhitespace(expression, pos);
        if (pos < expression.length())
        {
            throw std::invalid_argument("Неожиданный символ в выражении");
        }

        return result;
    }

private:
    // Обработка сложения и вычитания
    static double parseExpression(std::string_view expr, size_t &pos)
    {
        double result = parseTerm(expr, pos);

        while (pos < expr.length())
        {
            skipWhitespace(expr, pos);

            char op = expr[pos];
            if (op != '+' && op != '-')
                break;

            pos++;
            double term = parseTerm(expr, pos);
            result = (op == '+') ? result + term : result - term;
        }

        return result;
    }

    // Обработка умножения и деления
    static double parseTerm(std::string_view expr, size_t &pos)
    {
        double result = parseFactor(expr, pos);

        while (pos < expr.length())
        {
            skipWhitespace(expr, pos);

            char op = expr[pos];
            if (op != '*' && op != '/')
                break;

            pos++;
            double factor = parseFactor(expr, pos);

            if (op == '*')
                result *= factor;
            else if (factor == 0)
                throw std::invalid_argument("Деление на ноль!");
            else
                result /= factor;
        }

        return result;
    }

    // Обработка чисел и скобок
    static double parseFactor(std::string_view expr, size_t &pos)
    {
        skipWhitespace(expr, pos);

        if (pos >= expr.length())
            throw std::invalid_argument("Некорректное выражение");

        // Обработка скобок
        if (expr[pos] == '(')
        {
            pos++;
            double result = parseExpression(expr, pos);
            skipWhitespace(expr, pos);

            if (pos >= expr.length() || expr[pos] != ')')
                throw std::invalid_argument("Нет закрывающей скобки");

            pos++;
            return result;
        }

        // Обработка отрицательных чисел
        bool negative = false;
        if (expr[pos] == '-')
        {
            negative = true;
            pos++;
        }

        double result{};
        auto [ptr, ec] = std::from_chars(
            expr.data() + pos,
            expr.data() + expr.length(),
            result);

        if (ec != std::errc())
            throw std::invalid_argument("Некорректное число");

        pos = ptr - expr.data();

        return negative ? -result : result;
    }

    // Пропускает пробелы в выражении
    static void skipWhitespace(std::string_view expr, size_t &pos)
    {
        while (pos < expr.length() && std::isspace(static_cast<unsigned char>(expr[pos])))
            pos++;
    }
};
