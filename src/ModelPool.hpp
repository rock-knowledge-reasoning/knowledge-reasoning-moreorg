#ifndef ORGANIZATION_MODEL_MODEL_POOL_HPP
#define ORGANIZATION_MODEL_MODEL_POOL_HPP

#include <map>
#include <set>
#include <owlapi/model/IRI.hpp>
#include <boost/functional/hash.hpp>

namespace moreorg {

typedef owlapi::model::IRIList ModelCombination;
typedef std::set<ModelCombination> ModelCombinationSet;

/**
 * \class ModelPool
 * \brief A ModelPool describes a tuple set
 * {(<model-0>,n0),...,(<model-n>,nN)} to represent
 * a set of available resources
 */
class ModelPool : public std::map<owlapi::model::IRI, size_t>
{
    public:
        typedef std::vector<ModelPool> List;
        typedef std::set<ModelPool> Set;

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
         * Apply an upper bound of resources to an existing Set of model
         * combinations
         * \param modelPoolSet Set of modelPools
         * \param upperBounds Bounding ModelPool
         * \return all combinations that operate within the bounds of the given
         * ModelPool
         */
        static ModelPool::Set applyUpperBound(const ModelPool::Set& modelPools, const ModelPool& upperBound);

        /**
         * Apply an upper bound of resources to an existing Set of model
         * combinations
         * \param upperBounds Bounding ModelPool
         * \return all combinations that operate within the bounds of the given
         * ModelPool
         */
        static ModelCombinationSet applyUpperBound(const ModelCombinationSet& combinations, const ModelPool& upperBounds);

        /**
         * Apply a lower bound of resources to an existing set of model
         * combinations, i.e., remove ModelCombinations that operate below the lower
         * bound
         * \param combinations
         * \param lowerBound Bounding ModelPool
         * \return all combinations that operate within the bounds of the given
         * ModelPool
         */
        static ModelCombinationSet applyLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound);

        /**
         * Apply a lower bound of resources to an existing set of model
         * combinations, i.e., remove ModelCombinations that operate below the lower
         * bound
         * \param combinations
         * \param lowerBound Bounding ModelPool
         * \return all combinations that operate within the bounds of the given
         * ModelPool
         */
        static ModelPool::Set applyLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound);

        /*
         * Enforces the minimum requirement by expanding missing models to
         * a model combination to fulfill the requirement,
         * Apply a lower bound of resources to an existing set of model
         * combinations
         * \param combinations
         * \param lowerBound Bounding ModelPool
         * \return all combinations that operate now within the bounds of the given
         * ModelPool
         */
        static ModelCombinationSet expandToLowerBound(const ModelCombinationSet& combinations, const ModelPool& lowerBound);

        /*
         * Enforces the minimum requirement by expanding missing models to
         * a model combination to fulfill the requirement,
         * Apply a lower bound of resources to an existing set of model
         * combinations
         * \param combinations
         * \param lowerBound Bounding ModelPool
         * \return all combinations that operate now within the bounds of the given
         * ModelPool
         */
        static ModelPool::Set expandToLowerBound(const ModelPool::Set& modelPools, const ModelPool& lowerBound);

        /**
         * Check if the current model pool lies within the bound given by
         * the upper bound
         */
        bool isWithinUpperBound(const ModelPool& upperBound) const;

        /**
         * Convert this model pool to a model combination
         * \return model combination
         */
        ModelCombination toModelCombination() const;

        /**
         * Stringify a ModelPool::Set
         * \param modelPoolSet set to stringify
         * \param indent indentation level
         * \return stringified object
         */
        static std::string toString(const std::set<ModelPool>& modelPoolSet, uint32_t indent = 0);

        /**
         * Stringify a ModelPool::Vector
         * \param modelPoolSet set to stringify
         * \param indent indentation level
         * \return stringified object
         */
        static std::string toString(const std::vector<ModelPool>& modelPoolList, uint32_t indent = 0);

        /**
         * Compute all combinations that can be generated from the given model
         * pool (which defines the available resources)
         * \param maxSize If maxSize > 0 then this will be interpreted as maximum size of the combination in terms
         * of allowed model instances that are part of the ModelPool \see
         * numberOfInstances
         * \return Set of modelpool representing all possible combinations
         */
        std::set<ModelPool> allCombinations(size_t maxSize = 0) const;

        /**
         * Return the number of instances that are defined by this pool
         * \return number of instances
         */
        uint32_t numberOfInstances() const { return toModelCombination().size(); }

        /**
         * Get the cardinality of a resource
         * \param defaultVal Return this default value if the model entry does
         * not exist
         */
        size_t getValue(const owlapi::model::IRI& resource, size_t defaultVal) const;

        /**
         * Get the entry with the maximum number of resources
         * \return Entry with maximum number of resources -- if multiple entries
         * exist the first found will be returned
         */
        std::pair<owlapi::model::IRI, size_t> getMaxResource() const;

        /**
         * Get the entry with the minimum number of resources
         * \return Entry with minimum number of resources -- if multiple entries
         * exist the first found will be returned
         */
        std::pair<owlapi::model::IRI, size_t> getMinResource() const;

        /**
         * Get the maximum value of all key value pairs
         * \return maximum value of resource count
         */
        size_t getMaxResourceCount() const;

        /**
         * Get the minimum value of all key value pairs
         * \return minimum value of resource count
         */
        size_t getMinResourceCount() const;

        /**
         * Get the list of models
         * \return list of models
         */
        owlapi::model::IRIList getModels() const;

        /**
         * Check if this ModelPool describes an atomic model, i.e.
         * 1 key value pair with cardinality 1
         */
        bool isAtomic() const { return size() == 1 && begin()->second == 1; }

        /**
         * Get model if this describes an atomic model
         */
        const owlapi::model::IRI& getAtomic() const;

        /**
         * Get a model pool with all cardinalities unequal to zero
         * \return model pool without zero cardinalities
         */
        ModelPool compact() const;

        /**
         * Check if pool assignment is null, i.e. contains only 0 cardinalities or
         * is overall empty
         * \return True if pool assignment is null
         */
        bool isNull() const;
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
     * Check if pool assignment is null, i.e. contains only 0 cardinalities or
     * is overall empty
     * \return True if pool assignment is null
     */
    bool isNull() const;

    /**
     * Check if the pool assignment is positive, i.e. is not negative and not
     * null
     * \return True if pool assignment is positive
     */
    bool isPositive() const { return !(isNegative() || isNull()); }

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

} // end namespace moreorg

// Enable usage of ModelPool as key in unordered maps
namespace std {
template<>
struct hash<moreorg::ModelPool> {
    size_t operator()(const moreorg::ModelPool& modelPool) const
    {
        size_t seed = 0;
        for(const moreorg::ModelPool::value_type& v : modelPool)
        {
            size_t hashValue = std::hash<owlapi::model::IRI>()(v.first);
            boost::hash_combine(seed, hashValue);
            boost::hash_combine(seed, v.second);
        }
        return seed;
    }
};
} // end namespace std

#endif // ORGANIZATION_MODEL_MODEL_POOL_HPP
