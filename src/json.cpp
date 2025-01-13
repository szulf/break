#include "json.hpp"
#include <cctype>
#include <print>
#include <stdexcept>

namespace json
{

static auto parse_number(const std::string& source, size_t& pos) -> Json
{
    size_t start{pos};
    bool is_negative{false};

    if (source[pos] == '.')
    {
        throw std::runtime_error{"Invalid number format: '.' at the start"};
    }

    if (source[pos] == '-')
    {
        pos++;
        is_negative = true;
    }

    if (std::isdigit(source[pos]))
    {
        while (std::isdigit(source[pos]))
        {
            pos++;
        }
    }
    else
    {
        throw std::runtime_error{"Invalid number format: no digits after '-'"};
    }

    if (source[pos] == '.')
    {
        pos++;
    }
    else
    {
        if (is_negative)
        {
            if (source[start + 1] == '0' && start + 2 != pos)
            {
                throw std::runtime_error{"Invalid number format: '0' at the start"};
            }
        }
        else
        {
            if (source[start] == '0' && start + 1 != pos)
            {
                throw std::runtime_error{"Invalid number format: '0' at the start"};
            }
        }

        return Json{std::stoi(source.substr(start, pos))};
    }

    if (std::isdigit(source[pos]))
    {
        while (std::isdigit(source[pos]))
        {
            pos++;
        }
    }
    else
    {
        throw std::runtime_error{"Invalid number format: no digits after '.'"};
    }

    if (!std::isspace(source[pos]) && source[pos] != ',')
    {
        throw std::runtime_error{"Invalid number format: unexpected character after last number"};
    }

    if (is_negative)
    {
        if (source[start + 1] == '0' && source[start + 2] != '.')
        {
            throw std::runtime_error{"Invalid number format: '0' at the start"};
        }
    }
    else
    {
        if (source[start] == '0' && source[start + 1] != '.')
        {
            throw std::runtime_error{"Invalid number format: '0' at the start"};
        }

    }

    return Json{std::stod(source.substr(start, pos))};
}

auto decode(const std::string& source, size_t pos) -> Json
{
    // while (pos + 1 < source.length())
    // {
    // object
    if (source[pos] == '{')
    {

    }

    // array
    if (source[pos] == '[')
    {

    }

    // string
    if (source[pos] == '"')
    {

    }

    // number
    if (source[pos] == '.' || source[pos] == '-' || std::isdigit(source[pos]))
    {
        return parse_number(source, pos);
    }

    // bool
    if (source.substr(pos, 4) == "true" || source.substr(pos, 5) == "false")
    {

    }

    // null
    if (source.substr(pos, 4) == "null")
    {

    }

    //     pos++;
    // }

    return {};
}

auto encode(const Json& json) -> std::string
{
    return {};
}

}
