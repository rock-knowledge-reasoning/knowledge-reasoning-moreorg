find_package(Boost REQUIRED regex thread system)
set(ICU_LIBS icutu
    icutest
    icudata
    icule
    icuio
    icui18n
    iculx
    icuuc
)

rock_library(owlapi
    SOURCES owlapi/KnowledgeBase.cpp
        owlapi/csp/ResourceMatch.cpp
        owlapi/db/FileBackend.cpp
        owlapi/db/rdf/SopranoDB.cpp
        owlapi/db/rdf/Sparql.cpp
        owlapi/db/rdf/SparqlInterface.cpp
        owlapi/db/rdf/Variable.cpp
        owlapi/io/XMLUtils.cpp
        owlapi/model/IRI.cpp
        owlapi/model/OWLAxiom.cpp
        owlapi/model/OWLCardinalityRestriction.cpp
        owlapi/model/OWLClass.cpp
        owlapi/model/OWLClassAssertionAxiom.cpp
        owlapi/model/OWLClassExpression.cpp
        owlapi/model/OWLDataPropertyAssertionAxiom.cpp
        owlapi/model/OWLDeclarationAxiom.cpp
        owlapi/model/OWLDisjointClassesAxiom.cpp
        owlapi/model/OWLDisjointUnionAxiom.cpp
        owlapi/model/OWLEquivalentClassesAxiom.cpp
        owlapi/model/OWLLiteral.cpp
        owlapi/model/OWLLiteralInteger.cpp
        owlapi/model/OWLLiteralDouble.cpp
        owlapi/model/OWLObject.cpp
        owlapi/model/OWLObjectPropertyAssertionAxiom.cpp
        owlapi/model/OWLOntology.cpp
        owlapi/model/OWLOntologyAsk.cpp
        owlapi/model/OWLOntologyReader.cpp
        owlapi/model/OWLOntologyTell.cpp
        owlapi/model/OWLQualifiedRestriction.cpp
        owlapi/model/OWLRestriction.cpp
        owlapi/model/OWLSubClassOfAxiom.cpp
        owlapi/model/NodeID.cpp
        owlapi/model/URI.cpp
        owlapi/reasoner/factpp/Types.cpp
    HEADERS owlapi/Exceptions.hpp
        owlapi/KnowledgeBase.hpp
        owlapi/OWLApi.hpp
        owlapi/Vocabulary.hpp
        owlapi/csp/ResourceMatch.hpp
        owlapi/vocabularies/OM.hpp
        owlapi/vocabularies/OWL.hpp
        owlapi/vocabularies/RDF.hpp
        owlapi/vocabularies/RDFS.hpp
        owlapi/db/FileBackend.hpp
        owlapi/db/rdf/SopranoDB.hpp
        owlapi/db/rdf/Sparql.hpp
        owlapi/db/rdf/SparqlInterface.hpp
        owlapi/db/rdf/Variable.hpp
        owlapi/model/HasAnnotations.hpp
        owlapi/model/HasIRI.hpp
        owlapi/model/IRI.hpp
        owlapi/model/OWLAnnotation.hpp
        owlapi/model/OWLAnnotationObject.hpp
        owlapi/model/OWLAnnotationProperty.hpp
        owlapi/model/OWLAnnotationValue.hpp
        owlapi/model/OWLAnonymousIndividual.hpp
        owlapi/model/OWLAxiom.hpp
        owlapi/model/OWLCardinalityRestriction.hpp
        owlapi/model/OWLClassAssertionAxiom.hpp
        owlapi/model/OWLClassExpression.hpp
        owlapi/model/OWLClass.hpp
        owlapi/model/OWLDataRange.hpp
        owlapi/model/OWLEntity.hpp
        owlapi/model/OWLExactCardinalityRestriction.hpp
        owlapi/model/OWLIndividual.hpp
        owlapi/model/OWLIndiviualAxiom.hpp
        owlapi/model/OWLInverseObjectProperty.hpp
        owlapi/model/OWLLiteral.hpp
        owlapi/model/OWLLiteralInteger.hpp
        owlapi/model/OWLLiteralDouble.hpp
        owlapi/model/OWLLogicalAxiom.hpp
        owlapi/model/OWLLogicalEntity.hpp
        owlapi/model/OWLMaxCardinalityRestriction.hpp
        owlapi/model/OWLMinCardinalityRestriction.hpp
        owlapi/model/OWLNamedIndividual.hpp
        owlapi/model/OWLNamedObject.hpp
        owlapi/model/OWLObjectExactCardinality.hpp
        owlapi/model/OWLObject.hpp
        owlapi/model/OWLObjectPropertyExpression.hpp
        owlapi/model/OWLObjectProperty.hpp
        owlapi/model/OWLOntology.hpp
        owlapi/model/OWLOntologyAsk.hpp
        owlapi/model/OWLOntologyReader.hpp
        owlapi/model/OWLOntologyTell.hpp
        owlapi/model/OWLPropertyExpression.hpp
        owlapi/model/OWLPropertyRange.hpp
        owlapi/model/OWLQualifiedRestriction.hpp
        owlapi/model/OWLReasoner.hpp
        owlapi/model/OWLRestriction.hpp
        owlapi/model/URI.hpp
        owlapi/reasoner/factpp/Types.hpp
    DEPS_PKGCONFIG soprano factpp liburiparser base-lib
)

set(GECODE_LIBRARIES
    gecodesearch
    gecodesupport
    gecodeset
    gecodekernel
    gecodeminimodel
    gecodeint
    gecodefloat
)

include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(owlapi ${Boost_LIBRARIES} ${ICU_LIBS} ${GECODE_LIBRARIES})

rock_library(organization_model
    SOURCES
        OrganizationModel.cpp
        ccf/Actor.cpp
        ccf/CombinedActor.cpp
        ccf/Interface.cpp
        ccf/InterfaceType.cpp
        ccf/Link.cpp
        ccf/LinkGroup.cpp
        ccf/LinkType.cpp
        ccf/Scenario.cpp
        organization_model/ActorModelLink.cpp
        organization_model/EndpointModel.cpp
        organization_model/Grounding.cpp
        organization_model/InterfaceConnection.cpp
        organization_model/Statistics.cpp
        exporter/PDDLExporter.cpp
        metrics/Redundancy.cpp
        utils/CoalitionStructureGeneration.cpp
    HEADERS
        OrganizationModel.hpp
        ccf/Actor.hpp
        ccf/CombinedActor.hpp
        ccf/Interface.hpp
        ccf/InterfaceType.hpp
        ccf/Link.hpp
        ccf/LinkGroup.hpp
        ccf/LinkType.hpp
        ccf/Scenario.hpp
        exporter/PDDLExporter.hpp
        metrics/Redundancy.hpp
        organization_model/ActorModelLink.hpp
        organization_model/EndpointModel.hpp
        organization_model/Grounding.hpp
        organization_model/InterfaceConnection.hpp
        organization_model/Statistics.hpp
        utils/CoalitionStructureGeneration.hpp
        utils/CorrelationClustering.hpp
    DEPS owlapi
    DEPS_PKGCONFIG numeric pddl_planner 
)
target_link_libraries(organization_model glpk)

#rock_executable(owl_om_bin Main.cpp
#    DEPS organization_model)

rock_executable(multiagent_caf ccf/CAF.cpp
    DEPS organization_model)

rock_executable(multiagent_scenario_analysis utils/ScenarioAnalysis.cpp
    DEPS organization_model)

rock_executable(limited_combinations utils/LimitedCombination.cpp
    DEPS organization_model)
