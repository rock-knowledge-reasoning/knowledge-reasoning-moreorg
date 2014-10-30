#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <owl_om/utils/CoalitionStructureGeneration.hpp>

using namespace multiagent::utils;

void updateCostMap(std::map<Coalition, double>& costMap, const std::string& desc, double cost)
{
    Coalition c;
    for(size_t i = 0; i < desc.size(); ++i)
    {
        std::string name = desc.substr(i,1);
        c.push_back(name); 
    }
    std::sort(c.begin(), c.end());
    costMap[c] = cost;
}

double coalitionValueFunction(const Coalition& coalition)
{
    Coalition c = coalition;
    std::sort(c.begin(), c.end());

    std::map<Coalition, double> costMap;
    updateCostMap(costMap, "a", 125.0);
    updateCostMap(costMap, "b", 50.0);
    updateCostMap(costMap, "c", 75.0);
    updateCostMap(costMap, "d", 150);

    updateCostMap(costMap, "ab", 175);
    updateCostMap(costMap, "ac", 150);
    updateCostMap(costMap, "ad", 100);
    updateCostMap(costMap, "bc", 150);
    updateCostMap(costMap, "bd", 200);
    updateCostMap(costMap, "cd", 125);

    updateCostMap(costMap, "abc", 200);
    updateCostMap(costMap, "abd", 150);
    updateCostMap(costMap, "acd", 300);
    updateCostMap(costMap, "bcd", 150);

    updateCostMap(costMap, "abcd", 425);

   // std::map<Coalition, double>::const_iterator cit = costMap.begin();
   // for(; cit != costMap.end(); ++cit)
   // {
   //     Coalition c = cit->first;
   //     std::string s;
   //     for(size_t i = 0; i < c.size(); ++i)
   //     {
   //         s += c[i];
   //     }
   //     BOOST_TEST_MESSAGE("Return value: " << cit->second << " for " << s);
   // }

    double value = costMap[c];
    BOOST_TEST_MESSAGE("Return value: " << value);

    return value;
}

double coalitionStructureValueFunction(const CoalitionStructure& c)
{
    double value = 0.0;
    for(int i = 0; i < c.size(); ++i)
    {
        value += coalitionValueFunction( c[i] );
    }
    return value;
}

BOOST_AUTO_TEST_CASE(it_should_compute_csg)
{
    AgentList agents;
    agents.push_back("a");
    agents.push_back("b");
    agents.push_back("c");
    agents.push_back("d");
    CoalitionStructureGeneration csg(agents, coalitionValueFunction, coalitionStructureValueFunction);
    csg.prepare();
    BOOST_TEST_MESSAGE("Preparation done: " << csg.toString());
    csg.findBest();
}
