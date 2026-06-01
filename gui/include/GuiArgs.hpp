#pragma once

#include <string>

struct GuiArgs {
    int port;
    std::string host;
};

GuiArgs parseGuiArgs(int argc, char **argv);
void printGuiUsage();