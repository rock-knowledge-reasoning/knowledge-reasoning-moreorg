#ifndef OWLAPI_VOCABULARIES_RDF_HPP
#define OWLAPI_VOCABULARIES_RDF_HPP

namespace owlapi {
namespace vocabulary {

/**
 * The vocabulary for RDF
 * \see http://www.w3.org/TR/rdf-schema/#ch_summary
 */
class RDF
{
public:
    VOCABULARY_BASE_IRI("http://www.w3.org/1999/02/22-rdf-syntax-ns#");
    VOCABULARY_ADD_WORD(Alt);
    VOCABULARY_ADD_WORD(Bag);
    VOCABULARY_ADD_WORD(List);
    VOCABULARY_ADD_WORD(Property);
    VOCABULARY_ADD_WORD(Seq);
    VOCABULARY_ADD_WORD(Statement);
    VOCABULARY_ADD_WORD(XMLLiteral);
    VOCABULARY_ADD_WORD(first);
    VOCABULARY_ADD_WORD(object);
    VOCABULARY_ADD_WORD(nil);
    VOCABULARY_ADD_WORD(predicate);
    VOCABULARY_ADD_WORD(rest);
    VOCABULARY_ADD_WORD(subject);
    VOCABULARY_ADD_WORD(type);
    VOCABULARY_ADD_WORD(value);
    VOCABULARY_ADD_WORD(_1);
    VOCABULARY_ADD_WORD(_2);
    VOCABULARY_ADD_WORD(_3);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // OWLAPI_VOCABULARIES_RDF_HPP
