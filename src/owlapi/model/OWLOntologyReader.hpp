#ifndef OWLAPI_MODEL_OWL_ONTOLOGY_READER_HPP
#define OWLAPI_MODEL_OWL_ONTOLOGY_READER_HPP

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/db/rdf/SparqlInterface.hpp>

namespace owlapi {
namespace model { 

class OWLOntologyTell;
class OWLOntologyAsk;

class OWLOntologyReader
{
    /// Pointer to the underlying query interfaces for SPARQL
    db::query::SparqlInterface* mSparqlInterface;

    OWLOntologyTell* mTell;
    OWLOntologyAsk* mAsk;

protected:

    db::query::Results findAll(const db::query::Variable& subject, const db::query::Variable& predicate, const db::query::Variable& object) const;

    void load();

    void loadObjectProperties();

public:
    /**
     * Default constructor for the OWLOntologyReader
     * use the SPARQL query interface by default
     */
    OWLOntologyReader();

    virtual ~OWLOntologyReader();

    virtual OWLOntology::Ptr fromFile(const std::string& filename);
};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_ONTOLOGY_READER_HPP
