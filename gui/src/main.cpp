#include "GuiArgs.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    try {
        GuiArgs args = parseGuiArgs(argc, argv);

        std::cout << "zappy_gui skeleton" << std::endl;
        std::cout << "port = " << args.port << std::endl;
        std::cout << "host = " << args.host << std::endl;
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "[ERROR]: " << error.what() << std::endl;
        std::cerr << "Try './zappy_gui --help' for usage." << std::endl;
        return 84;
    }
}