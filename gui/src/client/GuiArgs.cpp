#include "client/GuiArgs.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>

void printGuiUsage()
{
    std::cout << "USAGE: ./zappy_gui -p port -h machine" << std::endl;
}

static int parsePositiveInt(const std::string &value, const std::string &flag)
{
    char *end = nullptr;
    long result = std::strtol(value.c_str(), &end, 10);

    if (*end != '\0' || result <= 0 || result > 65535)
        throw std::runtime_error("bad value for " + flag);
    return static_cast<int>(result);
}

GuiArgs parseGuiArgs(int argc, char **argv)
{
    GuiArgs args = {
        .port = 0,
        .host = ""
    };

    if (argc == 2 && std::string(argv[1]) == "--help") {
        printGuiUsage();
        std::exit(0);
    }

    for (int i = 1; i < argc; ++i) {
        std::string current = argv[i];

        if (current == "-p") {
            if (i + 1 >= argc)
                throw std::runtime_error("missing value for -p");
            args.port = parsePositiveInt(argv[++i], "-p");
        } else if (current == "-h") {
            if (i + 1 >= argc)
                throw std::runtime_error("missing value for -h");
            args.host = argv[++i];
        } else {
            throw std::runtime_error("unknown argument: " + current);
        }
    }

    if (args.port == 0)
        throw std::runtime_error("missing argument: -p");
    if (args.host.empty())
        throw std::runtime_error("missing argument: -h");

    return args;
}