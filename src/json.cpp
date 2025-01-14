#include "json.hpp"
#include <cctype>
#include <print>
#include <stdexcept>

// TODO
// change all std::runtime_errors into your own json_parsing_errors

namespace json
{

auto print_json(const json::Json& json) -> void
{
    // Use std::visit to handle the variant
    std::visit([](const auto& val)
    {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>)
        {
            std::print("null");
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            std::print("{}", (val ? "true" : "false"));
        }
        else if constexpr (std::is_same_v<T, int64_t>)
        {
            std::print("{}", val);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            std::print("{}", val);
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            std::print("\"{}\"", val);
        }
        else if constexpr (std::is_same_v<T, std::unordered_map<std::string, json::Json>>)
        {
            std::print("{{ ");
            for (const auto& [key, value] : val)
            {
                std::print("\"{}\"", key);
                print_json(value);
                std::print(", ");
            }
            std::print(" }}");
        }
        else if constexpr (std::is_same_v<T, std::vector<json::Json>>)
        {
            std::print("[ ");
            for (const auto& item : val)
            {
                print_json(item);
                // sucks that it prints a trailing comma, but i aint gon fix it now
                std::print(", ");
            }
            std::print("]");
        }
    }, json.value);
}

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

        return Json{std::stoll(source.substr(start, pos))};
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

// TODO
// json doesnt allow multiline strings
// this code does
// fix that
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

static auto skip_whitespace(const std::string& source, size_t& pos) -> void
{
    while (std::isspace(source[pos]))
    {
        pos += 1;
    }
}

static inline auto parse_array(const std::string& source, size_t& pos) -> Json
{
    std::vector<Json> jsons{};

    pos += 1;

    while (source[pos] != ']')
    {
        skip_whitespace(source, pos);
        jsons.emplace_back(decode(source, pos));
        skip_whitespace(source, pos);
        if (source[pos] == ',')
        {
            pos += 1;
            skip_whitespace(source, pos);
            if (source[pos] == ']')
            {
                throw std::runtime_error{"Invalid array format: trailing ','"};
            }
        }
        else
        {
            skip_whitespace(source, pos);
            if (source[pos] == ']')
            {
                pos += 1;
                break;
            }
            else
            {
                throw std::runtime_error{"Invalid array format: missing ',' after an element"};
            }
        }
    }

    pos += 1;

    return Json{jsons};
}

auto decode(const std::string& source) -> Json
{
    size_t pos = 0;
    return decode(source, pos);
}

auto decode(const std::string& source, size_t& pos) -> Json
{
    bool is_first{pos == 0};

    // object
    if (source[pos] == '{')
    {

    }

    // array
    if (source[pos] == '[')
    {
        Json temp = parse_array(source, pos);
        // if (is_first)
        // {
        //     if (pos + 1 == source.length())
        //     {
        //         return temp;
        //     }
        //     else
        //     {
        //         throw std::runtime_error{"Invalid json syntax: array"};
        //     }
        // }
        return temp;
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
