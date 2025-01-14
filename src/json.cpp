#include "json.hpp"
#include <cctype>
#include <print>
#include <stdexcept>

namespace json
{

static inline auto parse_number(const std::string& source, size_t& pos) -> Json
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

static inline auto parse_bool(const std::string& source, size_t& pos) -> Json
{
    if (source.substr(pos, 4) == "true")
    {
        pos += 4;
        return Json{true};
    }

    if (source.substr(pos, 5) == "false")
    {
        pos += 5;
        return Json{false};
    }

    throw std::runtime_error{"Invalid boolean format"};
}

static inline auto parse_null(const std::string& source, size_t& pos) -> Json
{
    if (source.substr(pos, 4) == "null")
    {
        pos += 4;
        return Json{nullptr};
    }

    throw std::runtime_error{"invalid null format"};
}

static inline auto parse_string(const std::string& source, size_t& pos) -> Json
{
    size_t start{pos + 1};
    pos = source.find("\"", start);
    if (pos == std::string::npos)
    {
        throw std::runtime_error{"Invalid string format: missing ending '\"'"};
    }

    pos += 1;
    return Json{source.substr(start, pos - start - 1)};
}

auto decode(const std::string& source, size_t pos) -> Json
{
    bool is_first{pos == 0};

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
        Json temp = parse_string(source, pos);
        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                throw std::runtime_error{"Invalid json syntax: string"};
            }
        }
        return temp;
    }

    // number
    if (source[pos] == '.' || source[pos] == '-' || std::isdigit(source[pos]))
    {
        Json temp = parse_number(source, pos);
        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                throw std::runtime_error{"Invalid json syntax: number"};
            }
        }
        return temp;
    }

    // bool
    if (source.substr(pos, 4) == "true" || source.substr(pos, 5) == "false")
    {
        Json temp = parse_bool(source, pos);
        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                throw std::runtime_error{"Invalid json syntax: bool"};
            }
        }
        return temp;
    }

    // null
    if (source.substr(pos, 4) == "null")
    {
        Json temp = parse_null(source, pos);
        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                throw std::runtime_error{"Invalid json syntax: null"};
            }
        }
        return temp;
    }

    throw std::runtime_error{"Invalid json syntax"};
}

auto encode(const Json& json) -> std::string
{
    return {};
}

}
