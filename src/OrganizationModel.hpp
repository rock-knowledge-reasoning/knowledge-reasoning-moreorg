#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP

#include <stdint.h>
#include <organization_model/SharedPtr.hpp>
#include <owlapi/model/OWLOntology.hpp>
#include <organization_model/FunctionalityMapping.hpp>
#include <organization_model/Functionality.hpp>
#include <organization_model/Service.hpp>

namespace organization_model {

class OrganizationModelAsk;
class OrganizationModelTell;

typedef std::vector<owlapi::model::IRIList> CandidatesList;

/**
 * \mainpage Organization modelling with OWL
 *
 * The main purpose of the organization model is to describe (reconfigurable)
 * multirobot systems so that one can reason about structure and funtion.
 * The organization model relies on a basic system
 * description -- provided in OWL -- to infer system combinations from it. 
 *
 * These system combinations (coalition / composite actors) have quantifiable
 * requirements. These requirements are defined by the combination of 
 * restrictions which apply to the model of atomic actors.
 *
 * The organization model allows to augment an organization model for a given
 * set of atomic actors.
 *
 * \verbatim
#include <organization_model/OrganizationModel.hpp>

using namespace organization_model;

OrganizationModel om;
Ontology::Ptr ontology = om.ontology();

 \endverbatim
 */
class OrganizationModel
{
    friend class OrganizationModelAsk;
    friend class OrganizationModelTell;

public:
    typedef shared_ptr<OrganizationModel> Ptr;

    /**
     * Constructor to create an OrganizationModel from an existing description file
     * \param filename File name to an rdf/xml formatted ontology description file
     */
    OrganizationModel(const std::string& filename = "");

    owlapi::model::OWLOntology::Ptr ontology() { return mpOntology; }

    const owlapi::model::OWLOntology::Ptr ontology() const { return mpOntology; }

    /**
     * Perform a deep copy of the OrganizationModel, thus 
     * changes on the copy will not affect the current model instance
     * \return Copy of this OrganizationModel
     */
    OrganizationModel copy() const;

    static std::string toString(const Pool2FunctionMap& poolFunctionMap);
    static std::string toString(const Function2PoolMap& functionPoolMap);

    static ModelPool combination2ModelPool(const ModelCombination& combination);
    static ModelCombination modelPool2Combination(const ModelPool& pool);
    static std::string toString(const ModelCombinationSet& combinations);

    static OrganizationModel::Ptr getInstance(const std::string& filename = "");

private:
    /// Ontology that serves as basis for this organization model
    owlapi::model::OWLOntology::Ptr mpOntology;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
