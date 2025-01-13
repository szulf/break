#include "json.hpp"
#include <fstream>
#include <iostream>
#include <print>
#include <sstream>
#include <string>

static void printJson(const json::Json& json) {
    // Use std::visit to handle the variant
    std::visit([](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            std::println("null");
        } else if constexpr (std::is_same_v<T, bool>) {
            std::println("{}", (val ? "true" : "false"));
        } else if constexpr (std::is_same_v<T, int64_t>) {
            std::println("{}", val);
        } else if constexpr (std::is_same_v<T, double>) {
            std::println("{}", val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            std::println("\"{}\"", val);
        } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, json::Json>>) {
            std::print("{{ ");
            for (const auto& [key, value] : val) {
                std::print("\"{}\"", key);
                printJson(value);
                std::print(", ");
            }
            std::println(" }}");
        } else if constexpr (std::is_same_v<T, std::vector<json::Json>>) {
            std::print("[ ");
            for (const auto& item : val) {
                printJson(item);
                std::print(", ");
            }
            std::println("]");
        }
    }, json.value);
}

static auto load_file(const std::ifstream& file) -> std::string
{
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

int main(int argc, char** argv)
{
    std::ifstream file{"test.json"};
    std::string str = load_file(file);
    auto x = json::decode(str);

    printJson(x);

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
