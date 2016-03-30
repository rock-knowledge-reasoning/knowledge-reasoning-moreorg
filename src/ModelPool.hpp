#ifndef ORGANIZATION_MODEL_MODEL_POOL_HPP
#define ORGANIZATION_MODEL_MODEL_POOL_HPP

#include <map>
#include <set>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

typedef owlapi::model::IRIList ModelCombination;

/**
 * \class ModelPool
 * \brief A ModelPool describes a tuple set
 * {(<model-0>,n0),...,(<model-n>,nN)} to represent
 * a set of available resources
 */
class ModelPool : public std::map<owlapi::model::IRI, size_t>
{
    public:
        /**
         * Default constructor
         */
        ModelPool();

        /**
         * Construct model pool from and existing list of resources
         * (ModelCombination)
         */
        ModelPool(const ModelCombination& modelCombination);

        /**
         * Set the count of a given resource (model)
         * \param resource Model name for the resource
         * \param count Number of instances that are available of this model
         */
        void setResourceCount(const owlapi::model::IRI& resource, size_t count);

        /**
         * Stringify object
         * \param indent Indentation level
         * \return stringified ModelPool
         */
        std::string toString(uint32_t indent = 0) const;

        /**
         * Applies an upper bound to the current model pool
         * \return model bound after applying the upper bound
         */
        ModelPool applyUpperBound(const ModelPool& upperBounds) const;

        /**
         * Convert this model pool to a model combination
         * \return model combination
         */
        ModelCombination toModelCombination() const;

        /**
         * Stringify list a ModelPoolSet
         * \param modelPoolSet set to stringify
         * \param indent indentation level
         * \return stringified object
         */
        static std::string toString(const std::set<ModelPool>& modelPoolSet, uint32_t indent = 0);

        /**
         * Compute all combinations that can be generated from the given model
         * pool (which defines the available resources)
         *
         * \return Set of modelpool representing all possible combinations
         */
        std::set<ModelPool> allCombinations() const;
};

/**
 * \class ModelPoolDelta
 * \brief A ModelPoolDelta represents the difference between two ModelPools
 */
class ModelPoolDelta : public std::map<owlapi::model::IRI, int>
{
public:
    ModelPoolDelta(const ModelPool& pool = ModelPool());

    /**
     * Check if the pool assignment contains any negative values
     * \return True if any assignment contains a negative value, false otherwise
     */
    bool isNegative() const;

    /**
     * Convert to model pool
     * \throws if conversion is not possible, due to negative values
     * \return ModelPool
     */
    ModelPool toModelPool() const;

    /**
     * Stringify ModelPoolDelta
     * \param indent Indentation level
     * \return stringified object
     */
    std::string toString(uint32_t indent = 0) const;

    /**
     * Get models, i.e. keys of the model pool
     * \return List of IRIs that represent the resource models
     */
    static std::vector<owlapi::model::IRI> getModels(const ModelPool& modelPool);
};

/// The model pool set definition
typedef std::set<ModelPool> ModelPoolSet;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_MODEL_POOL_HPP
