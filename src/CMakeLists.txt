find_package(Boost REQUIRED regex)
rock_library(owl_om
    SOURCES KnowledgeBase.cpp
        OrganizationModel.cpp
        Vocabulary.cpp
        db/Ontology.cpp
        db/FileBackend.cpp
        db/rdf/SopranoDB.cpp
        db/rdf/SparqlInterface.cpp
        exporter/PDDLExporter.cpp
    HEADERS KnowledgeBase.hpp
        OrganizationModel.hpp
        Property.hpp
        Vocabulary.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
        db/Ontology.hpp
        db/FileBackend.hpp
        db/rdf/SopranoDB.hpp
        db/rdf/SparqlInterface.hpp
        exporter/PDDLExporter.hpp
    DEPS_PKGCONFIG base-lib redland soprano factpp pddl_planner
)
target_link_libraries(owl_om ${Boost_LIBRARIES})

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

