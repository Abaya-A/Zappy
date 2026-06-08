#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

class ProtocolCommand {
public:
    ProtocolCommand(std::string name, std::vector<std::string> args, std::string raw);

    const std::string &name() const;
    const std::vector<std::string> &args() const;
    const std::string &raw() const;

    bool is(const std::string &name) const;
    bool hasArgCount(std::size_t count) const;

    std::optional<std::string> arg(std::size_t index) const;
    std::optional<int> intArg(std::size_t index) const;
    std::optional<int> idArg(std::size_t index) const;

private:
    std::string _name;
    std::vector<std::string> _args;
    std::string _raw;
};