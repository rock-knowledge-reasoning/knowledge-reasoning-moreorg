#ifndef ORGANIZATION_MODEL_MODEL_POOL_HPP
#define ORGANIZATION_MODEL_MODEL_POOL_HPP

#include <map>
#include <set>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

typedef owlapi::model::IRIList ModelCombination;

class ModelPool : public std::map<owlapi::model::IRI, size_t>
{
    public:
        ModelPool();

        ModelPool(const ModelCombination& modelCombination);

        void setResourceCount(const owlapi::model::IRI& resource, size_t count);

        /**
         * Stringify object
         */
        std::string toString() const;

        ModelPool applyUpperBound(const ModelPool& upperBounds) const;

        ModelCombination toModelCombination() const;

        static std::string toString(const std::set<ModelPool>& modelPoolSet, uint32_t indent = 0);

        std::set<ModelPool> allCombinations() const;
};

class ModelPoolDelta : public std::map<owlapi::model::IRI, int>
{
public:
    ModelPoolDelta(const ModelPool& pool = ModelPool());

    /**
     * Check if the pool assignment contains any negative values
     */
    bool isNegative() const;

    /**
     * Convert to model pool
     * \throws if conversion is not possible, due to negative values
     */
    ModelPool toModelPool() const;

    std::string toString() const;

    /**
     * Get models, i.e. keys of the model pool
     */
    static std::vector<owlapi::model::IRI> getModels(const ModelPool& modelPool);
};

typedef std::set<ModelPool> ModelPoolSet;

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_MODEL_POOL_HPP
