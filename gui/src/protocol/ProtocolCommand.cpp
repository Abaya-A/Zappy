#include "protocol/ProtocolCommand.hpp"

#include <charconv>
#include <utility>

ProtocolCommand::ProtocolCommand(std::string name, std::vector<std::string> args, std::string raw)
    : _name(std::move(name)),
      _args(std::move(args)),
      _raw(std::move(raw))
{
}

const std::string &ProtocolCommand::name() const
{
    return _name;
}

const std::vector<std::string> &ProtocolCommand::args() const
{
    return _args;
}

const std::string &ProtocolCommand::raw() const
{
    return _raw;
}

bool ProtocolCommand::is(const std::string &name) const
{
    return _name == name;
}

bool ProtocolCommand::hasArgCount(std::size_t count) const
{
    return _args.size() == count;
}

std::optional<std::string> ProtocolCommand::arg(std::size_t index) const
{
    if (index >= _args.size())
        return std::nullopt;
    return _args[index];
}

std::optional<int> ProtocolCommand::intArg(std::size_t index) const
{
    int value = 0;
    const std::string *text = nullptr;

    if (index >= _args.size())
        return std::nullopt;

    text = &_args[index];
    auto result = std::from_chars(text->data(), text->data() + text->size(), value);

    if (result.ec != std::errc{} || result.ptr != text->data() + text->size())
        return std::nullopt;
    return value;
}

std::optional<int> ProtocolCommand::idArg(std::size_t index) const
{
    int value = 0;
    const std::string *text = nullptr;

    if (index >= _args.size())
        return std::nullopt;

    text = &_args[index];
    if (text->empty() || (*text)[0] != '#')
        return std::nullopt;

    auto result = std::from_chars(text->data() + 1, text->data() + text->size(), value);

    if (result.ec != std::errc{} || result.ptr != text->data() + text->size())
        return std::nullopt;
    return value;
}