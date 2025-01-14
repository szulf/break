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

auto print_json(const json::Json& json) -> void;
auto decode(const std::string& source) -> Json;
auto decode(const std::string& source, size_t& pos) -> Json;
auto encode(const Json& json) -> std::string;

}
