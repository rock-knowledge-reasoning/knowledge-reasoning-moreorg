#include <boost/test/unit_test.hpp>
#include <organization_model/Analyser.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;
using namespace organization_model::vocabulary;
using namespace owlapi::model;

struct AnalyserFixture
{
    AnalyserFixture()
    {
        OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
        IRI sherpa = OM::resolve("Sherpa");
        IRI payload = OM::resolve("Payload");

        ModelPool modelPool;
        modelPool[sherpa] = 2;
        modelPool[payload] = 5;

        ask = OrganizationModelAsk(om, modelPool, true);

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
                true,
                activity::ACTIVE_OPERATIVE,
                FunctionalityRequirement());

        StatusSample sample1(agent1,
                base::Position(0,0,0),
                base::Position(100,0,0),
                0,
                100,
                true,
                activity::ACTIVE_OPERATIVE,
                FunctionalityRequirement());

        analyser = new Analyser(ask);
        analyser->add(sample0);
        analyser->add(sample1);

        analyser->createIndex();
    }

    ~AnalyserFixture()
    {
        delete analyser;
        analyser = NULL;
    }

    Analyser* analyser;

    Agent::List agents;
    AtomicAgent::Set atomicAgents;
    OrganizationModelAsk ask;
};

BOOST_AUTO_TEST_SUITE(analyser)

BOOST_FIXTURE_TEST_CASE(get_current_position, AnalyserFixture)
{
    // 30*0.5
    std::vector<base::Position> positions = analyser->getPositions(30);
    for(const base::Position& position : positions)
    {
        BOOST_TEST_MESSAGE("Position: " << position.x() << "/"
                << position.y() << "/"
                << position.z() );
    }
}

BOOST_FIXTURE_TEST_CASE(get_energy, AnalyserFixture)
{
    // Check corner case
    for(const AtomicAgent& a : atomicAgents)
    {
        {
            double energyReduction = analyser->getEnergyReductionAbsolute(0, a);
            BOOST_REQUIRE_MESSAGE(energyReduction == 0, "Energy reduction at start should be zero, was '" << energyReduction << "'");
        }

        {
            double energyReduction = analyser->getEnergyReductionAbsolute(100, a);
            if( a.getFacet(ask).getEnergyCapacity() != 0)
            {
                BOOST_REQUIRE_MESSAGE(energyReduction > 0, "Energy reduction for '" <<
                        a.getName() << "' should be > 0, but was '" << energyReduction << "'");
            } else {
                BOOST_REQUIRE_MESSAGE(energyReduction == 0, "Energy reduction should be == 0, but was '" << energyReduction << "'");
            }
        }
        for(size_t i = 0; i < 100; ++i)
        {
            BOOST_TEST_MESSAGE("Energy reduction (" << a.getName() << ") absolute -- relative: " << analyser->getEnergyReductionAbsolute(i, a) << " -- " << analyser->getEnergyReductionRelative(i,a));
        }
    }

    for(size_t i = 0; i < 100; ++i)
    {
        BOOST_TEST_MESSAGE("EnergyMAD: " << analyser->getEnergyMAD(i));
    }

    for(size_t i = 0; i < 100; ++i)
    {
        BOOST_TEST_MESSAGE("getEnergyAvailableMinRelative: " << analyser->getEnergyAvailableMinRelative(i));
    }
}

BOOST_AUTO_TEST_SUITE_END()
