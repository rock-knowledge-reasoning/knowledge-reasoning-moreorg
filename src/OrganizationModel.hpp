#ifndef ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
#define ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP

#include <stdint.h>
#include <owlapi/model/OWLOntology.hpp>

namespace owlapi {
namespace model {

class OWLOntologyTell;
class OWLOntologyAsk;

} // end namespace model
} // end namespace owlapi

namespace organization_model {

class Service
{
public:
    typedef double QualityOfService;
    typedef double Duration;
    typedef double Safety;

    Service(const owlapi::model::IRI& model)
        : mModel(model)
    {}

    const owlapi::model::IRI& getModel() const { return mModel; }

private:
    owlapi::model::IRI mModel;
    /// How good shall be the quality
    QualityOfService mQos;
    /// How low shall the service be provided
    Duration mDurationInS;
    /// How safe (redundancy level, etc. shall be the execution)
    Safety mSafety;
};

typedef std::vector<Service> ServiceList;


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

public:
    typedef boost::shared_ptr<OrganizationModel> Ptr;

    typedef owlapi::model::IRIList ModelCombination;
    typedef std::vector<ModelCombination> ModelCombinationList;
    typedef std::map<owlapi::model::IRI, size_t> ModelPool;
    typedef std::map<owlapi::model::IRI, int> ModelPoolDelta;

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

    static std::string toString(const Combination2FunctionMap& combinationFunctionMap);

    static std::string toString(const Function2CombinationMap& functionCombinationMap);

protected:
    boost::shared_ptr<owlapi::model::OWLOntologyAsk> ask() { return mpAsk; }
    boost::shared_ptr<owlapi::model::OWLOntologyTell> tell() { return mpTell; }

private:
    /// Ontology that serves as basis for this organization model
    owlapi::model::OWLOntology::Ptr mpOntology;

    boost::shared_ptr<owlapi::model::OWLOntologyAsk> mpAsk;
    boost::shared_ptr<owlapi::model::OWLOntologyTell> mpTell;

};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ORGANIZATION_MODEL_HPP
