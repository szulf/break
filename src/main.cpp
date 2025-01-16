#include "json.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <stdexcept>
#include <string>

static auto load_file(const std::filesystem::path& path) -> std::string
{
    std::ifstream file{path};
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

struct BuildConfig
{
    std::string config_name;
    std::string config_flags;
};

struct BuildParams
{
    std::string project_name{};
    std::vector<std::string> source_files{};
    std::vector<std::string> header_files{};

    std::string c_compiler{};
    std::string cpp_compiler{};

    std::string c_flags{};
    std::string cpp_flags{};

    std::vector<BuildConfig> configs{};

    bool export_compile_commands{};
};

auto parse_break_file() -> std::expected<BuildParams, std::string>
{
    auto x = json::decode(load_file("break.json"));
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

    json::Json json_temp{};

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

int main(int argc, char** argv)
{
    // std::string str = load_file("break.json");
    // auto x = json::decode(str);
    // if (!x)
    // {
    //     std::println("{}", x.error());
    //     return 1;
    // }
    //
    // json::print_json(x.value());
    // std::println();

    auto p = parse_break_file();
    if (!p)
    {
        std::println("{}", p.error());
        return 1;
    }

    std::println("project_name: {}", p.value().project_name);
    for (const auto& file : p.value().source_files)

    {
        std::println("src_file: {}", file);
    }
    for (const auto& file : p.value().header_files)
    {
        std::println("head_file: {}", file);
    }
    std::println("c_compiler: {}", p.value().c_compiler);
    std::println("cpp_compiler: {}", p.value().cpp_compiler);
    std::println("c_flags: {}", p.value().c_flags);
    std::println("cpp_flags: {}", p.value().cpp_flags);
    std::println("configs:");
    for (const auto& config : p.value().configs)
    {
        std::println("    {}: {}", config.config_name, config.config_flags);
    }
    std::println("export_compile_commands: {}", p.value().export_compile_commands);

    // std::vector<std::string> args{argv, argv + argc};
    //
    // if (argc < 2)
    // {
    //     std::println("Not enough arguments");
    //     std::println("Use 'break help' for help");
    //     return 1;
    // }
    //
    // if (args[1] == "help")
    // {
    //     if (argc == 2)
    //     {
    //         std::println("BREAK - an attempt at a better make\n");
    //         std::println("Commands:\n");
    //         std::println("init     start a new break project\n");
    //         std::println("build    build all of the files\n");
    //     }
    //     else
    //     {
    //         if (args[2] == "init")
    //         {
    //             std::println("usage: break init project-name\n\n");
    //             // TODO
    //             // add some more here
    //             // like a little description
    //         }
    //     }
    // }
    // else if (args[1] == "init")
    // {
    //     if (argc != 3)
    //     {
    //         std::println(stderr, "Not enough arguments");
    //         std::println(stderr, "Use 'break help init' for help");
    //         return 1;
    //     }
    //
    //     // if (std::filesystem::exists("break.json"))
    //     // {
    //     //     std::println("'break.json' file already exists");
    //     //     std::println("please delete it to run this command");
    //     // }
    //
    //     std::ofstream breakfile{"break.json"};
    //
    //     std::println(breakfile,
    //             "{{\n"
    //             "    \"project_name\": \"{}\",\n"
    //             "    \"source_files\": [ \"src/main.cpp\" ],\n\n"
    //             "    \"configs\": [\n"
    //             "        {{ \"debug\": \"-O0 -g -Wall -Werror\" }},\n"
    //             "        {{ \"release\": \"-O3 -DNDEBUG\" }},\n"
    //             "    ],\n"
    //             "}}"
    //             , args[2]);
    // }
    // else if (args[1] == "build")
    // {
    //     // if (!std::filesystem::exists("build"))
    //     // {
    //     //     std::filesystem::create_directory("build");
    //     // }
    // }

    return 0;
}
