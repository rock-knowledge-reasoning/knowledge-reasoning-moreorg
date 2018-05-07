#include <boost/test/unit_test.hpp>
#include <organization_model/Heuristics.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;
using namespace owlapi::model;
using namespace organization_model::vocabulary;

struct HeuristicsFixture
{
    HeuristicsFixture()
    {
        OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
        IRI sherpa = OM::resolve("Sherpa");
        IRI payload = OM::resolve("Payload");

        ModelPool modelPool;
        modelPool[sherpa] = 2;
        modelPool[payload] = 5;

        OrganizationModelAsk ask(om, modelPool, true);

        AtomicAgent::List sherpas;
        for(size_t i = 0; i < modelPool[sherpa]; ++i)
        {
            sherpas.push_back( AtomicAgent(i, sherpa));
        }
        AtomicAgent::List payloads;
        for(size_t i = 0; i < modelPool[payload]; ++i)
        {
            payloads.push_back( AtomicAgent(i, payload));
        }

        Agent agent0;
        agent0.add(sherpas[0]);
        agent0.add(payloads[0]);
        agent0.add(payloads[1]);
        agent0.update(ask);

        Agent agent1;
        agent1.add(sherpas[1]);
        agent1.add(payloads[2]);
        agent1.add(payloads[3]);
        agent1.add(payloads[4]);
        agent1.update(ask);

        agents.push_back(agent0);
        agents.push_back(agent1);
        atomicAgents.insert(sherpas.begin(), sherpas.end());
        atomicAgents.insert(payloads.begin(), payloads.end());

        StatusSample sample0(agent0,
                base::Position(0,0,0),
                base::Position(0,100,0),
                0,
                100,
                Agent::OPERATIVE,
                activity::BUSY);

        StatusSample sample1(agent1,
                base::Position(0,0,0),
                base::Position(100,0,0),
                0,
                100,
                Agent::OPERATIVE,
                activity::BUSY);

        statusSamples.push_back(sample0);
        statusSamples.push_back(sample1);

        heuristics = new Heuristics(ask);
    }

    ~HeuristicsFixture()
    {
        delete heuristics;
        heuristics = NULL;
    }

    StatusSample::List statusSamples;
    Heuristics* heuristics;

    Agent::List agents;
    AtomicAgent::Set atomicAgents;
};

BOOST_AUTO_TEST_SUITE(heuristics)

BOOST_AUTO_TEST_CASE(estimate_current_position)
{
    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = vocabulary::OM::resolve("Sherpa");

    ModelPool modelPool;
    modelPool[sherpa] = 1;

    OrganizationModelAsk ask(om, modelPool, true);

    Heuristics heuristics(ask);
    base::Position fromPosition(0.0,0.0,0.0);
    base::Position toPosition(100.0,0.0,0.0);
    size_t durationInS = 60;
    // expecting 0.5 m/s for Sherpa
    double expectedDistance = 60*0.5;

    AtomicAgent atomicAgent(0, sherpa);
    Agent agent(atomicAgent);

    base::Position position = heuristics.positionLinear(agent,
            fromPosition,
            toPosition,
            durationInS);

    BOOST_REQUIRE_MESSAGE(position.x() == expectedDistance && position.y() == 0 && position.z() == 0,
            "Position estimate: " << position.x() << "/" << position.y() << "/" << position.z());
}

BOOST_FIXTURE_TEST_CASE(energy, HeuristicsFixture)
{
    for(const StatusSample& s : statusSamples)
    {
        double energyConsumption = heuristics->getEnergyConsumption(&s);
        BOOST_REQUIRE_MESSAGE(energyConsumption != 0, "EnergyConsumption should be greater than 0, was " << energyConsumption);

        for(const AtomicAgent& aa : s.getAgent().getAtomicAgents())
        {
            double energyReduction = heuristics->getEnergyReductionAbsolute(&s,
                    aa,
                    0,
                    0);

            BOOST_REQUIRE_MESSAGE(energyReduction == 0, "EnergyReduction for " << aa.toString() << " should be 0, was "
                    << energyReduction);
        }
    }

}

BOOST_FIXTURE_TEST_CASE(reconfiguration_cost, HeuristicsFixture)
{
    AtomicAgent::List availableAgents(atomicAgents.begin(), atomicAgents.end());

    Agent agent0;
    agent0.add(availableAgents[0]);
    agent0.add(availableAgents[1]);

    Agent agent1;
    agent1.add(availableAgents[2]);

    {
        Agent::Set coalitionStructure0;
        coalitionStructure0.insert(agent0);
        coalitionStructure0.insert(agent1);
        Agent::Set coalitionStructure1;
        coalitionStructure1.insert(agent0);
        coalitionStructure1.insert(agent1);

        double cost = heuristics->getReconfigurationCost(coalitionStructure0, coalitionStructure1);
        BOOST_REQUIRE_MESSAGE(cost == 0, "Same agent coalition structure reconfiguration cost should be 0, was " << cost);
    }

    Agent agent2;
    agent2.add(availableAgents[0]);
    agent2.add(availableAgents[1]);
    agent2.add(availableAgents[2]);

    {
        Agent::Set coalitionStructure0;
        coalitionStructure0.insert(agent0);
        coalitionStructure0.insert(agent1);
        Agent::Set coalitionStructure1;
        coalitionStructure1.insert(agent2);

        double cost = heuristics->getReconfigurationCost(coalitionStructure0, coalitionStructure1);
        double expectedCost = 600*2 + 180*3;
        BOOST_REQUIRE_MESSAGE(cost == expectedCost, "Reconfiguration cost should be '" << expectedCost << "', was " << cost);
    }
}

BOOST_AUTO_TEST_SUITE_END()
