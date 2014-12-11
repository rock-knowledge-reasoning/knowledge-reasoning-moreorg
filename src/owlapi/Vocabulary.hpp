#ifndef OWLAPI_VOCABULARY_HPP
#define OWLAPI_VOCABULARY_HPP

#include <string>
#include <owl_om/owlapi/model/IRI.hpp>

#define VOCABULARY_BASE_IRI(X) \
    static owlapi::model::IRI IRIPrefix() { static owlapi::model::IRI iri(X); return iri;}
#define VOCABULARY_ADD_WORD(NAME) \
    static owlapi::model::IRI NAME() { static owlapi::model::IRI name = IRIPrefix().resolve(#NAME); return name; }
#define VOCABULARY_DYNAMIC_EXTENSION \
    static owlapi::model::IRI resolve(const std::string& name) { return IRIPrefix().resolve(name); }

namespace owlapi {
namespace vocabulary {
/** 
 * \class Custom vocabulary
 */
class Custom
{
    owlapi::model::IRI mBase;
public:
    Custom(const std::string& prefix)
        : mBase(prefix,"")
    {
    }

    owlapi::model::IRI operator[](const std::string& name) const { return mBase.resolve(name); }
};

} // end namespace vocabulary
} // end namespace owlapi

#include <owl_om/owlapi/vocabularies/OWL.hpp>
#include <owl_om/owlapi/vocabularies/RDF.hpp>
#include <owl_om/owlapi/vocabularies/RDFS.hpp>
#include <owl_om/owlapi/vocabularies/OM.hpp>
#include <owl_om/owlapi/vocabularies/XSD.hpp>

#endif // OWLAPI_VOCABULARY_HPP
