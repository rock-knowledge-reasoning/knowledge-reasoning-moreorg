#include "PDDLExporter.hpp"
#include <iostream>
#include <fstream>
#include <base-logging/Logging.hpp>
#include <numeric/Combinatorics.hpp>
#include <pddl_planner/representation/Domain.hpp>
#include "../OrganizationModelAsk.hpp"
#include "../vocabularies/OM.hpp"
#include "../facades/Robot.hpp"

using namespace owlapi::model;

namespace organization_model {

std::map<PDDLExporter::Keyword, std::string> PDDLExporter::KeywordTxt =
{
    { PDDLExporter::AND, "and"},
    { PDDLExporter::NOT, "not"},
    { PDDLExporter::OR, "or"},
    { PDDLExporter::WHEN, "when" },
    { PDDLExporter::ATOMIC, "atomic" },
    { PDDLExporter::AT, "at" },
    { PDDLExporter::OPERATIVE, "operative" },
    { PDDLExporter::EMBODIES, "embodies" },
    { PDDLExporter::MOBILE, "mobile" },
    { PDDLExporter::PROVIDES, "provides" },
    { PDDLExporter::DISTANCE, "distance" },
    { PDDLExporter::TOTALCOST, "total-cost" }
};

PDDLExporter::PDDLExporter(const OrganizationModelAsk& ask,
        size_t maxCoalitionSize)
    : mAsk(ask)
    , mMaxCoalitionSize(maxCoalitionSize)
{
}

pddl_planner::representation::Domain PDDLExporter::toDomain()
{
    using namespace owlapi::vocabulary;
    using namespace organization_model::vocabulary;
    using namespace pddl_planner;

    pddl_planner::representation::Domain domain("om");
    domain.addRequirement("strips");
    domain.addRequirement("equality");
    domain.addRequirement("typing");
    domain.addRequirement("conditional-effects");
    domain.addRequirement("action-costs");

    domain.addType(OM::Actor().getFragment());
    domain.addType(OM::Location().getFragment());

    IRIList baseKlasses{ OM::Actor() };
    for(const IRI& baseKlass : baseKlasses)
    {
        // Adding types to the domain -> concepts
        IRIList klasses = mAsk.ontology().allSubClassesOf(baseKlass);
        for(const IRI& klass : klasses)
        {
            try {
                domain.addType(klass.getFragment(), OM::Actor().getFragment());
            } catch(const std::invalid_argument& e)
            {
                LOG_WARN_S << "Domain: failed to add klass: " << e.what();
            }
        }
    }

    mAgentTypes = allAgentTypes();
    for(const ModelPool& pool : mAgentTypes)
    {
        // Agent instances as list of ModelCombination
        std::vector<IRIList> agentInstances = createAgentInstances(pool);
        for(const IRIList& instance : agentInstances)
        {
            std::string name = agentName(instance);
            mAgentName2Instance[name] = instance;
            mAgent2Type[name] = pool;

            try {
                domain.addConstant( pddl_planner::representation::Constant(name, OM::Actor().getFragment()) );
            } catch(const std::invalid_argument& e)
            {
                LOG_WARN_S << e.what();
            }
        }
    }

    mAgentType2Functionality.clear();
    OrganizationModelAsk ask(mAsk.getOrganizationModel(), mAsk.getModelPool(), false);
    IRIList functionalities = mAsk.ontology().allSubClassesOf( OM::Functionality(), false);
    for(const owlapi::model::IRI& functionality : functionalities)
    {
        Resource::Set functionalitySet;
        functionalitySet.insert( Resource(functionality) );

        // Adding the functionality constant
        try {
            std::string instanceName = functionality.getFragment();
            domain.addConstant( pddl_planner::representation::Constant(instanceName, OM::Functionality().getFragment()) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << "Domain: failed to add constant: " << e.what();
        }


        // Here we have to assume that the saturation bound is not set for the
        // organization model ask
        ModelPool::Set supportedModels = ask.getResourceSupport(functionalitySet);
        for(const ModelPool& m : supportedModels)
        {
            mAgentType2Functionality[m].push_back(functionality);
        }
    }


    std::string actorType = OM::Actor().getFragment();
    std::string locationType = OM::Location().getFragment();
    std::string functionalityType = OM::Functionality().getFragment();
    // Add predicates
    using namespace pddl_planner::representation;
    domain.addPredicate( Predicate(KeywordTxt[AT], TypedItem("?x", actorType), TypedItem("?l", locationType)) );
    domain.addPredicate( Predicate(KeywordTxt[OPERATIVE], TypedItem("?x",actorType)) );
    domain.addPredicate( Predicate(KeywordTxt[EMBODIES], TypedItem("?x",actorType), TypedItem("?y", actorType)) );
    domain.addPredicate( Predicate(KeywordTxt[MOBILE], TypedItem("?x",actorType)) );
    domain.addPredicate( Predicate(KeywordTxt[PROVIDES], TypedItem("?x", actorType), TypedItem("?s", functionalityType)) );

    // Add functions
    domain.addFunction( Function(KeywordTxt[DISTANCE], TypedItem("?start", locationType), TypedItem("?end", locationType)));
    domain.addFunction( Function(KeywordTxt[TOTALCOST]) );


    // Add actions
    {
        pddl_planner::representation::Action move("move", TypedItem("?obj", actorType), TypedItem("?m", locationType), TypedItem("?l", locationType));
        Expression precondition(KeywordTxt[AND],
            Expression(KeywordTxt[AT],"?obj","?m"),
            Expression(KeywordTxt[NOT],
                Expression("=","?m","?l")),
            Expression(KeywordTxt[MOBILE], "?obj"),
            Expression(KeywordTxt[OPERATIVE], "?obj")
        );
        Expression effect(KeywordTxt[AND],
            Expression(KeywordTxt[AT],"?obj","?l"),
            Expression(KeywordTxt[NOT],
                Expression(KeywordTxt[AT],"?obj","?m"))
        );

        move.addPrecondition(precondition);
        move.addEffect(effect);
        domain.addAction(move);
    }
    {
        pddl_planner::representation::Action reconfigure_join("physical_merge", TypedItem("?z", actorType), TypedItem("?l", locationType));
        Expression precondition(KeywordTxt[AND],
                Expression(FORALL,
                    TypedItem("?a",actorType),
                    Expression(KeywordTxt[OR],
                        Expression(KeywordTxt[AND],
                            Expression(KeywordTxt[EMBODIES], "?z", "?a"),
                            Expression(KeywordTxt[OPERATIVE], "?a"),
                            Expression(KeywordTxt[AT], "?a", "?l")
                        ),
                        Expression(KeywordTxt[NOT],
                            Expression(KeywordTxt[EMBODIES], "?z", "?a"))
                    )
                ),
                Expression(KeywordTxt[NOT],
                    Expression(KeywordTxt[ATOMIC],"?z")),
                Expression(KeywordTxt[NOT],
                    Expression(KeywordTxt[OPERATIVE],"?z"))
        );

        Expression effect(KeywordTxt[AND],
                Expression(FORALL,
                    TypedItem("?a", actorType),
                    Expression(KeywordTxt[WHEN],
                        Expression(KeywordTxt[EMBODIES],"?z","?a"),
                        Expression(KeywordTxt[AND],
                            Expression(KeywordTxt[NOT],
                                Expression(KeywordTxt[AT],"?a","?l")),
                            Expression(KeywordTxt[NOT],
                                Expression(KeywordTxt[OPERATIVE],"?a"))
                            )
                    ) // end when
                ), // end forall
                Expression(KeywordTxt[OPERATIVE],"?z"),
                Expression(KeywordTxt[AT],"?z","?l")
        ); // end Expression 'forall'

        reconfigure_join.addPrecondition(precondition);
        reconfigure_join.addEffect(effect);
        domain.addAction(reconfigure_join);
    }
    {

        pddl_planner::representation::Action reconfigure_split("physical_split", TypedItem("?z", actorType), TypedItem("?l", locationType));
        Expression precondition(KeywordTxt[AND],
                Expression(KeywordTxt[OPERATIVE],"?z"),
                Expression(KeywordTxt[AT],"?z","?l")
        );

        Expression effect(KeywordTxt[AND],
                // Make sure status of operation changes
                Expression(KeywordTxt[NOT],
                    Expression(KeywordTxt[OPERATIVE],"?z")),
                Expression(KeywordTxt[NOT],
                    Expression(KeywordTxt[AT],"?z","?l")),
                // Make sure location is properly set for released
                Expression(FORALL,
                    TypedItem("?a", actorType),
                    Expression(KeywordTxt[WHEN], // first param condition, second effect
                        Expression(KeywordTxt[EMBODIES],"?z","?a"),
                        Expression(KeywordTxt[AND],
                            Expression(KeywordTxt[OPERATIVE],"?a"),
                            Expression(KeywordTxt[AT],"?a","?l")
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

pddl_planner::representation::Problem PDDLExporter::toProblem()
{
    mAgent2Type.clear();
    mAgentType2Functionality.clear();

    using namespace owlapi::vocabulary;
    using namespace organization_model::vocabulary;

    using namespace pddl_planner;
    using namespace pddl_planner::representation;

    pddl_planner::representation::Problem problem("om-partial", toDomain());

    for(const std::pair<std::string, IRIList>& instance : mAgentName2Instance)
    {
        try {
            const std::string& instanceName = instance.first;
            const IRIList& relatedActors = instance.second;

            if( relatedActors.size() == 1)
            {
                problem.addInitialStatus( Expression(KeywordTxt[ATOMIC], instanceName) );
            } else {
                // in a composite system identify the individual atomic agents
                for(const IRI& relatedActor : relatedActors)
                {
                    problem.addInitialStatus( Expression(KeywordTxt[EMBODIES], instanceName, relatedActor.getFragment()) );
                }
            }

            // provides: Actor provides Capability / Service
            const ModelPool& agentType = mAgent2Type[instanceName];
            IRIList functionalities = mAgentType2Functionality[agentType];
            for(const IRI& related : functionalities)
            {
                problem.addInitialStatus( Expression(KeywordTxt[PROVIDES], instanceName, related.getFragment()) );

            }

            // Mobility is handled specially
            facades::Robot robot(agentType, mAsk);
            if( robot.isMobile() )
            {
                problem.addInitialStatus( Expression(KeywordTxt[MOBILE], instanceName) );
            }
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
    }

    return problem;
}


void PDDLExporter::saveDomain(const std::string& filename)
{
    std::ofstream outfile;
    outfile.open(filename.c_str());
    outfile << toDomain().toLISP();
    outfile.close();
}

void PDDLExporter::saveProblem(const std::string& filename)
{
    std::ofstream outfile;
    outfile.open(filename.c_str());
    outfile << toProblem().toLISP();
    outfile.close();
}

std::vector<owlapi::model::IRIList> PDDLExporter::createAgentInstances(const ModelPool& modelPool)
{
    // First identify all atomic agent instances
    IRIList atomicAgents;
    for(const ModelPool::value_type& m : modelPool)
    {
        for(size_t i = 0; i < m.second; ++i)
        {
            std::stringstream ss;
            ss << m.first << i;
            atomicAgents.push_back(IRI(ss.str()));
        }
    }


    // Now with all instances, create the combinations
    std::vector<IRIList> instances;
    size_t size = modelPool.numberOfInstances();
    numeric::Combination<IRI> combinations(atomicAgents, size, numeric::MAX);
    do
    {
        IRIList combination = combinations.current();
        instances.push_back(combination);
    } while(combinations.next());

    return instances;
}

std::string PDDLExporter::agentName(const owlapi::model::IRIList& agentInstance) const
{
    std::stringstream ss;
    owlapi::model::IRIList::const_iterator cit = agentInstance.begin();
    for(; cit != agentInstance.end(); ++cit)
    {
        const IRI& iri = *cit;
        ss << iri.getFragment();
        if(cit + 1 != agentInstance.end())
        {
            ss << "_";
        }
    }
    return ss.str();
}

ModelPool::Set PDDLExporter::allAgentTypes() const
{
    return mAsk.getModelPool().allCombinations(mMaxCoalitionSize);
}

}
