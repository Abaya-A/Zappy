#include "Resource.hpp"

#include <iostream>

static int g_failed = 0;

static void check(bool condition, const char *message)
{
    if (!condition) {
        std::cerr << "[FAIL] " << message << std::endl;
        ++g_failed;
        return;
    }
    std::cout << "[ OK ] " << message << std::endl;
}

int main()
{
    check(Resource::COUNT == 7, "resource count is 7");

    check(Resource::index(ResourceType::Food) == 0, "food index is 0");
    check(Resource::index(ResourceType::Linemate) == 1, "linemate index is 1");
    check(Resource::index(ResourceType::Thystame) == 6, "thystame index is 6");

    check(Resource::name(ResourceType::Food) == "food", "food name is correct");
    check(Resource::name(ResourceType::Linemate) == "linemate", "linemate name is correct");
    check(Resource::name(ResourceType::Thystame) == "thystame", "thystame name is correct");

    check(Resource::fromIndex(0).value_or(ResourceType::Thystame) == ResourceType::Food,
        "index 0 converts to food");
    check(Resource::fromIndex(6).value_or(ResourceType::Food) == ResourceType::Thystame,
        "index 6 converts to thystame");
    check(!Resource::fromIndex(7).has_value(), "invalid index fails");

    check(Resource::fromName("food").value_or(ResourceType::Thystame) == ResourceType::Food,
        "name food converts to enum");
    check(Resource::fromName("thystame").value_or(ResourceType::Food) == ResourceType::Thystame,
        "name thystame converts to enum");
    check(!Resource::fromName("unknown").has_value(), "invalid name fails");

    if (g_failed != 0)
        return 84;

    std::cout << "All resource tests passed" << std::endl;
    return 0;
}