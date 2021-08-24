#include <boost/test/unit_test.hpp>
#include <moreorg/OrganizationModel.hpp>
#include <moreorg/OrganizationModelAsk.hpp>
#include <moreorg/reasoning/ResourceMatch.hpp>
#include <moreorg/vocabularies/OM.hpp>
#include <moreorg/Resource.hpp>
#include <moreorg/PropertyConstraintSolver.hpp>
#include <gecode/search.hh>
#include "test_utils.hpp"

#include <moreorg/facades/Robot.hpp>

using namespace moreorg;
using namespace moreorg::reasoning;
using namespace moreorg::vocabulary;

using namespace owlapi::vocabulary;
using namespace owlapi::model;


BOOST_AUTO_TEST_SUITE(moreorg_ask)

BOOST_AUTO_TEST_CASE(supported_functionalities)
{
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(getOMSchema());
    std::vector<std::string> robotNames = { "Sherpa", "SherpaTT", "CoyoteIII", "CREX"  };

    for(const std::string& robotName : robotNames )
    {
        IRI robot = vocabulary::OM::resolve(robotName);

        ModelPool modelPool;
        modelPool[robot] = 1;

        OrganizationModelAsk ask(om, modelPool, true);
        BOOST_TEST_MESSAGE("Supported functionalities by " << robot << ": " <<  ask.getSupportedFunctionalities().toString() );

        Resource::Set resources;
        resources.insert( { Resource(vocabulary::OM::resolve("MoveTo")) });

        BOOST_REQUIRE_MESSAGE(ask.isSupporting(modelPool, resources),
                "Robot '" << robot << "' supports 'MoveTo'");
    }
}

BOOST_AUTO_TEST_CASE(recursive_resolution)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = OM::resolve("Sherpa");
    IRI basecamp = OM::resolve("BaseCamp");
    IRI locationImageProvider = OM::resolve("LocationImageProvider");

    OrganizationModelAsk ask(om);
    Functionality functionality(locationImageProvider);

    typedef std::pair<uint32_t, algebra::SupportType> ExpectedResult;
    std::vector<ExpectedResult> expected;
    IRIList agents;

    agents.push_back(sherpa);
    expected.push_back( ExpectedResult(1,algebra::FULL_SUPPORT) );

    agents.push_back(basecamp);
    expected.push_back( ExpectedResult(0, algebra::NO_SUPPORT) );


    for(size_t i = 0; i < agents.size(); ++i)
    {
        IRI agent = agents[i];
        IRIList combination;
        combination.push_back(agent);

        IRIList functionalities;
        functionalities.push_back(functionality.getModel());

        IRIList supportedFunctionalityModels = ResourceMatch::filterSupportedModels(combination, functionalities, om->ontology());

        OWLOntologyAsk oAsk(om->ontology());
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = oAsk.getCardinalityRestrictions(locationImageProvider, vocabulary::OM::has());
        BOOST_TEST_MESSAGE("Cardinality: " << OWLCardinalityRestriction::toString(restrictions));
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality.getModel(), agent);
            BOOST_REQUIRE_MESSAGE(saturationPoint == expected[i].first,  expected[i].first << " expected as saturation point for " << agent.getFragment() << " sufficient for LocationImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, agent, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == expected[i].second, "Support from " << agent.getFragment() << " for LocationImageProvider at saturation point: expected " << expected[i].second << " was " << supportType);
        }
    }
}

