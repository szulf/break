#include "build.hpp"
#include "json.hpp"

#include <format>
#include <fstream>
#include <sstream>
#include <print>

namespace build
{

auto parse_break_file() -> std::expected<BuildParams, std::string>
{
    std::ifstream file{"break.json"};
    std::stringstream ss;
    ss << file.rdbuf();
    std::string break_str = ss.str();

    auto x = json::decode(break_str);
    if (!x)
    {
        return std::unexpected<std::string>{x.error()};
    }

    std::unordered_map<std::basic_string<char>, json::Json> obj{};
    try
    {
        obj = std::get<std::unordered_map<std::string, json::Json>>(x.value().value);
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: root element needs to be an object"};
    }

    std::string project_name{};
    try
    {
        project_name = std::get<std::string>(obj["project_name"].value);
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'project_name' needs to be a string"};
    }

    std::vector<std::string> source_files{};
    std::vector<std::string> header_files{};
    try
    {
        std::vector<json::Json> source_temp{std::get<std::vector<json::Json>>(obj["source_files"].value)};
        for (const auto& temp : source_temp)
        {
            source_files.emplace_back(std::get<std::string>(temp.value));
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'source_files' needs to be an array of strings"};
    }

    try
    {
        if (obj.find("header_files") != obj.end())
        {
            std::vector<json::Json> header_temp{std::get<std::vector<json::Json>>(obj["header_files"].value)};
            for (const auto& temp : header_temp)
            {
                header_files.emplace_back(std::get<std::string>(temp.value));
            }
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'header_files' needs to be an array of strings"};
    }

    std::string c_compiler{};
    std::string cpp_compiler{};
    try
    {
        if (obj.find("c_compiler") != obj.end())
        {
            c_compiler = std::get<std::string>(obj["c_compiler"].value);
        }
        else
        {
            c_compiler = std::getenv("CC");
        }

        if (obj.find(cpp_compiler) != obj.end())
        {
            cpp_compiler = std::get<std::string>(obj["cpp_compiler"].value);
        }
        else
        {
            cpp_compiler = std::getenv("CXX");
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'c_compiler' and 'cpp_compiler' need to be strings"};
    }

    std::string c_flags{};
    std::string cpp_flags{};
    try
    {
        if (obj.find("c_flags") != obj.end())
        {
            c_flags = std::get<std::string>(obj["c_flags"].value);
        }

        if (obj.find("cpp_flags") != obj.end())
        {
            cpp_flags = std::get<std::string>(obj["cpp_flags"].value);
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'c_flags' and 'cpp_flags' need to be strings"};
    }

    std::vector<BuildConfig> configs{};
    try
    {
        if (obj.find("configs") != obj.end())
        {
            std::vector<json::Json> temp{std::get<std::vector<json::Json>>(obj["configs"].value)};

            for (const auto& t : temp)
            {
                auto config{std::get<std::unordered_map<std::string, json::Json>>(t.value)};
                if (config.size() != 1)
                {
                    throw std::runtime_error{"invalid config object size"};
                }

                auto obj_val{std::get<std::string>(config.begin()->second.value)};
                configs.emplace_back(BuildConfig{config.begin()->first, obj_val});
            }
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'configs' needs to be an array of objects of a single string"};
    }

    bool export_compile_commands{false};
    try
    {
        if (obj.find("export_compile_commands") != obj.end())
        {
            export_compile_commands = std::get<bool>(obj["export_compile_commands"].value);
        }
    }
    catch (std::exception e)
    {
        return std::unexpected<std::string>{"Invalid break.json formatting: 'export_compile_commands' needs to be a boolean"};
    }

    return BuildParams{
        .project_name = project_name,
        .source_files = source_files,
        .header_files = header_files,
        .c_compiler = c_compiler,
        .cpp_compiler = cpp_compiler,
        .c_flags = c_flags,
        .cpp_flags = cpp_flags,
        .configs = configs,
        .export_compile_commands = export_compile_commands
    };
}

// TODO
// only generate commands for files that have changed
auto build_compile_commands(const BuildParams& params, const std::optional<BuildConfig>& config) -> std::vector<std::string>
{
    std::vector<std::string> cmds{};

    for (const auto& src_file : params.source_files)
    {
        if (src_file.substr(src_file.size() - 2) == ".c")
        {
            std::string output_file{src_file + ".o"};
            output_file = output_file.substr(output_file.rfind("/"));
            output_file = "build/obj" + output_file;

            std::string cmd{std::format("{} -c {} {} -o {}", params.c_compiler, params.c_flags, src_file, output_file)};

            if (config)
            {
                cmd += " " + config->config_flags;
            }

            cmds.emplace_back(cmd);
        }
        else
        {
            std::string output_file{src_file + ".o"};
            output_file = output_file.substr(output_file.rfind("/"));
            output_file = "build/obj" + output_file;

            std::string cmd{std::format("{} -c {} {} -o {}", params.cpp_compiler, params.cpp_flags, src_file, output_file)};

            if (config)
            {
                cmd += " " + config->config_flags;
            }

            cmds.emplace_back(cmd);
        }
    }

    return cmds;
}

};
