rock_library(owl_om
    SOURCES Class.cpp
        Registry.cpp
        Vocabulary.cpp
        db/KnowledgeBase.cpp
        db/FileBackend.cpp
        db/rdf/SopranoDB.cpp
        db/rdf/SparqlInterface.cpp
    HEADERS Class.hpp
        Property.hpp
        Registry.hpp
        Vocabulary.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
        db/KnowledgeBase.hpp
        db/FileBackend.hpp
        db/rdf/SopranoDB.hpp
        db/rdf/SparqlInterface.hpp
    DEPS_PKGCONFIG base-lib redland soprano
)
target_link_libraries(owl_om ctemplate ${Boost_LIBRARIES})

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