BOOST_AUTO_TEST_CASE(functional_saturation)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    IRI stereoImageProvider = OM::resolve("StereoImageProvider");
    IRI locationImageProvider = OM::resolve("LocationImageProvider");
    IRI emiPowerProvider = OM::resolve("EmiPowerProvider");

    OrganizationModelAsk ask(om);
    {
        IRI functionality = stereoImageProvider;
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for StereoImageProvider at saturation point");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, payload);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 0, "0 Payload sufficient for StereoImageProvider: was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, payload, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::NO_SUPPORT, "No support from payload for StereoImageProvider");
        }
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, payloadCamera);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 2, "2 PayloadCamera requiremed for StereoImageProvider: was" << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, payloadCamera, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::PARTIAL_SUPPORT, "Partial support from payload camera for StereoImageProvider at saturation point -- (Power required for full support)");
        }
    }
    {
        IRI functionality = locationImageProvider;
        {
            uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, sherpa);
            BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 Sherpa sufficient for '" << functionality << "' : was " << saturationPoint);

            algebra::SupportType supportType = ask.getSupportType(functionality, sherpa, saturationPoint);
            BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from sherpa for '" << functionality << "' at saturation point");
        }
    }
    {
        IRI functionality = emiPowerProvider;
        {
            {
                IRI system = sherpa;
                uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, system);
                BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 '" << system << "' sufficient for '" << functionality << "' : was " << saturationPoint);

                algebra::SupportType supportType = ask.getSupportType(functionality, system, saturationPoint);
                BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from '" << system << "' for '" << functionality << "' at saturation point");
            }
            {
                IRI system = crex;
                uint32_t saturationPoint = ask.getFunctionalSaturationBound(functionality, system);
                BOOST_REQUIRE_MESSAGE(saturationPoint == 1, "1 '" << system << "' sufficient for '" << functionality << "' : was " << saturationPoint);

                algebra::SupportType supportType = ask.getSupportType(functionality, system, saturationPoint);
                BOOST_REQUIRE_MESSAGE(supportType == algebra::FULL_SUPPORT, "Full support from '" << system << "' for '" << functionality << "' at saturation point");

            }
        }
    }
    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;

        ask.prepare(modelPool);

        Functionality::Set functionalitySet;
        Functionality functionality(stereoImageProvider);
        functionalitySet.insert(functionality);

        ModelPool::Set combinations = ask.getBoundedResourceSupport(functionalitySet);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Bounded resource support for: " << stereoImageProvider.toString() << " by '" << ModelPool::toString(combinations) << "'");

        BOOST_REQUIRE_MESSAGE(ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");

        functionalitySet.insert( Functionality(emiPowerProvider) );
        functionalitySet.insert( Functionality(locationImageProvider) );
        BOOST_REQUIRE_MESSAGE(ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        ask.prepare(modelPool);

        Functionality::Set functionalitySet;
        Functionality functionality(stereoImageProvider);
        functionalitySet.insert(functionality);

        ModelPool::Set combinations = ask.getBoundedResourceSupport(functionalitySet);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Bounded resource support for: " << stereoImageProvider.toString() << " by '" << ModelPool::toString(combinations) << "'");

        BOOST_REQUIRE_MESSAGE(!ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is not minimal");

        functionalitySet.insert( Functionality(emiPowerProvider) );
        functionalitySet.insert( Functionality(locationImageProvider) );

        Functionality::Set intermediateCheck;
        intermediateCheck.insert(Functionality(emiPowerProvider));
        algebra::SupportType support0= ask.getSupportType(intermediateCheck, modelPool);
        algebra::SupportType support1= ask.getSupportType(functionalitySet, modelPool);

        BOOST_REQUIRE_MESSAGE(!ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is not minimal: support was " << algebra::SupportTypeTxt[support0] << " vs. " << algebra::SupportTypeTxt[support1] );

    }
    {
        ModelPool modelPool;
        modelPool[crex] = 2;
        modelPool[sherpa] = 1;

        ask.prepare(modelPool);

        Functionality::Set functionalitySet;
        Functionality functionality(stereoImageProvider);
        functionalitySet.insert(functionality);

        ModelPool::Set combinations = ask.getBoundedResourceSupport(functionalitySet);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Bounded resource support for: " << stereoImageProvider.toString() << " by '" << ModelPool::toString(combinations) << "'");

        BOOST_REQUIRE_MESSAGE(!ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");

        functionalitySet.insert( Functionality(emiPowerProvider) );
        functionalitySet.insert( Functionality(locationImageProvider) );
        BOOST_REQUIRE_MESSAGE(!ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");
    }

    {
        ModelPool modelPool;
        modelPool[crex] = 2;

        ask.prepare(modelPool);

        Functionality::Set functionalitySet;
        Functionality functionality(stereoImageProvider);
        functionalitySet.insert(functionality);

        ModelPool::Set combinations = ask.getBoundedResourceSupport(functionalitySet);
        BOOST_REQUIRE_MESSAGE(combinations.empty(), "Bounded resource support"
                " for: " << stereoImageProvider.toString() << " by '"
                << ModelPool::toString(combinations) << "' is not given ");

        BOOST_REQUIRE_MESSAGE(ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");

        functionalitySet.insert( Functionality(emiPowerProvider) );
        functionalitySet.insert( Functionality(locationImageProvider) );
        BOOST_REQUIRE_MESSAGE(ask.isMinimal(modelPool, functionalitySet), "ModelPool " << modelPool.toString() << " is minimal");
    }
}

BOOST_AUTO_TEST_CASE(functional_saturation_with_property_constraints)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    IRI stereoImageProvider = OM::resolve("StereoImageProvider");
    IRI locationImageProvider = OM::resolve("LocationImageProvider");
    IRI emiPowerProvider = OM::resolve("EmiPowerProvider");

    OrganizationModelAsk ask(om);
    {
        ModelPool modelPool;
        modelPool[sherpa] = 30;
        modelPool[crex] = 30;

        // Use functional saturation bound
        OrganizationModelAsk ask(om, modelPool, true);

        Functionality::Set functionalities;
        Functionality transportProvider( OM::resolve("TransportProvider") );

        double minItems = 30;
        PropertyConstraint constraint(OM::resolve("transportCapacity"), PropertyConstraint::GREATER_EQUAL, minItems);
        PropertyConstraint::Set constraints;
        constraints.insert(constraint);
        transportProvider.setPropertyConstraints(constraints);

        Functionality stereoImageProvider( OM::resolve("StereoImageProvider") );
        functionalities.insert( transportProvider );
        functionalities.insert( stereoImageProvider );

        {
            ModelPool modelPool = ask.getFunctionalSaturationBound(functionalities);
            BOOST_REQUIRE_MESSAGE(!modelPool.empty(), "Saturation bound for combinations supporting transport for minimum of " << minItems << " items: '" << modelPool.toString() << "'");
            BOOST_REQUIRE_MESSAGE(modelPool[crex] == 15, "Up to 15 instances of '" << crex << "' can contribute for this functionality, was: " << modelPool[crex]);
            BOOST_REQUIRE_MESSAGE(modelPool[sherpa] == 3, "Up to 3 instances of '" << sherpa << "' can contribute for this functionality was: " << modelPool[sherpa]);
        }
    }
}

BOOST_AUTO_TEST_CASE(get_resource_support)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;

        OrganizationModelAsk ask(om, modelPool);
        IRI functionalityModel = OM::resolve("ImageProvider");
        Functionality::Set functionalities;
        functionalities.insert( Functionality(functionalityModel) );

        ModelPool::Set combinations = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << functionalityModel.toString() << ": '" << ModelPool::toString(combinations) << "'");
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        OrganizationModelAsk ask(om, modelPool);
        Functionality::Set functionalities;

        IRI imageProvider = OM::resolve("ImageProvider");
        IRI emiPowerProvider = OM::resolve("EmiPowerProvider");
        functionalities.insert( Functionality(imageProvider) );
        functionalities.insert( Functionality(emiPowerProvider) );

        ModelPool::Set combinations = ask.getResourceSupport(functionalities);
        BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << imageProvider.toString() << " and " << emiPowerProvider.toString() << ": '" << ModelPool::toString(combinations) << "'");
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[crex] = 1;

        OrganizationModelAsk ask(om, modelPool);
        Functionality::Set functionalities;

        Functionality transportProvider( OM::resolve("TransportProvider") );
        functionalities.insert( transportProvider );

        {
            Functionality fr;
            BOOST_REQUIRE_THROW( ask.getResourceSupport(fr), std::invalid_argument );
        }
        {
            double minItems = 10;
            PropertyConstraint constraint(OM::resolve("transportCapacity"), PropertyConstraint::GREATER_EQUAL, minItems);
            PropertyConstraint::Set constraints;
            constraints.insert(constraint);
            transportProvider.setPropertyConstraints(constraints);
            ModelPool::Set combinations = ask.getResourceSupport(transportProvider);
            BOOST_REQUIRE_MESSAGE(combinations.size() >= 1, "Combinations supporting transport for minimum of " << minItems << " items: '" << ModelPool::toString(combinations) << "'");
        }
        {
            double minItems = 30;
            PropertyConstraint constraint(OM::resolve("transportCapacity"), PropertyConstraint::GREATER_EQUAL, minItems);
            PropertyConstraint::Set constraints;
            constraints.insert(constraint);
            transportProvider.setPropertyConstraints(constraints);

            ModelPool::Set combinations = ask.getResourceSupport(transportProvider);
            BOOST_REQUIRE_MESSAGE(combinations.empty(), "No combinations supporting transport for minimum of " << minItems << " items: '" << ModelPool::toString(combinations) << "'");
        }
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 4;
        modelPool[crex] = 10;

        // Use functional saturation bound
        OrganizationModelAsk ask(om, modelPool, true);
        Functionality::Set functionalities;

        Functionality transportProvider( OM::resolve("TransportProvider") );
        Functionality stereoImageProvider( OM::resolve("StereoImageProvider") );
        functionalities.insert( transportProvider );
        functionalities.insert( stereoImageProvider );

        {
            ModelPool::Set combinations = ask.getResourceSupport(functionalities);
            BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting : " << transportProvider.toString() << " and " << stereoImageProvider.toString() << " with model pool: " << modelPool.toString(12) << "\n '" << ModelPool::toString(combinations) << "' (functional saturation bound is active)");
        }

        {
            double minItems = 10;
            PropertyConstraint constraint(OM::resolve("transportCapacity"), PropertyConstraint::GREATER_EQUAL, minItems);
            PropertyConstraint::Set constraints;
            constraints.insert(constraint);
            transportProvider.setPropertyConstraints(constraints);
            ModelPool::Set combinations = ask.getResourceSupport(transportProvider);
            BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting transport for minimum of " << minItems << " items: '" << ModelPool::toString(combinations) << "'");
        }
        {
            double minItems = 30;
            PropertyConstraint constraint(OM::resolve("transportCapacity"), PropertyConstraint::GREATER_EQUAL, minItems);
            PropertyConstraint::Set constraints;
            constraints.insert(constraint);
            transportProvider.setPropertyConstraints(constraints);
            ModelPool::Set combinations = ask.getResourceSupport(transportProvider);
            BOOST_REQUIRE_MESSAGE(!combinations.empty(), "Combinations supporting transport for minimum of " << minItems << " items: '" << ModelPool::toString(combinations) << "'");
        }
    }
}

