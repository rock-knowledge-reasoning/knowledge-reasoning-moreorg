#ifndef OWL_OM_DB_ONTOLOGY_HPP
#define OWL_OM_DB_ONTOLOGY_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <owl_om/KnowledgeBase.hpp>
#include <owl_om/db/rdf/SparqlInterface.hpp>

//! Main enclosing namespace for OWL Organization Modelling
namespace owl_om {
//! Interfacing databases and such
namespace db {

/**
 * @class KnowledgeBase
 * @brief KnowledgeBase serves as main layer of abstraction for the underlying database
 */
class Ontology : public owl_om::KnowledgeBase
{
private:
    Ontology();

public:
    /**
     * Deconstructor implementation
     */
    ~Ontology();

    typedef boost::shared_ptr<Ontology> Ptr;

    /**
     * Create knowledge base from file
     */
    static Ontology::Ptr fromFile(const std::string& filename);

    /**
     * Update the ontology
     */
    void refresh();

    /**
     * Find all items that match the query
     */
    query::Results findAll(const Uri& subject, const Uri& predicate, const Uri& object) const;

    /**
     * Convert ontology to string representation
     */
    std::string toString() const;

private:
    query::SparqlInterface* mSparqlInterface;
};

} // end namespace db
} // end namespace owl_om

#endif // OWL_OM_DB_ONTOLOGY_HPP
