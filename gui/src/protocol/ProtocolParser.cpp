#include "protocol/ProtocolParser.hpp"

#include <sstream>
#include <vector>

std::optional<ProtocolCommand> ProtocolParser::parseLine(const std::string &line) const
{
    std::istringstream stream(line);
    std::string name;
    std::vector<std::string> args;
    std::string arg;

    if (!(stream >> name))
        return std::nullopt;

    while (stream >> arg)
        args.push_back(arg);

    return ProtocolCommand(name, args, line);
}