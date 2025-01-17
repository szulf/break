#pragma once

#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace build
{

struct BuildConfig
{
    const std::string config_name;
    const std::string config_flags;
};

struct BuildParams
{
    const std::string project_name{};
    const std::vector<std::string> source_files{};
    const std::vector<std::string> header_files{};

    const std::string c_compiler{};
    const std::string cpp_compiler{};

    const std::string c_flags{};
    const std::string cpp_flags{};

    const std::vector<BuildConfig> configs{};

    const bool export_compile_commands{};
};

auto parse_break_file() -> std::expected<BuildParams, std::string>;
auto build_compile_commands(const BuildParams& params, const std::optional<BuildConfig>& config) -> std::vector<std::string>;

};
