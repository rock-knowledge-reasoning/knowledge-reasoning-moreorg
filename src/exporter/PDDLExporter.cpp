#include "PDDLExporter.hpp"
#include <pddl_planner/representation/Domain.hpp>
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>
#include <owl_om/Vocabulary.hpp>

namespace owl_om {

pddl_planner::representation::Domain PDDLExporter::toDomain(const OrganizationModel& model)
{
    using namespace owl_om::vocabulary;
    using namespace pddl_planner;

    pddl_planner::representation::Domain domain("om");
    domain.addRequirement("strips");
    domain.addRequirement("equality");
    domain.addRequirement("typing");
    domain.addRequirement("conditional-effects");

    std::string actorType = OM::Actor().getFragment();
    std::string locationType = OM::Location().getFragment();
    // Adding types to the domain -> concepts
    IRIList klasses = model.ontology()->allClasses();
    BOOST_FOREACH(IRI klass, klasses)
    {
        LOG_DEBUG_S << "Domain: adding klass: '" << klass << "'";
        try {
            domain.addType(klass.getFragment());
            LOG_DEBUG_S << "Domain: adding klass: '" << klass << "' as '" << klass.getFragment() << "'";
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << "Domain: not adding klass: " << e.what();
        }
    }

    LOG_DEBUG_S << "All instance of actor";
    IRIList instances = model.ontology()->allInstancesOf( OM::Actor(), false);
    BOOST_FOREACH(IRI instance, instances)
    {
        LOG_DEBUG_S << "Domain: adding typed constant: '" << instance << "' of type '" << klassType << "'";
        try {
            LOG_DEBUG_S << "Domain: adding typed constant: '" << instance << "' of type '" << OM::Actor() << "'";
            std::string instanceName = instance.getFragment();
            domain.addConstant( pddl_planner::representation::Constant(instanceName, actorType) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    // Add predicates
    using namespace pddl_planner::representation;
    domain.addPredicate( Predicate("at", TypedItem("?x", actorType), TypedItem("?l", locationType)) );
    domain.addPredicate( Predicate("operative", TypedItem("?x",actorType)) );
    domain.addPredicate( Predicate("embodies", TypedItem("?x",actorType), TypedItem("?y", actorType)) );
    domain.addPredicate( Predicate("mobile", TypedItem("?x",actorType), TypedItem("?y", actorType)) );
    domain.addPredicate( Predicate("provides", TypedItem("?x", actorType), TypedItem("?s", actorType)) );

    // Add actions
    {
        pddl_planner::representation::Action move("move", TypedItem("?obj", actorType), TypedItem("?m", locationType), TypedItem("?l", locationType));
        Expression precondition("and",
            Expression("at","?obj","?m"),
            Expression("not",
                Expression("=","?m","?l")),
            Expression("mobile", "?obj"),
            Expression("operative", "?obj")
        );
        Expression effect("and",
            Expression("at","?obj","?l"),
            Expression("not",
                Expression("at","?obj","?m"))
        );

        move.addPrecondition(precondition);
        move.addEffect(effect);
        domain.addAction(move);
    }
    {
        pddl_planner::representation::Action reconfigure_join("physical_merge", TypedItem("?x", actorType), TypedItem("?y", actorType), TypedItem("?z", actorType));
        Expression precondition("and",
                Expression("not",
                    Expression("=","?x","?y")),
                Expression("not",
                    Expression("=","?x","?z")),
                Expression("not",
                    Expression("=","?y","?z")),
                Expression(EXISTS,
                    TypedItem("?l", locationType),
                    Expression("and",
                        Expression("at","?x","?l"),
                        Expression("at","?y","?l"))
                ),
                Expression("not",
                    Expression("operative","?z")),
                Expression("operative","?x"),
                Expression("operative","?y"),
                Expression("embodies","?z","?x"),
                Expression("embodies","?z","?y")
        );

        Expression effect("and",
                Expression("not",
                    Expression("operative","?y")),
                Expression("not",
                    Expression("operative","?x")),
                Expression("operative","?z"),
                Expression(FORALL,
                    TypedItem("?l", locationType),
                    Expression("when",
                        Expression("at","?x","?l"),
                        Expression("and",
                            Expression("not",
                                Expression("at","?x","?l")),
                            Expression("not",
                                Expression("at","?y","?l")),
                            Expression("at","?z","?l"))
                    ) // end Expression 'when'
                ) // end Expression 'forall'
        );

        reconfigure_join.addPrecondition(precondition);
        reconfigure_join.addEffect(effect);
        domain.addAction(reconfigure_join);
    }
    {

        pddl_planner::representation::Action reconfigure_split("physical_split", TypedItem("?x", actorType), TypedItem("?y", actorType), TypedItem("?z", actorType));
        Expression precondition("and",
                Expression("not",
                    Expression("=","?x","?y")),
                Expression("not",
                    Expression("=","?x","?z")),
                Expression("not",
                    Expression("=","?y","?z")),
                Expression("not",
                    Expression("operative","?x")),
                Expression("not",
                    Expression("operative","?y")),
                Expression("operative","?z"),
                Expression("embodies","?z","?x"),
                Expression("embodies","?z","?y")
        );

        Expression effect("and",
                // Make sure status of operation changes
                Expression("not",
                    Expression("operative","?z")),
                Expression("operative","?x"),
                Expression("operative","?y"),
                // Make sure location is properly set for released
                Expression(FORALL,
                    TypedItem("?l", "Location"),
                    Expression("when", // first param condition, second effect
                        Expression("at","?z","?l"),
                        Expression("and",
                            Expression("at","?x","?l"),
                            Expression("at","?y","?l"),
                            Expression("not",
                                Expression("at","?z","?l")
                            )
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
    using namespace owl_om::vocabulary;

    using namespace pddl_planner;
    using namespace pddl_planner::representation;

    pddl_planner::representation::Problem problem("om-partial", toDomain(model));

    IRIList instances = model.ontology()->allInstancesOf( OM::Actor(), false);
    BOOST_FOREACH(IRI instance, instances)
    {
        try {
            std::string instanceName = instance.getFragment();
            IRIList relatedActors = model.ontology()->allRelatedInstances(instance, OM::has(), OM::Actor());
            BOOST_FOREACH(IRI relatedActor, relatedActors)
            {
                // embodies: CombinedActor embodies Actor
                problem.addInitialStatus( Expression("embodies", instanceName, relatedActor.getFragment()) );
            }

            // mobile: general characteristics

            // provides: Actor provides Capability / Service

        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
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
