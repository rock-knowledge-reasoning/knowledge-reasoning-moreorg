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

rock_library(owl_om
    SOURCES KnowledgeBase.cpp
        OrganizationModel.cpp
        Ontology.cpp
        ccf/Actor.cpp
        ccf/CombinedActor.cpp
        ccf/Interface.cpp
        ccf/InterfaceType.cpp
        ccf/Link.cpp
        ccf/LinkGroup.cpp
        ccf/LinkType.cpp
        ccf/Scenario.cpp
        db/FileBackend.cpp
        db/rdf/SopranoDB.cpp
        db/rdf/Sparql.cpp
        db/rdf/SparqlInterface.cpp
        db/rdf/Variable.cpp
        organization_model/ActorModelLink.cpp
        organization_model/EndpointModel.cpp
        organization_model/Grounding.cpp
        organization_model/InterfaceConnection.cpp
        organization_model/Statistics.cpp
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
        owlapi/model/OWLObject.cpp
        owlapi/model/OWLObjectPropertyAssertionAxiom.cpp
        owlapi/model/OWLQualifiedRestriction.cpp
        owlapi/model/OWLSubClassOfAxiom.cpp
        owlapi/model/NodeID.cpp
        owlapi/model/URI.cpp
        exporter/PDDLExporter.cpp
        metrics/Redundancy.cpp
        reasoner/factpp/Types.cpp
        utils/CoalitionStructureGeneration.cpp
    HEADERS Exceptions.hpp
        KnowledgeBase.hpp
        Ontology.hpp
        OrganizationModel.hpp
        OWLApi.hpp
        Vocabulary.hpp
        vocabularies/OM.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
        ccf/Actor.hpp
        ccf/CombinedActor.hpp
        ccf/Interface.hpp
        ccf/InterfaceType.hpp
        ccf/Link.hpp
        ccf/LinkGroup.hpp
        ccf/LinkType.hpp
        ccf/Scenario.hpp
        db/FileBackend.hpp
        db/rdf/SopranoDB.hpp
        db/rdf/Sparql.hpp
        db/rdf/SparqlInterface.hpp
        db/rdf/Variable.hpp
        exporter/PDDLExporter.hpp
        metrics/Redundancy.hpp
        organization_model/ActorModelLink.hpp
        organization_model/EndpointModel.hpp
        organization_model/Grounding.hpp
        organization_model/InterfaceConnection.hpp
        organization_model/Statistics.hpp
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
        owlapi/model/OWLPropertyExpression.hpp
        owlapi/model/OWLPropertyRange.hpp
        owlapi/model/OWLQualifiedRestriction.hpp
        owlapi/model/OWLReasoner.hpp
        owlapi/model/OWLRestriction.hpp
        owlapi/model/URI.hpp
        reasoner/factpp/Types.hpp
        utils/CoalitionStructureGeneration.hpp
        utils/CorrelationClustering.hpp
    DEPS_PKGCONFIG numeric soprano factpp pddl_planner liburiparser
)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(owl_om ${Boost_LIBRARIES} ${ICU_LIBS} glpk)

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

rock_executable(multiagent_caf ccf/CAF.cpp
    DEPS owl_om)

rock_executable(multiagent_scenario_analysis utils/ScenarioAnalysis.cpp
    DEPS owl_om)

rock_executable(limited_combinations utils/LimitedCombination.cpp
    DEPS owl_om)