BOOST_AUTO_TEST_CASE(get_functionalities)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));

    IRI sherpa = OM::resolve("Sherpa");
    IRI crex = OM::resolve("CREX");
    IRI payload = OM::resolve("Payload");
    IRI payloadCamera = OM::resolve("PayloadCamera");

    {
        ModelPool modelPool;
        modelPool[sherpa] = 2;
        modelPool[payload] = 10;
        OrganizationModelAsk ask(om, modelPool);

        {
            owlapi::model::IRIList functionalities = ask.getSupportedFunctionalities(ModelPool());
            BOOST_REQUIRE_MESSAGE(functionalities.empty(), "Supported functionalities should be empty for empty model pool");
        }
        {
            ModelPool modelPoolQuery;
            modelPoolQuery[sherpa] = 2;
            modelPoolQuery[payload] = 12;

            owlapi::model::IRIList functionalities = ask.getSupportedFunctionalities(modelPoolQuery);
            BOOST_REQUIRE_MESSAGE(!functionalities.empty(), "Supported functionalities by model pool: " << modelPoolQuery.toString() << ":" << functionalities);
        }
    }
}

BOOST_AUTO_TEST_CASE(to_string)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    ModelPool pool;
    pool.setResourceCount( OM::resolve("Sherpa"), 3);
    pool.setResourceCount( OM::resolve("CREX"), 2);
    pool.setResourceCount( OM::resolve("CoyoteIII"), 3);
    pool.setResourceCount( OM::resolve("BaseCamp"), 5);
    pool.setResourceCount( OM::resolve("Payload"), 10);

    bool applyFunctionalSaturationBound = true;
    OrganizationModelAsk ask(om, pool, applyFunctionalSaturationBound);
    BOOST_TEST_MESSAGE(ask.toString());
}

