#include "PDDLExporter.hpp"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <base/Logging.hpp>
#include <pddl_planner/representation/Domain.hpp>
#include <owlapi/Vocabulary.hpp>

using namespace owlapi::model;

namespace organization_model {

pddl_planner::representation::Domain PDDLExporter::toDomain(const OrganizationModel& model)
{
    using namespace owlapi::vocabulary;
    using namespace pddl_planner;

    pddl_planner::representation::Domain domain("om");
    domain.addRequirement("strips");
    domain.addRequirement("equality");
    domain.addRequirement("typing");
    domain.addRequirement("conditional-effects");
    domain.addRequirement("action-costs");

    std::string actorType = OM::Actor().getFragment();
    std::string locationType = OM::Location().getFragment();
    std::string serviceType = OM::Service().getFragment();

    // Adding types to the domain -> concepts
    owlapi::model::OWLOntologyAsk ask(model.ontology());
    IRIList klasses = ask.allClasses();
    BOOST_FOREACH(IRI klass, klasses)
    {
        try {
            domain.addType(klass.getFragment());
            LOG_DEBUG_S << "Domain: adding klass: '" << klass << "' as '" << klass.getFragment() << "'";
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << "Domain: not adding klass: " << e.what();
        }
    }

    LOG_DEBUG_S << "All instance of actor";
    IRIList instances = ask.allInstancesOf( OM::Actor(), false);
    BOOST_FOREACH(IRI instance, instances)
    {
        try {
            LOG_DEBUG_S << "Domain: adding typed constant: '" << instance << "' of type '" << OM::Actor() << "'";
            std::string instanceName = instance.getFragment();
            domain.addConstant( pddl_planner::representation::Constant(instanceName, actorType) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }
    LOG_DEBUG_S << "All services";
    IRIList services = ask.allInstancesOf( OM::ServiceModel(), false);
    BOOST_FOREACH(IRI service, services)
    {
        try {
            LOG_DEBUG_S << "Domain: adding typed constant: '" << service << "' of type '" << OM::ServiceModel() << "'";
            std::string instanceName = service.getFragment();
            domain.addConstant( pddl_planner::representation::Constant(instanceName, serviceType) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    std::string at = "at";
    std::string operative = "operative";
    std::string embodies = "embodies";
    std::string mobile = "mobile";
    std::string provides = "provides";

    // Add predicates
    using namespace pddl_planner::representation;
    domain.addPredicate( Predicate(at, TypedItem("?x", actorType), TypedItem("?l", locationType)) );
    domain.addPredicate( Predicate(operative, TypedItem("?x",actorType)) );
    domain.addPredicate( Predicate(embodies, TypedItem("?x",actorType), TypedItem("?y", actorType)) );
    domain.addPredicate( Predicate(mobile, TypedItem("?x",actorType)) );
    domain.addPredicate( Predicate(provides, TypedItem("?x", actorType), TypedItem("?s", serviceType)) );

    // Add functions
    domain.addFunction( Function("distance", TypedItem("?start", locationType), TypedItem("?end", locationType)));
    domain.addFunction( Function("total-cost") );


    // Add actions
    {
        pddl_planner::representation::Action move("move", TypedItem("?obj", actorType), TypedItem("?m", locationType), TypedItem("?l", locationType));
        Expression precondition("and",
            Expression(at,"?obj","?m"),
            Expression("not",
                Expression("=","?m","?l")),
            Expression(mobile, "?obj"),
            Expression(operative, "?obj")
        );
        Expression effect("and",
            Expression(at,"?obj","?l"),
            Expression("not",
                Expression(at,"?obj","?m"))
        );

        move.addPrecondition(precondition);
        move.addEffect(effect);
        domain.addAction(move);
    }
    {
        pddl_planner::representation::Action reconfigure_join("physical_merge", TypedItem("?z", actorType), TypedItem("?l", locationType));
        Expression precondition("and",
                Expression(FORALL,
                    TypedItem("?a",actorType),
                    Expression("or",
                        Expression("and",
                            Expression(embodies, "?z", "?a"),
                            Expression(operative, "?a"),
                            Expression(at, "?a", "?l")
                        ),
                        Expression("not",
                            Expression(embodies, "?z", "?a"))
                    )
                ),
                Expression("not",
                    Expression("atomic","?z")),
                Expression("not",
                    Expression(operative,"?z"))
        );

        Expression effect("and",
                Expression(FORALL,
                    TypedItem("?a", actorType),
                    Expression("when",
                        Expression(embodies,"?z","?a"),
                        Expression("and",
                            Expression("not",
                                Expression(at,"?a","?l")),
                            Expression("not",
                                Expression(operative,"?a"))
                            )
                    ) // end when
                ), // end forall
                Expression(operative,"?z"),
                Expression(at,"?z","?l")
        ); // end Expression 'forall'

        reconfigure_join.addPrecondition(precondition);
        reconfigure_join.addEffect(effect);
        domain.addAction(reconfigure_join);
    }
    {

        pddl_planner::representation::Action reconfigure_split("physical_split", TypedItem("?z", actorType), TypedItem("?l", locationType));
        Expression precondition("and",
                Expression(operative,"?z"),
                Expression(at,"?z","?l")
        );

        Expression effect("and",
                // Make sure status of operation changes
                Expression("not",
                    Expression(operative,"?z")),
                Expression("not",
                    Expression(at,"?z","?l")),
                // Make sure location is properly set for released
                Expression(FORALL,
                    TypedItem("?a", actorType),
                    Expression("when", // first param condition, second effect
                        Expression(embodies,"?z","?a"),
                        Expression("and",
                            Expression(operative,"?a"),
                            Expression(at,"?a","?l")
                        )
                    ) // end Expression 'when'
                ) // end Expression 'forall'
        ); // end effect

        reconfigure_split.addPrecondition(precondition);
        reconfigure_split.addEffect(effect);
        domain.addAction(reconfigure_split);
    }

    return domain;
}

pddl_planner::representation::Problem PDDLExporter::toProblem(const OrganizationModel& model)
{
    using namespace owlapi::vocabulary;

    using namespace pddl_planner;
    using namespace pddl_planner::representation;

    pddl_planner::representation::Problem problem("om-partial", toDomain(model));

    std::string atomic = "atomic";
    std::string embodies = "embodies";
    std::string mobile = "mobile";
    std::string provides = "provides";
    std::string mobilityCapability = "MoveTo";

    owlapi::model::OWLOntologyAsk ask(model.ontology());
    IRIList instances = ask.allInstancesOf( OM::Actor(), false);
    BOOST_FOREACH(IRI instance, instances)
    {
        try {
            std::string instanceName = instance.getFragment();
            IRIList relatedActors = ask.allRelatedInstances(instance, OM::has(), OM::Actor());
            BOOST_FOREACH(IRI relatedActor, relatedActors)
            {
                // embodies: CombinedActor embodies Actor
                problem.addInitialStatus( Expression(embodies, instanceName, relatedActor.getFragment()) );
            }

            // provides: Actor provides Capability / Service
            IRIList relatedServicesOrCapabilities = ask.allRelatedInstances(instance, OM::provides());
            BOOST_FOREACH(IRI related, relatedServicesOrCapabilities)
            {
                if(ask.isInstanceOf(related, OM::ServiceModel()))
                {
                    problem.addInitialStatus( Expression(provides, instanceName, related.getFragment()) );
                }

                if(ask.isInstanceOf(related, OM::CapabilityModel()))
                {
                    // mobile: general characteristics
                    if(related.getFragment() == mobilityCapability)
                    {
                        problem.addInitialStatus( Expression(mobile, instanceName) );
                    }
                }
            }
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    // All atomic actors
    IRIList atomicActors = ask.allInstancesOf( OM::Actor(), true);
    BOOST_FOREACH(IRI atomicActor, atomicActors)
    {
        std::string actorName = atomicActor.getFragment();
        problem.addInitialStatus( Expression(atomic, actorName) );
    }

    return problem;
}


void PDDLExporter::save(const std::string& filename, const OrganizationModel& model)
{
    pddl_planner::representation::Domain domain = toDomain(model);

    std::ofstream outfile;
    outfile.open(filename.c_str());
    outfile << domain.toLISP();
    outfile.close();
}

}
