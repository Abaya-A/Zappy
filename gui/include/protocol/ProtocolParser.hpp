#pragma once

#include "ProtocolCommand.hpp"

#include <optional>
#include <string>

class ProtocolParser {
public:
    std::optional<ProtocolCommand> parseLine(const std::string &line) const;
};