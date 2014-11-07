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
//    return coalition.size();

    Coalition c = coalition;
    std::sort(c.begin(), c.end());

    std::map<Coalition, double> costMap;
    updateCostMap(costMap, "a", 125.0);
    updateCostMap(costMap, "b", 50.0);
    updateCostMap(costMap, "c", 75.0);
    updateCostMap(costMap, "d", 150);
    for(char i = 'e'; i < 'z'; ++i)
    {
        updateCostMap(costMap, std::string(1, i), 100);
    }

    updateCostMap(costMap, "ab", 176);
    updateCostMap(costMap, "ac", 150);
    updateCostMap(costMap, "ad", 100);
    updateCostMap(costMap, "bc", 150);
    updateCostMap(costMap, "bd", 200);
    updateCostMap(costMap, "cd", 125);

    updateCostMap(costMap, "abc", 200);
    updateCostMap(costMap, "abd", 150);
    updateCostMap(costMap, "acd", 220);
    updateCostMap(costMap, "bcd", 150);

    updateCostMap(costMap, "abcd", 325);
    updateCostMap(costMap, "efghijklm", 1400);
    updateCostMap(costMap, "opqr", 5400);
    updateCostMap(costMap, "stuvwxyz", 15400);

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

    std::map<Coalition, double>::const_iterator cit = costMap.find(coalition);
    if(cit != costMap.end())
    {
        return cit->second;
    } else {
        double value = coalition.size()*coalition.size();
        return value;
    }
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
    char numberOfAgents = 8;
    for(char a = 'a'; a < 'a' + numberOfAgents; ++a)
    {
        std::stringstream ss;
        ss << a;
        agents.push_back(ss.str());
    }
    CoalitionStructureGeneration csg(agents, coalitionValueFunction, coalitionStructureValueFunction);
    BOOST_TEST_MESSAGE("Preparation done: " << csg.toString());
    CoalitionStructure cs = csg.findBest(1.0);
    BOOST_TEST_MESSAGE("Found best: " << CoalitionStructureGeneration::toString(cs) << ", value: " << coalitionStructureValueFunction(cs));

    csg.reset();
    BOOST_TEST_MESSAGE("Reset done: " << csg.toString());
    csg.anytimeSearch(1.0);
    while(!csg.completed())
    {
        sleep(1);
        BOOST_TEST_MESSAGE("Intermediate result: " << CoalitionStructureGeneration::toString( csg.currentBestSolution()) << ", value: " << coalitionStructureValueFunction(csg.currentBestSolution()) );
    }
    BOOST_TEST_MESSAGE("Final result: " << CoalitionStructureGeneration::toString(csg.currentBestSolution()) << ", value: " << coalitionStructureValueFunction(csg.currentBestSolution()) );
}
