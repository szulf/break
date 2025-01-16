#include "json.hpp"
#include <print>

namespace json
{

auto print_json(const json::Json& json) -> void
{
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
            for (size_t i = 0; const auto& [key, value] : val)
            {
                std::print("\"{}\": ", key);
                print_json(value);
                if (i + 1 != val.size())
                {
                    std::print(",");
                }
                std::print(" ");
                i++;
            }
            std::print(" }}");
        }
        else if constexpr (std::is_same_v<T, std::vector<json::Json>>)
        {
            std::print("[ ");
            for (size_t i = 0; const auto& item : val)
            {
                print_json(item);
                if (i + 1 != val.size())
                {
                    std::print(",");
                }
                i++;
                std::print(" ");
            }
            std::print("]");
        }
    }, json.value);
}

static inline auto parse_number(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    size_t start{pos};
    bool is_negative{false};

    if (source[pos] == '.')
    {
        return std::unexpected<std::string>{std::format("Invalid number format: '.' at the start, around line {}", line)};
    }

    if (source[pos] == '-')
    {
        pos += 1;
        is_negative = true;
    }

    if (std::isdigit(source[pos]))
    {
        while (std::isdigit(source[pos]))
        {
            pos += 1;
        }
    }
    else
    {
        return std::unexpected<std::string>{std::format("Invalid number format: no digits after '-', around line {}", line)};
    }

    if (source[pos] == '.')
    {
        pos += 1;
    }
    else
    {
        if (is_negative)
        {
            if (source[start + 1] == '0' && start + 2 != pos)
            {
                return std::unexpected<std::string>{std::format("Invalid number format: '0' at the start, around line {}", line)};
            }
        }
        else
        {
            if (source[start] == '0' && start + 1 != pos)
            {
                return std::unexpected<std::string>{std::format("Invalid number format: '0' at the start, around line {}", line)};
            }
        }

        return std::expected<Json, std::string>{std::stoll(source.substr(start, pos))};
    }

    if (std::isdigit(source[pos]))
    {
        while (std::isdigit(source[pos]))
        {
            pos += 1;
        }
    }
    else
    {
        return std::unexpected<std::string>{std::format("Invalid number format: no digits after '.', around line {}", line)};
    }

    if (is_negative)
    {
        if (source[start + 1] == '0' && source[start + 2] != '.')
        {
            return std::unexpected<std::string>{std::format("Invalid number format: '0' at the start, around line {}", line)};
        }
    }
    else
    {
        if (source[start] == '0' && source[start + 1] != '.')
        {
            return std::unexpected<std::string>{std::format("Invalid number format: '0' at the start, around line {}", line)};
        }

    }

    return std::expected<Json, std::string>{std::stod(source.substr(start, pos))};
}

static inline auto parse_bool(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    if (source.substr(pos, 4) == "true")
    {
        pos += 4;
        return std::expected<Json, std::string>{true};
    }

    if (source.substr(pos, 5) == "false")
    {
        pos += 5;
        return std::expected<Json, std::string>{false};
    }

    return std::unexpected<std::string>{std::format("Invalid boolean format, around line {}", line)};
}

static inline auto parse_null(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    if (source.substr(pos, 4) == "null")
    {
        pos += 4;
        return std::expected<Json, std::string>{nullptr};
    }

    return std::unexpected<std::string>{std::format("invalid null format, around line {}", line)};
}

static inline auto parse_string(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    size_t start{pos + 1};
    pos = source.find("\"", start);

    if (pos == std::string::npos)
    {
        return std::unexpected<std::string>{std::format("Invalid string format: missing ending '\"', around line {}", line)};
    }

    if (source.substr(start - 1, pos - start + 2).contains('\n'))
    {
        return std::unexpected<std::string>{std::format("Invalid string format: multiline strings are not allowed, around line {}", line)};
    }

    pos += 1;
    return std::expected<Json, std::string>{source.substr(start, pos - start - 1)};
}

static auto skip_whitespace(const std::string& source, size_t& pos, size_t& line) -> void
{
    while (std::isspace(source[pos]))
    {
        if (source[pos] == '\n')
        {
            line += 1;
        }

        pos += 1;
    }
}

