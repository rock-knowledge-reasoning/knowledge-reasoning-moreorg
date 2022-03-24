#include "test_utils.hpp"
#include <boost/test/unit_test.hpp>
#include <moreorg/Analyser.hpp>
#include <moreorg/vocabularies/OM.hpp>

using namespace moreorg;
using namespace moreorg::vocabulary;
using namespace owlapi::model;

struct AnalyserFixture
{
    AnalyserFixture()
    {
        OrganizationModel::Ptr om =
            make_shared<OrganizationModel>(getOMSchema());
        IRI sherpa = OM::resolve("Sherpa");
        IRI payload = OM::resolve("Payload");

        ModelPool modelPool;
        modelPool[sherpa] = 2;
        modelPool[payload] = 5;

        ask = OrganizationModelAsk(om, modelPool, true);

        AtomicAgent::List sherpas;
        for(size_t i = 0; i < modelPool[sherpa]; ++i)
        {
            sherpas.push_back(AtomicAgent(i, sherpa));
        }
        AtomicAgent::List payloads;
        for(size_t i = 0; i < modelPool[payload]; ++i)
        {
            payloads.push_back(AtomicAgent(i, payload));
        }

        Agent agent0;
        agent0.add(sherpas[0]);
        agent0.add(payloads[0]);
        agent0.add(payloads[1]);

        Agent agent1;
        agent1.add(sherpas[1]);
        agent1.add(payloads[2]);
        agent1.add(payloads[3]);
        agent1.add(payloads[4]);

        agents.push_back(agent0);
        agents.push_back(agent1);
        atomicAgents.insert(sherpas.begin(), sherpas.end());
        atomicAgents.insert(payloads.begin(), payloads.end());

        StatusSample sample00(agent0,
                              base::Position(0, 0, 0),
                              base::Position(0, 100, 0),
                              0,
                              25,
                              Agent::OPERATIVE,
                              activity::BUSY);

        StatusSample sample01(agent0,
                              base::Position(0, 100, 0),
                              base::Position(0, 100, 0),
                              26,
                              75,
                              Agent::OPERATIVE,
                              activity::BUSY);

        StatusSample sample02(agent0,
                              base::Position(0, 100, 0),
                              base::Position(0, 120, 0),
                              76,
                              100,
                              Agent::OPERATIVE,
                              activity::BUSY);

        StatusSample sample10(agent1,
                              base::Position(0, 0, 0),
                              base::Position(100, 0, 0),
                              0,
                              100,
                              Agent::OPERATIVE,
                              activity::BUSY);

        Resource::Set resources;
        resources.insert(Resource(OM::resolve("TransportProvider")));

        RequirementSample r00(resources,
                              ModelPool(),
                              base::Position(0, 0, 0),
                              base::Position(0, 100, 0),
                              0,
                              25);

        Resource::Set stereoImageProvider;
        stereoImageProvider.insert(
            Resource(OM::resolve("StereoImageProvider")));

        RequirementSample r01(stereoImageProvider,
                              ModelPool(),
                              base::Position(0, 100, 0),
                              base::Position(0, 100, 0),
                              26,
                              75);

        RequirementSample r02(resources,
                              ModelPool(),
                              base::Position(0, 100, 0),
                              base::Position(0, 120, 0),
                              76,
                              100);
        RequirementSample r10(resources,
                              ModelPool(),
                              base::Position(0, 0, 0),
                              base::Position(100, 0, 0),
                              0,
                              100);

        analyser = new Analyser(ask);
        analyser->add(sample00);
        analyser->add(sample01);
        analyser->add(sample02);
        analyser->add(sample10);

        analyser->add(r00);
        analyser->add(r01);
        analyser->add(r10);

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
        BOOST_TEST_MESSAGE("Position: " << position.x() << "/" << position.y()
                                        << "/" << position.z());
    }
}

BOOST_FIXTURE_TEST_CASE(get_energy, AnalyserFixture)
{
    // Check corner case
    for(const AtomicAgent& a : atomicAgents)
    {
        {
            double energyReduction = analyser->getEnergyReductionAbsolute(0, a);
            BOOST_REQUIRE_MESSAGE(
                energyReduction == 0,
                "Energy reduction at start should be zero, was '"
                    << energyReduction << "'");
        }

        {
            double energyReduction =
                analyser->getEnergyReductionAbsolute(100, a);
            if(a.getFacade(ask).getEnergyCapacity() != 0)
            {
                BOOST_REQUIRE_MESSAGE(energyReduction > 0,
                                      "Energy reduction for '"
                                          << a.getName()
                                          << "' should be > 0, but was '"
                                          << energyReduction << "'");
            } else
            {
                BOOST_REQUIRE_MESSAGE(
                    energyReduction == 0,
                    "Energy reduction should be == 0, but was '"
                        << energyReduction << "'");
            }
        }
        for(size_t i = 0; i < 100; ++i)
        {
            BOOST_TEST_MESSAGE("Energy reduction ("
                               << a.getName() << ") absolute -- relative: "
                               << analyser->getEnergyReductionAbsolute(i, a)
                               << " -- "
                               << analyser->getEnergyReductionRelative(i, a));
        }
    }

    for(size_t i = 0; i < 100; ++i)
    {
        BOOST_TEST_MESSAGE("EnergyMAD: " << analyser->getEnergyMAD(i));
    }

    for(size_t i = 0; i < 100; ++i)
    {
        BOOST_TEST_MESSAGE("getEnergyAvailableMinRelative: "
                           << analyser->getEnergyAvailableMinRelative(i));
    }
}

BOOST_FIXTURE_TEST_CASE(samples, AnalyserFixture)
{
    BOOST_TEST_MESSAGE("All samples at: 0 #"
                       << analyser->getSampleColumnDescriptionO());
    for(int i = 0; i < 100; ++i)
    {
        std::vector<double> row = analyser->getSampleO(i);
        BOOST_TEST_MESSAGE("Sample: " << Analyser::toString(row));
    }
}

BOOST_AUTO_TEST_SUITE_END()
