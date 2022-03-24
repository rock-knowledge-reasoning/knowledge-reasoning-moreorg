#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP

#include "FunctionalityMapping.hpp"
#include "QueryCache.hpp"
#include "Service.hpp"
#include <moreorg/SharedPtr.hpp>
#include <owlapi/model/OWLOntology.hpp>
#include <stdint.h>

namespace moreorg {

class OrganizationModelAsk;
class OrganizationModelTell;

typedef std::vector<owlapi::model::IRIList> CandidatesList;

/**
 * \mainpage Organization modelling with OWL
 *
 * The main purpose of the organization model is to describe (reconfigurable)
 * multirobot systems so that one can reason about structure and function.
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
#include <moreorg/OrganizationModel.hpp>

using namespace moreorg;

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
     * Constructor to create an organization model from a given model
     * description identified through an IRI \see
     * owlapi::model::OWLOntologyIO::load
     */
    explicit OrganizationModel(const owlapi::model::IRI& iri);

    /**
     * Constructor to create an OrganizationModel from an existing description
     * file \param filename File name to an rdf/xml formatted ontology
     * description file
     */
    explicit OrganizationModel(const std::string& filename = "");

    owlapi::model::OWLOntology::Ptr ontology() { return mpOntology; }

    const owlapi::model::OWLOntology::Ptr ontology() const
    {
        return mpOntology;
    }

    /**
     * Perform a deep copy of the OrganizationModel, thus
     * changes on the copy will not affect the current model instance
     * \return Copy of this OrganizationModel
     */
    OrganizationModel copy() const;

    static std::string toString(const Pool2FunctionMap& poolFunctionMap,
                                uint32_t indent = 0);
    static std::string toString(const Function2PoolMap& functionPoolMap,
                                uint32_t indent = 0);

    static ModelPool combination2ModelPool(const ModelCombination& combination);
    static ModelCombination modelPool2Combination(const ModelPool& pool);
    static std::string toString(const ModelCombinationSet& combinations);

    static OrganizationModel::Ptr getInstance(const std::string& filename = "");
    /**
     * Get an organization model from a given IRI, i.e. check for existing
     * (installed) organization model file and tries to retrieve them otherwise
     */
    static OrganizationModel::Ptr getInstance(const owlapi::model::IRI& iri);

    /**
     * Reset / Clear the query cache
     */
    void resetQueryCache() { mQueryCache.clear(); }

private:
    /// Ontology that serves as basis for this organization model
    owlapi::model::OWLOntology::Ptr mpOntology;

protected:
    QueryCache mQueryCache;
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
