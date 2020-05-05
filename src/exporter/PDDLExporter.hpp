#ifndef ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP
#define ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP

#include <moreorg/OrganizationModelAsk.hpp>
#include <pddl_planner/representation/Domain.hpp>
#include <pddl_planner/representation/Problem.hpp>

namespace moreorg {

/**
 * \brief The PDDLExporter allows to export an instance of the OrganizationModel
 * to PDDL (Plan Domain Definition Language), which can be augmented to define a
 * planning problem in PDDL
 *
 * \detaile While the domain will contain all basic definition the problem, will
 * have to be augmented with further information before saving
 */
class PDDLExporter
{

public:
    PDDLExporter(const OrganizationModelAsk& ask, size_t maxCoalitionSize = 15);

    enum Keyword { AND,
        NOT,
        OR,
        WHEN,
        AT,
        ATOMIC,
        OPERATIVE,
        EMBODIES,
        MOBILE,
        PROVIDES,
        DISTANCE,
        TOTALCOST
    };
    static std::map<Keyword, std::string> KeywordTxt;

    /**
     * Convert the existing model to a PDDL domain description
     * \return Domain description
     */
    pddl_planner::representation::Domain toDomain();

    /**
     * Convert the existing model to a partial PDDL problem description
     * This will only lead to a partial description of the problem and has to be
     * extended regarding, e.g., locations (under :objects of the problem
     * definition) and the goal (under :goal) of the problem definition.
     * A complete definition might look like the following:
     * \verbatim
        (define (problem om-partial)
            (:domain om)
            (:objects
                l0 l1 l2 - Location)
            (:init
                (atomic Sherpa0)
                (provides Sherpa0 TransportProvider)
                (provides Sherpa0 StereoImageProvider)
                (provides Sherpa0 ManipulationProvider)
            )
            (:goal
                (at Sherpa0_Sherpa1 l2)
            )
        )
     \endverbatim
     * \return Problem description
     */
    pddl_planner::representation::Problem toProblem();

    /**
     * Save the organization models domain description to a file
     * \param filename Name of file
     */
    void saveDomain(const std::string& filename);

    /**
     * Save the problem description to a file
     */
    void saveProblem(const std::string& filename);

private:
    OrganizationModelAsk mAsk;
    size_t mMaxCoalitionSize;

    // All available agents types (as of the given model pool)
    ModelPool::Set mAgentTypes;
    // relationship: <model> provides <functionalities>
    std::map<ModelPool, owlapi::model::IRIList> mAgentType2Functionality;

    // relationship: <instance> is_a <model>
    std::map<std::string, ModelPool> mAgent2Type;
    // relationship: <instance> embodies <agent model instance>
    std::map<std::string, owlapi::model::IRIList> mAgentName2Instance;

    /**
     * All available agents types, based on the given model pool
     *
     * Creates all feasible combinations of a model pool up to
     * mMaxCoalitionSize
     */
    ModelPool::Set allAgentTypes() const;

    /**
     * Create all feasible agent instances of a given ModelPool
     * \return list of instances (as list of ModelCombinations) with atomic
     * agents as <IRI-Model><number>
     */
    std::vector<owlapi::model::IRIList> createAgentInstances(const ModelPool& pool);

    /**
     * Get the agent name for a given coalition
     * \param coalition Agent as list of atomic agents
     */
    std::string agentName(const owlapi::model::IRIList& agentInstance) const;

};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_EXPORTER_PDDL_EXPORTER_HPP
