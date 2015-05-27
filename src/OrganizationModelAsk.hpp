#ifndef ORGANIZATION_MODEL_ASK_HPP
#define ORGANIZATION_MODEL_ASK_HPP

#include <owlapi/model/OWLCardinalityRestriction.hpp>
#include <owlapi/model/OWLOntologyAsk.hpp>
#include <organization_model/OrganizationModel.hpp>
#include <organization_model/algebra/ResourceSupportVector.hpp>

namespace organization_model {

class OrganizationModelAsk
{
    friend class algebra::ResourceSupportVector;

public:
    typedef boost::shared_ptr<OrganizationModelAsk> Ptr;

    OrganizationModelAsk(OrganizationModel::Ptr om, const ModelPool& modelPool = ModelPool());

    /**
     * Retrieve the list of all known service models
     */
    owlapi::model::IRIList getServiceModels() const;

    void setModelPool(const ModelPool& modelPool) { mModelPool = modelPool; }

    const Combination2FunctionMap& getCombination2FunctionMap() const { return mCombination2Function; }

    const Function2CombinationMap& getFunction2CombinationMap() const { return mFunction2Combination; }

    /**
     * Get the set of resources (as combination of models) that should support a given
     * list of services
     * That means, that services are either supported by separate systems or 
     * combined systems
     * \param services should be a set of services / service models
     * \return available resources to support this set of services
     */
    std::set<ModelCombinationSet> getResourceSupport(const ServiceSet& services) const;

    /**
     * Check how a service is supported by a model if given cardinality
     * of this model is provided
     * \see getFunctionalSaturationPoint in order to find the minimum number
     * \return required to provide the functionality (if full support can be achieved)
     */
    algebra::SupportType getSupportType(const Service& service,
            const owlapi::model::IRI& model,
            uint32_t cardinalityOfModel = 1) const;

    /**
     *  Depending on the contribution of a model to the service the functional
     *  saturation point can be interpreted differently:
     *  1. when all required resources (FULL_SUPPORT) are provided by the model:
     *     -- check how many instances are required of this model to achieve
     *     full functionality
     *  2. when only a partial set of resource are provided (PARTIAL_SUPPORT)
     *     -- check how many instances are actually contributing to enable the
     *     functionality (and when switching into providing redundancy only)
     *  \return number of instances required for functional saturation
     */
    uint32_t getFunctionalSaturationPoint(const Service& service, const owlapi::model::IRI& model) const;

    /**
     * Get the set of resources
     * \return available resources to support this set of services
     */
    std::set<ModelCombination> getMinimalResourceSupport(const ServiceSet& services) const;

    std::set<ModelCombination> getMinimalResourceSupport_v1(const ServiceSet& services) const;
    std::set<ModelCombination> getMinimalResourceSupport_v2(const ServiceSet& services) const;

    /**
     * Retrieve the minimum cardinality of given model to support the set of
     * services
     */
    uint32_t minRequiredCardinality(const ServiceSet& services, const owlapi::model::IRI& model) const;

    /**
     * Check if two ModelCombinations can be built from distinct resources
     */
    bool canBeDistinct(const ModelCombination& a, const ModelCombination& b) const;

    bool isSupporting(const ModelCombination& c, const ServiceSet& services) const;

protected:
    /**
     * Prepare the organization model for a given set of available models
     */
    void prepare();

    /**
     * Compute the functionality maps for the combination of models from a
     * limited set of available models
     */
    void computeFunctionalityMaps(const ModelPool& modelPool);

    /**
     * Get the support vector for a given model
     * Does not (!) account for (sub)class relationship
     */
    algebra::ResourceSupportVector getSupportVector(const owlapi::model::IRI& model,
        const owlapi::model::IRIList& filterLabels = owlapi::model::IRIList(),
        bool useMaxCardinality = false) const;

    algebra::ResourceSupportVector getSupportVector(const std::map<owlapi::model::IRI,
        owlapi::model::OWLCardinalityRestriction::MinMax>& modelBounds,
        const owlapi::model::IRIList& filterLabels = owlapi::model::IRIList(),
        bool useMaxCardinality = false ) const;

    owlapi::model::IRIList filterSupportedModels(const owlapi::model::IRIList& combinations,
        const owlapi::model::IRIList& serviceModels);

    /**
     * Return ontology that relates to this Ask object
     */
    owlapi::model::OWLOntologyAsk ontology() const { return mOntologyAsk; }

private:
    OrganizationModel::Ptr mpOrganizationModel;
    owlapi::model::OWLOntologyAsk mOntologyAsk;

    /// Maps a combination to its supported functionality
    Combination2FunctionMap mCombination2Function;
    /// Maps a functionality to combination that support this functionality
    Function2CombinationMap mFunction2Combination;

    /// Current pool of models, i.e.
    /// how many instances of type X,Y
    /// to be considered
    ModelPool mModelPool;
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ASK_HPP