BOOST_AUTO_TEST_CASE(to_string_extended_scenario)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    ModelPool pool;
    pool.setResourceCount( OM::resolve("Sherpa"), 3);
    pool.setResourceCount( OM::resolve("CREX"), 2);
    pool.setResourceCount( OM::resolve("CoyoteIII"), 3);
    pool.setResourceCount( OM::resolve("BaseCamp"), 5);
    pool.setResourceCount( OM::resolve("Payload"), 25);
    pool.setResourceCount( OM::resolve("PayloadCamera"), 25);
    pool.setResourceCount( OM::resolve("PayloadBattery"), 25);

    bool applyFunctionalSaturationBound = true;
    OrganizationModelAsk ask(om, pool, applyFunctionalSaturationBound);
    BOOST_TEST_MESSAGE(ask.toString());
}

BOOST_AUTO_TEST_CASE(property_constraint_solver)
{
    {
        PropertyConstraint::List constraints;
        owlapi::model::IRI property = OM::resolve("transportCapacity");

        PropertyConstraint constraint0(property, PropertyConstraint::GREATER_EQUAL, 0);
        constraints.push_back(constraint0);

        PropertyConstraint constraint1(property, PropertyConstraint::LESS_EQUAL, 10);
        constraints.push_back(constraint1);

        ValueBound vb = PropertyConstraintSolver::merge(constraints);
        BOOST_REQUIRE_MESSAGE(vb.getMin() == 0 && vb.getMax() == 10, "ValueBound should be 0 < vb < 10, but was " << vb.toString());
    }
    {
        PropertyConstraint::List constraints;
        owlapi::model::IRI property = OM::resolve("transportCapacity");

        PropertyConstraint constraint0(property, PropertyConstraint::GREATER_EQUAL, 15);
        constraints.push_back(constraint0);

        PropertyConstraint constraint1(property, PropertyConstraint::LESS_EQUAL, 10);
        constraints.push_back(constraint1);

        BOOST_REQUIRE_THROW(PropertyConstraintSolver::merge(constraints), std::invalid_argument);
    }
    {
        PropertyConstraint::List constraints;
        owlapi::model::IRI property = OM::resolve("transportCapacity");

        PropertyConstraint constraint0(property, PropertyConstraint::GREATER_EQUAL, 15);
        constraints.push_back(constraint0);

        PropertyConstraint constraint1(property, PropertyConstraint::LESS_EQUAL, 20);
        constraints.push_back(constraint1);

        PropertyConstraint constraint2(property, PropertyConstraint::EQUAL, 17.5);
        constraints.push_back(constraint2);

        ValueBound vb = PropertyConstraintSolver::merge(constraints);
        BOOST_REQUIRE_MESSAGE(vb.getMin() == 17.5 && vb.getMax() == 17.5, "ValueBound should be 17.5, but was " << vb.toString());
    }
}

