find_package(Boost REQUIRED regex)
rock_library(owl_om
    SOURCES KnowledgeBase.cpp
        OrganizationModel.cpp
        Ontology.cpp
        Vocabulary.cpp
        db/FileBackend.cpp
        db/rdf/SopranoDB.cpp
        db/rdf/Sparql.cpp
        db/rdf/SparqlInterface.cpp
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
        exporter/PDDLExporter.hpp
        owlapi/model/HasIRI.hpp
        owlapi/model/OWLAxiom.hpp
        owlapi/model/OWLEntity.hpp
        owlapi/model/OWLNamedObject.hpp
        owlapi/model/OWLObject.hpp
        reasoner/factpp/Types.hpp
    DEPS_PKGCONFIG base-lib redland soprano factpp pddl_planner
)
target_link_libraries(owl_om ${Boost_LIBRARIES})

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

