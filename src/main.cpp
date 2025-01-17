#include "build.hpp"

#include <print>

int main(int argc, char** argv)
{
    auto p = build::parse_break_file();
    if (!p)
    {
        std::println("{}", p.error());
        return 1;
    }

    // std::println("project_name: {}", p.value().project_name);
    // for (const auto& file : p.value().source_files)
    //
    // {
    //     std::println("src_file: {}", file);
    // }
    // for (const auto& file : p.value().header_files)
    // {
    //     std::println("head_file: {}", file);
    // }
    // std::println("c_compiler: {}", p.value().c_compiler);
    // std::println("cpp_compiler: {}", p.value().cpp_compiler);
    // std::println("c_flags: {}", p.value().c_flags);
    // std::println("cpp_flags: {}", p.value().cpp_flags);
    // std::println("configs:");
    // for (const auto& config : p.value().configs)
    // {
    //     std::println("    {}: {}", config.config_name, config.config_flags);
    // }
    // std::println("export_compile_commands: {}", p.value().export_compile_commands);

    auto cmds = build::build_compile_commands(p.value(), p.value().configs[0]);
    for (const auto& cmd : cmds)
    {
        std::println("{}", cmd);
    }

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
