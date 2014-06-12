#ifndef OWL_OM_DB_KNOWLEDGE_BASE_HPP
#define OWL_OM_DB_KNOWLEDGE_BASE_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <owl_om/Registry.hpp>
#include <owl_om/Instance.hpp>
#include <owl_om/db/rdf/SparqlInterface.hpp>

namespace owl_om {
namespace db {

class KnowledgeBase
{
private:
    KnowledgeBase();

    void updateRegistry(Registry* registry) const;

    void updateInstances(Instance::List* instances);

public:
    /**
     * Deconstructor implementation
     */
    ~KnowledgeBase();

    typedef boost::shared_ptr<KnowledgeBase> Ptr;

    /**
     * Create knowledge base from file
     */
    static KnowledgeBase::Ptr fromFile(const std::string& filename);

    /**
     * Find all item that match
     */
    query::Results findAll(const Uri& subject, const Uri& predicate, const Uri& object) const;

    /**
     * Get registry
     */
    const Registry& getRegistry() const { return mRegistry; }

    /**
     * Get instances, i.e. named individuals
     * \return instances
     */
    const Instance::List& getInstances() const { return mInstances; }


    std::string toString() const;

private:
    query::SparqlInterface* mSparqlInterface;
    Registry mRegistry;
    Instance::List mInstances;
};

} // end namespace db
} // end namespace owl_om

#endif // OWL_OM_DB_KNOWLEDGE_BASE_HPP
