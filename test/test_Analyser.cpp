#include <boost/test/unit_test.hpp>
#include <organization_model/Analyser.hpp>
#include <organization_model/vocabularies/OM.hpp>
#include "test_utils.hpp"

using namespace organization_model;
using namespace organization_model::vocabulary;
using namespace owlapi::model;

BOOST_AUTO_TEST_SUITE(analyser)

BOOST_AUTO_TEST_CASE(get_current_position)
{
    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");

    ModelPool modelPool;
    modelPool[sherpa] = 2;
    modelPool[payload] = 5;

    OrganizationModelAsk ask(om, modelPool, true);

    Analyser analyser(ask);

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

    Agent agent1;
    agent1.add(sherpas[1]);
    agent1.add(payloads[2]);
    agent1.add(payloads[3]);
    agent1.add(payloads[4]);

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

    analyser.add(sample0);
    analyser.add(sample1);

    analyser.createIndex();

    // 30*0.5
    std::vector<base::Position> positions = analyser.getPositions(30);
    for(const base::Position& position : positions)
    {
        BOOST_TEST_MESSAGE("Position: " << position.x() << "/"
                << position.y() << "/"
                << position.z() );
    }
}

BOOST_AUTO_TEST_SUITE_END()
