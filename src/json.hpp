#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <expected>

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

auto decode(const std::string& source) -> std::expected<Json, std::string>;
auto decode(const std::string& source, size_t& pos, size_t& line) -> std::expected<Json, std::string>;

// TODO
// maybe could use print_json for this
// idk if i want indentation
// might make a print_json version with indentation tbh
auto encode(const Json& json) -> std::string;

}
