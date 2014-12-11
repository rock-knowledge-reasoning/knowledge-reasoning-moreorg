#ifndef OWLAPI_VOCABULARIES_RDFS_HPP
#define OWLAPI_VOCABULARIES_RDFS_HPP

namespace owlapi {
namespace vocabulary {

/**
 * The vocabulary for RDFS
 */
class RDFS
{
public:
    VOCABULARY_BASE_IRI("http://www.w3.org/2000/01/rdf-schema#");
    VOCABULARY_ADD_WORD(Class);
    VOCABULARY_ADD_WORD(Container);
    VOCABULARY_ADD_WORD(ContainerMembershipProperty);
    VOCABULARY_ADD_WORD(Datatype);
    VOCABULARY_ADD_WORD(Literal);
    VOCABULARY_ADD_WORD(Resource);
    VOCABULARY_ADD_WORD(comment);
    VOCABULARY_ADD_WORD(domain);
    VOCABULARY_ADD_WORD(isDefinedBy);
    VOCABULARY_ADD_WORD(label);
    VOCABULARY_ADD_WORD(member);
    VOCABULARY_ADD_WORD(range);
    VOCABULARY_ADD_WORD(seeAlso);
    VOCABULARY_ADD_WORD(subClassOf);
    VOCABULARY_ADD_WORD(subPropertyOf);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // OWLAPI_VOCABULARIES_RDFS_HPP
