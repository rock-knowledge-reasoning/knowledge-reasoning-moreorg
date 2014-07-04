#include "PDDLExporter.hpp"
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

    // model.knowledgeBase();
    
    // Adding types to the domain -> concepts
    IRIList klasses = model.knowledgeBase().allClasses();
    BOOST_FOREACH(IRI klass, klasses)
    {
        domain.addType(klass);
        LOG_DEBUG_S << "Domain: adding klass: '" << klass << "'";
    }

    IRIList instances = model.knowledgeBase().allInstances();
    BOOST_FOREACH(IRI instance, instances)
    {
        IRI klassType = model.knowledgeBase().typeOf( instance );
        LOG_DEBUG_S << "Domain: adding typed constant: '" << instance << "' of type '" << klassType << "'";
        try {
            domain.addConstant( pddl_planner::representation::Constant(instance, klassType) );
        } catch(const std::invalid_argument& e)
        {
            LOG_WARN_S << e.what();
        }
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
