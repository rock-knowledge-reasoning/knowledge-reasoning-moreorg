#include "organization_model/ccf/Scenario.hpp"

#include <set>
#include <iostream>
#include <math.h>
#include <base/Time.hpp>

int main()
{
    using namespace multiagent::ccf;

    Scenario scenario = Scenario::fromConsole();
    std::cout << scenario;
    std::cout << "Start creating actor types" << std::endl;
    base::Time start = base::Time::now();
    scenario.createCompositeActorTypes();
    std::cout << "End creating actor types: " << (base::Time::now() - start).toSeconds() << std::endl;
    return 0;
}
