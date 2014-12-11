#ifndef OWLAPI_VOCABULARIES_OWL_HPP
#define OWLAPI_VOCABULARIES_OWL_HPP

#ifndef VOCABULARY_BASE_IRI
#error "Direct inclusion of this header is forbidden. Use Vocabulary.hpp instead"
#endif

namespace owlapi {
namespace vocabulary {

/**
 * The vocabulary for OWL
 * \see http://www.w3.org/TR/owl2-mapping-to-rdf/
 */
class OWL
{
public:
    VOCABULARY_BASE_IRI("http://www.w3.org/2002/07/owl#");
    VOCABULARY_ADD_WORD(AllDifferent);
    VOCABULARY_ADD_WORD(AnnotationProperty);
    VOCABULARY_ADD_WORD(Class);
    VOCABULARY_ADD_WORD(DataRange);
    VOCABULARY_ADD_WORD(DatatypeProperty);
    VOCABULARY_ADD_WORD(DeprecatedClass);
    VOCABULARY_ADD_WORD(DeprecatedProperty);
    VOCABULARY_ADD_WORD(FunctionalProperty);
    VOCABULARY_ADD_WORD(InverseFunctionalProperty);
    VOCABULARY_ADD_WORD(Nothing);
    VOCABULARY_ADD_WORD(ObjectProperty);
    VOCABULARY_ADD_WORD(Ontology);
    VOCABULARY_ADD_WORD(OntologyProperty);
    VOCABULARY_ADD_WORD(Restriction);
    VOCABULARY_ADD_WORD(SymmetricProperty);
    VOCABULARY_ADD_WORD(Thing);
    VOCABULARY_ADD_WORD(TransitiveProperty);
    VOCABULARY_ADD_WORD(allValuesFrom);
    VOCABULARY_ADD_WORD(backwardCompatibleWith);
    /// Unqualified cardinality
    VOCABULARY_ADD_WORD(cardinality);
    VOCABULARY_ADD_WORD(complementOf);
    VOCABULARY_ADD_WORD(differentFrom);
    VOCABULARY_ADD_WORD(disjointWith);
    VOCABULARY_ADD_WORD(distinctMembers);
    VOCABULARY_ADD_WORD(equivalentClass);
    VOCABULARY_ADD_WORD(equivalentProperty);
    VOCABULARY_ADD_WORD(hasValue);
    VOCABULARY_ADD_WORD(imports);
    VOCABULARY_ADD_WORD(incompatibleWith);
    VOCABULARY_ADD_WORD(intersectionOf);
    VOCABULARY_ADD_WORD(inverseOf);
    VOCABULARY_ADD_WORD(maxCardinality);
    VOCABULARY_ADD_WORD(minCardinality);
    VOCABULARY_ADD_WORD(onClass);
    VOCABULARY_ADD_WORD(onProperty);
    VOCABULARY_ADD_WORD(oneOf);
    VOCABULARY_ADD_WORD(priorVersion);
    VOCABULARY_ADD_WORD(sameAs);
    VOCABULARY_ADD_WORD(someValuesFrom);
    VOCABULARY_ADD_WORD(unionOf);
    VOCABULARY_ADD_WORD(versionInfo);


    // OWL 2
    VOCABULARY_ADD_WORD(NamedIndividual);
    VOCABULARY_ADD_WORD(qualifiedCardinality);
    VOCABULARY_ADD_WORD(maxQualifiedCardinality);
    VOCABULARY_ADD_WORD(minQualifiedCardinality);
    VOCABULARY_ADD_WORD(real);
    VOCABULARY_ADD_WORD(rational);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // OWLAPI_VOCABULARIES_OWL_HPP
