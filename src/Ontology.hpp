#ifndef OWL_OM_DB_ONTOLOGY_HPP
#define OWL_OM_DB_ONTOLOGY_HPP

#include <string>
#include <boost/shared_ptr.hpp>
#include <owl_om/KnowledgeBase.hpp>
#include <owl_om/db/rdf/SparqlInterface.hpp>

//! Main enclosing namespace for OWL Organization Modelling
namespace owl_om {

class OrganizationModel;

/**
 * @class KnowledgeBase
 * @brief KnowledgeBase serves as main layer of abstraction for the underlying database
 *
 * @see http://www.w3.org/TR/owl-parsing/#sec-implementation
 */
class Ontology : public owl_om::KnowledgeBase
{
    friend class OrganizationModel;

public:
    /**
     * Contructs an empty ontology
     * Prefer using Ontology::fromFile
     */
    Ontology();

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
    void reload();

    /**
     * Find all items that match the query
     */
    db::query::Results findAll(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object) const;

    /**
     * Convert ontology to string representation
     */
    std::string toString() const;

private:

    /**
     * Initialize all default classes used in the ontology
     */
    void initializeDefaultClasses();

    /**
     * Load all object properties
     */
    void loadObjectProperties();

    db::query::SparqlInterface* mSparqlInterface;
};

} // end namespace owl_om

#endif // OWL_OM_DB_ONTOLOGY_HPP
