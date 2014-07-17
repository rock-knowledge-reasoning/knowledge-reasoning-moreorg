#ifndef OWL_OM_VOCABULARY_HPP
#define OWL_OM_VOCABULARY_HPP

#include <string>
#include <owl_om/OWLApi.hpp>

#define VOCABULARY_BASE_IRI(X) \
    static owlapi::model::IRI IRIPrefix() { return owlapi::model::IRI::create(X); }
#define VOCABULARY_ADD_WORD(NAME) \
    static owlapi::model::IRI NAME() { return IRIPrefix().resolve(#NAME); }

namespace owl_om {
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
} // end namespace owl_om

#include <owl_om/vocabularies/OWL.hpp>
#include <owl_om/vocabularies/RDF.hpp>
#include <owl_om/vocabularies/RDFS.hpp>
#include <owl_om/vocabularies/OM.hpp>

#endif // OWL_OM_VOCABULARY_HPP
