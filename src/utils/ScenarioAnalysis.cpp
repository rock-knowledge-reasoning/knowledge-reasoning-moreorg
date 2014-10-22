#include "owl_om/ccf/Scenario.hpp"

#include <set>
#include <iostream>
#include <math.h>

int main()
{
    using namespace multiagent::ccf;

    Scenario scenario = Scenario::fromConsole();
    std::cout << scenario;
    return 0;
}
