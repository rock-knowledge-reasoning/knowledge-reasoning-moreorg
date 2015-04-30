#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP

#include <stdint.h>
#include <owlapi/model/OWLOntology.hpp>
#include <numeric/LimitedCombination.hpp>

namespace owlapi {
namespace model {

class OWLOntologyTell;
class OWLOntologyAsk;

} // end namespace model
} // end namespace owlapi

namespace organization_model {

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
public:

    typedef boost::shared_ptr<OrganizationModel> Ptr;
    typedef owlapi::model::IRIList ModelCombination;
    typedef std::vector<ModelCombination> ModelCombinationList;
    typedef std::map<owlapi::model::IRI, size_t> ModelPool;

    /// Maps a 'combined system' to the functionality it can 'theoretically'
    /// provide when looking at its resources
    typedef std::map<ModelCombination, owlapi::model::IRIList> Combination2FunctionMap;
    typedef std::map<owlapi::model::IRI, ModelCombinationList > Function2CombinationMap;

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

    /**
     * Prepare the organization model for a given set of available models
     */
    void prepare(const ModelPool& modelPool);

    /**
     * Retrieve the list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    static std::string toString(const Combination2FunctionMap& combinationFunctionMap);

    static std::string toString(const Function2CombinationMap& functionCombinationMap);

    const Combination2FunctionMap& getCombination2FunctionMap() const { return mCombination2Function; }

    const Function2CombinationMap& getFunction2CombinationMap() const { return mFunction2Combination; }

private:
    /// Ontology that serves as basis for this organization model
    owlapi::model::OWLOntology::Ptr mpOntology;

    boost::shared_ptr<owlapi::model::OWLOntologyAsk> mpAsk;
    boost::shared_ptr<owlapi::model::OWLOntologyTell> mpTell;

    /// Maps a combination to its supported functionality
    Combination2FunctionMap mCombination2Function;
    /// Maps a functionality to combination that support this functionality
    Function2CombinationMap mFunction2Combination;

    /**
     * Compute the functionality maps for the combination of models from a
     * limited set of available models
     */
    void computeFunctionalityMaps(const ModelPool& modelPool);

    static ModelPool combination2ModelPool(const ModelCombination& combination);
    static ModelCombination modelPool2Combination(const ModelPool& pool);
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
