#ifndef ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP 
#define ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP 

#include <organization_model/OrganizationModel.hpp>
#include <pddl_planner/representation/Domain.hpp>
#include <pddl_planner/representation/Problem.hpp>

namespace organization_model {

/**
 * \brief The PDDLExporter allow to export the an instance of OrganizationModel
 * to PDDL (Plan Domain Definition Language).
 */
class PDDLExporter
{

public:

    /**
     * Convert the existing model to a PDDL domain description
     * \return Domain description
     */
    pddl_planner::representation::Domain toDomain(const OrganizationModel& model);

    /**
     * Convert the existing model to a partial PDDL problem description
     * \return Problem description
     */
    pddl_planner::representation::Problem toProblem(const OrganizationModel& model);

    /**
     * Save the organization models domain description to a file
     * \param filename Name of file
     * \param model OrganizationModel that shall be saved
     */
    void save(const std::string& filename, const OrganizationModel& model);

};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP 