BOOST_AUTO_TEST_CASE(organization_structure_generation)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new OrganizationModel(getOMSchema()));
    IRI sherpa = OM::resolve("Sherpa");
    IRI payload = OM::resolve("Payload");
    IRI coyote = OM::resolve("CoyoteIII");
    IRI basecamp = OM::resolve("BaseCamp");
    IRI crex = OM::resolve("CREX");
    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Found feasible coalition structure for transport" << modelPool.toString(4));
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[coyote] = 3;
        modelPool[payload] = 3;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Found feasible coalition structure for transport" << modelPool.toString(4));
    }


    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 4;
        modelPool[coyote] = 3;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Found feasible coalition structure for transport" << modelPool.toString(4));
    }

    {
        ModelPool modelPool;
        modelPool[sherpa] = 1;
        modelPool[payload] = 4;
        modelPool[coyote] = 2;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Found feasible coalition structure for transport for " << modelPool.toString(4));
    }

    {
        ModelPool modelPool;
        modelPool[payload] = 4;
        modelPool[basecamp] = 2;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(csg.empty(), "No feasible coalition structure for transport for " << modelPool.toString(4));
    }
    {
        ModelPool modelPool;
        modelPool[payload] = 9;
        modelPool[sherpa] = 1;
        modelPool[coyote] = 1;
        modelPool[crex] = 1;
        modelPool[basecamp] = 1;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Feasible coalition structure for transport for " << modelPool.toString(4));
    }
}

