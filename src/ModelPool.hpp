#ifndef ORGANIZATION_MODEL_MODEL_POOL_HPP
#define ORGANIZATION_MODEL_MODEL_POOL_HPP

#include <map>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

typedef std::map<owlapi::model::IRI, size_t> ModelPool;

struct ModelPoolDelta : std::map<owlapi::model::IRI, int>
{
    ModelPoolDelta();
    ModelPoolDelta(const ModelPool& pool);

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
};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_MODEL_POOL_HPP
