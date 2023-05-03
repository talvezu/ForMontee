// #define FMT_HEADER_ONLY // may need this line

#include <filesystem>
#include <fstream>
#include <iostream>

#include <yaml-cpp/yaml.h>
//#include <yaml.h>
#include <cxxopts.hpp>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "PeriodicTask.h"
#include "config.hpp"
#include "my_lib.h"
#include "yaml_api.h"
#include "TasksFromConfig.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

using YAML::Parser;
int main(int argc, char **argv)
{

    //Parser parser;
    workflow<float> f("text.yaml");
    f.init();
    tasks_from_config(f);

   	//std::this_thread::sleep_for(10000ms);
	//end_task.store(true);

    std::cout << "JSON: " << NLOHMANN_JSON_VERSION_MAJOR << "."
              << NLOHMANN_JSON_VERSION_MINOR << "."
              << NLOHMANN_JSON_VERSION_PATCH << '\n';
    std::cout << "FMT: " << FMT_VERSION << '\n';
    std::cout << "CXXOPTS: " << CXXOPTS__VERSION_MAJOR << "."
              << CXXOPTS__VERSION_MINOR << "." << CXXOPTS__VERSION_PATCH
              << '\n';
    std::cout << "SPDLOG: " << SPDLOG_VER_MAJOR << "." << SPDLOG_VER_MINOR
              << "." << SPDLOG_VER_PATCH << '\n';
    std::cout << "\n\nUsage Example:\n";

    // Compiler Warning and clang tidy error
    std::int32_t i = 0;

    // Adress Sanitizer should see this
    int *x = new int[42];

    const auto welcome_message =
        fmt::format("Welcome to {} v{}\n", project_name, project_version);
    spdlog::info(welcome_message);

    cxxopts::Options options(project_name.data(), welcome_message);

    options.add_options("arguments")("h,help", "Print usage")(
        "f,filename",
        "File name",
        cxxopts::value<std::string>())(
        "v,verbose",
        "Verbose output",
        cxxopts::value<bool>()->default_value("false"));

    auto result = options.parse(argc, argv);

    if (argc == 1 || result.count("help"))
    {
        std::cout << options.help() << '\n';
        return 0;
    }

    auto filename = std::string{};
    auto verbose = false;

    if (result.count("filename"))
    {
        filename = result["filename"].as<std::string>();
    }
    else
    {
        return 1;
    }

    verbose = result["verbose"].as<bool>();

    if (verbose)
    {
        fmt::print("Opening file: {}\n", filename);
    }

    auto ifs = std::ifstream{filename};

    if (!ifs.is_open())
    {
        return 1;
    }

    const auto parsed_data = json::parse(ifs);

    if (verbose)
    {
        const auto name = parsed_data["name"];
        fmt::print("Name: {}\n", name);
    }

    return 0;
}