static inline auto parse_array(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    std::vector<Json> jsons{};

    pos += 1;

    skip_whitespace(source, pos, line);
    if (source[pos] == ']')
    {
        pos += 1;
        return std::expected<Json, std::string>{jsons};
    }

    while (source[pos] != ']')
    {
        skip_whitespace(source, pos, line);

        auto temp = decode(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        jsons.emplace_back(temp.value());
        skip_whitespace(source, pos, line);

        if (source[pos] == ',')
        {
            pos += 1;
            skip_whitespace(source, pos, line);
            if (source[pos] == ']')
            {
                return std::unexpected<std::string>{std::format("Invalid array format: trailing ',', around line {}", line)};
            }
        }
        else
        {
            skip_whitespace(source, pos, line);
            if (source[pos] == ']')
            {
                pos += 1;
                break;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Invalid array format: missing ',' after element, around line {}", line)};
            }
        }
    }

    return std::expected<Json, std::string>{jsons};
}

static inline auto parse_object(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    std::unordered_map<std::string, Json> jsons{};

    pos += 1;

    skip_whitespace(source, pos, line);
    if (source[pos] == '}')
    {
        pos += 1;
        return std::expected<Json, std::string>{jsons};
    }

    while (source[pos] != '}')
    {
        std::string obj_key{};

        skip_whitespace(source, pos, line);

        if (source[pos] == '"')
        {
            auto temp = parse_string(source, pos, line);
            if (!temp.has_value())
            {
                return temp;
            }
            obj_key = std::get<std::string>(temp.value().value);
        }
        else
        {
            return std::unexpected<std::string>{std::format("Invalid object format: missing key, around line {}", line)};
        }

        skip_whitespace(source, pos, line);

        if (source[pos] == ':')
        {
            pos += 1;
        }
        else
        {
            return std::unexpected<std::string>{std::format("Invalid object format: missing ':' after key, around line {}", line)};
        }

        skip_whitespace(source, pos, line);

        auto obj_val = decode(source, pos, line);
        if (!obj_val.has_value())
        {
            return obj_val;
        }

        if (jsons.contains(obj_key))
        {
            return std::unexpected<std::string>{std::format("Invalid object format: duplicate key, around line {}", line)};
        }

        jsons.emplace(obj_key, obj_val.value());

        if (source[pos] == ',')
        {
            pos += 1;
            skip_whitespace(source, pos, line);
            if (source[pos] == '}')
            {
                return std::unexpected<std::string>{std::format("Invalid object format: trailing ',', around line {}", line)};
            }
        }
        else
        {
            skip_whitespace(source, pos, line);
            if (source[pos] == '}')
            {
                pos += 1;
                break;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Invalid object format: missing ',' after entry, around line {}", line)};
            }
        }
    }

    return std::expected<Json, std::string>{jsons};
}

auto decode(const std::string& source) -> std::expected<Json, std::string>
{
    size_t pos = 0;
    size_t line = 1;
    return decode(source, pos, line);
}

auto decode(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>
{
    bool is_first{pos == 0};

    // object
    if (source[pos] == '{')
    {
        auto temp = parse_object(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(object), around line {}", line)};
            }
        }
        return temp;
    }

    // array
    if (source[pos] == '[')
    {
        auto temp = parse_array(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(array), around line {}", line)};
            }
        }
        return temp;
    }

    // string
    if (source[pos] == '"')
    {
        auto temp = parse_string(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(string), around line {}", line)};
            }
        }
        return temp;
    }

    // number
    if (source[pos] == '.' || source[pos] == '-' || std::isdigit(source[pos]))
    {
        auto temp = parse_number(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(number), around line {}", line)};
            }
        }
        return temp;
    }

    // bool
    if (source.substr(pos, 4) == "true" || source.substr(pos, 5) == "false")
    {
        auto temp = parse_bool(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(bool), around line {}", line)};
            }
        }
        return temp;
    }

    // null
    if (source.substr(pos, 4) == "null")
    {
        auto temp = parse_null(source, pos, line);
        if (!temp.has_value())
        {
            return temp;
        }

        if (is_first)
        {
            if (pos + 1 == source.length())
            {
                return temp;
            }
            else
            {
                return std::unexpected<std::string>{std::format("Extra elements after root element(null), around line {}", line)};
            }
        }
        return temp;
    }

    return std::unexpected<std::string>{std::format("Invalid json syntax, around line {}", line)};
}

auto encode(const Json& json) -> std::string
{
    return {};
}

}
