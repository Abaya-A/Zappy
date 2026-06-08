#include "protocol/ProtocolParser.hpp"

#include <iostream>
#include <optional>
#include <string>

static int g_failed = 0;

static void check(bool condition, const std::string &message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        ++g_failed;
        return;
    }
    std::cout << "[ OK ] " << message << std::endl;
}

static void testParseBasicCommand()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("msz 10 8");

    check(command.has_value(), "parser returns command");
    check(command->name() == "msz", "command name is msz");
    check(command->hasArgCount(2), "msz has 2 args");
    check(command->arg(0).value_or("") == "10", "first arg is 10");
    check(command->arg(1).value_or("") == "8", "second arg is 8");
    check(command->raw() == "msz 10 8", "raw line is preserved");
}

static void testIntegerArguments()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("bct 2 3 1 0 0 2 0 0 0");

    check(command.has_value(), "bct command parses");
    check(command->name() == "bct", "command name is bct");
    check(command->hasArgCount(9), "bct has 9 args");
    check(command->intArg(0).value_or(-1) == 2, "bct x is 2");
    check(command->intArg(1).value_or(-1) == 3, "bct y is 3");
    check(command->intArg(2).value_or(-1) == 1, "bct food is 1");
    check(command->intArg(5).value_or(-1) == 2, "bct resource index 5 is 2");
}

static void testProtocolIds()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("pnw #42 5 6 2 1 TeamA");

    check(command.has_value(), "pnw command parses");
    check(command->name() == "pnw", "command name is pnw");
    check(command->idArg(0).value_or(-1) == 42, "player id #42 parses");
    check(command->intArg(1).value_or(-1) == 5, "player x is 5");
    check(command->intArg(2).value_or(-1) == 6, "player y is 6");
    check(command->arg(5).value_or("") == "TeamA", "team name is TeamA");
}

static void testInvalidArguments()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("ppo #abc x 4 2");

    check(command.has_value(), "invalid values still produce command object");
    check(!command->idArg(0).has_value(), "invalid id does not parse");
    check(!command->intArg(1).has_value(), "invalid integer does not parse");
    check(command->intArg(2).value_or(-1) == 4, "valid integer still parses");
}

static void testEmptyLine()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("");

    check(!command.has_value(), "empty line returns nullopt");
}

static void testServerMessage()
{
    ProtocolParser parser;
    std::optional<ProtocolCommand> command = parser.parseLine("smg hello world from server");

    check(command.has_value(), "smg command parses");
    check(command->name() == "smg", "command name is smg");
    check(command->hasArgCount(4), "smg has 4 args");
    check(command->arg(0).value_or("") == "hello", "smg first word is hello");
    check(command->arg(3).value_or("") == "server", "smg last word is server");
}

int main()
{
    testParseBasicCommand();
    testIntegerArguments();
    testProtocolIds();
    testInvalidArguments();
    testEmptyLine();
    testServerMessage();

    if (g_failed != 0) {
        std::cerr << g_failed << " test(s) failed" << std::endl;
        return 84;
    }

    std::cout << "All protocol parser tests passed" << std::endl;
    return 0;
}