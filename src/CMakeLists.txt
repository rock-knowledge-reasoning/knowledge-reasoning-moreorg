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
    SOURCES KnowledgeBase.cpp
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
    HEADERS KnowledgeBase.hpp
        OrganizationModel.hpp
        OWLApi.hpp
        Ontology.hpp
        Vocabulary.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
        db/FileBackend.hpp
        db/rdf/SopranoDB.hpp
        db/rdf/Sparql.hpp
        db/rdf/SparqlInterface.hpp
        db/rdf/Variable.hpp
        exporter/PDDLExporter.hpp
        owlapi/io/XMLUtils.hpp
        owlapi/model/HasIRI.hpp
        owlapi/model/IRI.hpp
        owlapi/model/OWLAxiom.hpp
        owlapi/model/OWLEntity.hpp
        owlapi/model/OWLNamedObject.hpp
        owlapi/model/OWLObject.hpp
        owlapi/model/URI.hpp
        reasoner/factpp/Types.hpp
    DEPS_PKGCONFIG base-lib redland soprano factpp pddl_planner liburiparser
)
include_directories(${Boost_INCLUDE_DIR})
target_link_libraries(owl_om ${Boost_LIBRARIES} ${ICU_LIBS})

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

