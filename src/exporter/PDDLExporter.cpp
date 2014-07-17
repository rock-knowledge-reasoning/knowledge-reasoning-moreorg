#include "PDDLExporter.hpp"
#include <pddl_planner/representation/Domain.hpp>
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

namespace owl_om {

pddl_planner::representation::Domain PDDLExporter::toDomain(const OrganizationModel& model)
{
    using namespace pddl_planner;
    pddl_planner::representation::Domain domain("om");
    domain.addRequirement("strips");
    domain.addRequirement("equality");
    domain.addRequirement("typing");
    domain.addRequirement("conditional-effects");

    // Adding types to the domain -> concepts
    IRIList klasses = model.ontology()->allClasses();
    BOOST_FOREACH(IRI klass, klasses)
    {
        domain.addType(klass.toString());
        LOG_DEBUG_S << "Domain: adding klass: '" << klass << "'";
    }

    IRIList instances = model.ontology()->allInstances();
    BOOST_FOREACH(IRI instance, instances)
    {
        IRI klassType = model.ontology()->typeOf( instance );
        LOG_DEBUG_S << "Domain: adding typed constant: '" << instance << "' of type '" << klassType << "'";
        try {
            domain.addConstant( pddl_planner::representation::Constant(instance.toString(), klassType.toString()) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    // Add predicates
    using namespace pddl_planner::representation;
    domain.addPredicate( Predicate("at", TypedItem("?x","Actor"), TypedItem("?l","Location")) );
    domain.addPredicate( Predicate("operative", TypedItem("?x","Actor")) );
    domain.addPredicate( Predicate("embodies", TypedItem("?x","Actor"), TypedItem("?y", "Actor")) );
    domain.addPredicate( Predicate("mobile", TypedItem("?x","Actor"), TypedItem("?y", "Actor")) );
    domain.addPredicate( Predicate("provides", TypedItem("?x", "Actor"), TypedItem("?s", "Service")) );

    // Add actions
    {
        pddl_planner::representation::Action move("move", TypedItem("?obj","Actor"), TypedItem("?m", "Location"), TypedItem("?l","Location"));
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
        pddl_planner::representation::Action reconfigure_join("physical_merge", TypedItem("?x", "Actor"), TypedItem("?y", "Actor"), TypedItem("?z", "Actor"));
        Expression precondition("and",
                Expression("not",
                    Expression("=","?x","?y")),
                Expression("not",
                    Expression("=","?x","?z")),
                Expression("not",
                    Expression("=","?y","?z")),
                Expression(EXISTS,
                    TypedItem("?l","Location"),
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
                    TypedItem("?l","Location"),
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

        pddl_planner::representation::Action reconfigure_split("physical_split", TypedItem("?x", "Actor"), TypedItem("?y", "Actor"), TypedItem("?z", "Actor"));
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


void PDDLExporter::save(const std::string& filename, const OrganizationModel& model)
{
    pddl_planner::representation::Domain domain = toDomain(model);

    std::ofstream outfile;
    outfile.open(filename.c_str());
    outfile << domain.toLISP();
    outfile.close();
}

}