BOOST_AUTO_TEST_CASE(organization_structure_generation_vrp)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om(new
            OrganizationModel(IRI("http://www.rock-robotics.org/2017/11/vrp#") ));

    owlapi::vocabulary::Custom vocabulary("http://www.rock-robotics.org/2017/11/vrp#");

    IRI vehicle = vocabulary.resolve("Vehicle");
    IRI commodity = vocabulary.resolve("Commodity");
    {
        ModelPool modelPool;
        modelPool[vehicle] = 1;
        modelPool[commodity] = 30;

        OrganizationModelAsk ask(om, modelPool, true);

        Resource::Set resources;
        resources.insert( Resource( OM::resolve("MoveTo") ) );

        ModelPool::List csg = ask.findFeasibleCoalitionStructure(modelPool,
                resources,
                1000);
        BOOST_REQUIRE_MESSAGE(!csg.empty(), "Found feasible coalition structure for transport" << modelPool.toString(4));
    }
}

BOOST_AUTO_TEST_CASE(robotpool)
{
    using namespace owlapi::vocabulary;
    using namespace owlapi::model;

    OrganizationModel::Ptr om = make_shared<OrganizationModel>(owlapi::model::IRI("http://www.rock-robotics.org/2018/10/robots/robotpool"));
    owlapi::vocabulary::Custom vocabulary("http://www.rock-robotics.org/2018/10/robots/extrabots#");

    IRI vehicle = vocabulary.resolve("WalkingBot");
    {
        ModelPool modelPool;
        modelPool[vehicle] = 2;

        OrganizationModelAsk ask(om, modelPool, true);
        facades::Robot robot(modelPool, ask);

        BOOST_TEST_MESSAGE("All TransportProvider instances: " << ask.ontology().allInstancesOf(vocabulary::OM::resolve("TransportProvider")));
        BOOST_TEST_MESSAGE("All types of WalkingBot instance: " << ask.ontology().allTypesOf(vehicle)); // returns class information
        BOOST_REQUIRE_MESSAGE( robot.isMobile(), "WalkingBot is mobile");
    }
}

BOOST_AUTO_TEST_CASE(cardinality_restrictions)
{
    owlapi::model::IRI iri("http://www.rock-robotics.org/2015/12/projects/TransTerrA");
    OrganizationModel::Ptr om = make_shared<OrganizationModel>(iri);

    ModelPool pool;
    pool[OM::resolve("Payload")] = 1;
    OrganizationModelAsk ask(om, pool, true);

    std::vector<OWLCardinalityRestriction::Ptr> r_required =
        ask.getRequiredCardinalities(pool, OM::resolve("has"));

    BOOST_REQUIRE_MESSAGE(!r_required.empty(), "Payload has restrictions" <<
            OWLCardinalityRestriction::toString(r_required));

}


BOOST_AUTO_TEST_SUITE_END()
