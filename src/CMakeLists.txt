find_package(Boost REQUIRED regex)
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
    SOURCES Combinatorics.cpp
        KnowledgeBase.cpp
        OrganizationModel.cpp
        Ontology.cpp
        db/FileBackend.cpp
        db/rdf/SopranoDB.cpp
        db/rdf/Sparql.cpp
        db/rdf/SparqlInterface.cpp
        db/rdf/Variable.cpp
        owlapi/io/XMLUtils.cpp
        owlapi/model/IRI.cpp
        owlapi/model/URI.cpp
        exporter/PDDLExporter.cpp
        metrics/Redundancy.cpp
        reasoner/factpp/Types.cpp
    HEADERS Combinatorics.hpp
        Exceptions.hpp
        KnowledgeBase.hpp
        Ontology.hpp
        OrganizationModel.hpp
        OWLApi.hpp
        Vocabulary.hpp
        vocabularies/OM.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
        ccf/CCF.hpp
        db/FileBackend.hpp
        db/rdf/SopranoDB.hpp
        db/rdf/Sparql.hpp
        db/rdf/SparqlInterface.hpp
        db/rdf/Variable.hpp
        exporter/PDDLExporter.hpp
        metrics/Redundancy.hpp
        owlapi/io/XMLUtils.hpp
        #owlapi/model/AnonymousIndividual.hpp
        owlapi/model/HasIRI.hpp
        owlapi/model/HasAnnotations.hpp
        owlapi/model/IRI.hpp
        owlapi/model/OWLAnnotation.hpp
        owlapi/model/OWLAxiom.hpp
        owlapi/model/OWLClassExpression.hpp
        owlapi/model/OWLDataRange.hpp
        owlapi/model/OWLEntity.hpp
        owlapi/model/OWLNamedObject.hpp
        owlapi/model/OWLObject.hpp
        owlapi/model/OWLPropertyRange.hpp
        owlapi/model/URI.hpp
        owlapi/model/entities/OWLAnnotationProperty.hpp
        owlapi/model/entities/OWLClass.hpp
        owlapi/model/entities/OWLDataProperty.hpp
        owlapi/model/entities/OWLDatatype.hpp
        owlapi/model/entities/OWLLogicalEntity.hpp
        owlapi/model/entities/OWLNamedIndividual.hpp
        owlapi/model/entities/OWLObjectProperty.hpp
        reasoner/factpp/Types.hpp
    DEPS_PKGCONFIG base-lib soprano factpp pddl_planner liburiparser
)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(owl_om ${Boost_LIBRARIES} ${ICU_LIBS})

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

