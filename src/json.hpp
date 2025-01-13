#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace json
{

struct Json
{
    std::variant<
        std::nullptr_t,
        bool,
        int64_t,
        double,
        std::string,
        std::unordered_map<std::string, Json>,
        std::vector<Json>
    > value;
};

auto decode(const std::string& source, size_t pos = 0) -> Json;
auto encode(const Json& json) -> std::string;

}
