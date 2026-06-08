#include "client/GuiArgs.hpp"
#include "client/GuiClient.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char **argv)
{
    try {
        GuiArgs args = parseGuiArgs(argc, argv);
        GuiClient client(args.host, args.port, argc, argv);
        
        if (!client.connect())
            return 84;

        client.run();
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "[ERROR]: " << error.what() << std::endl;
        std::cerr << "Try './zappy_gui --help' for usage." << std::endl;
        return 84;
    }
}