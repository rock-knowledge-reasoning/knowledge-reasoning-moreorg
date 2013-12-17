rock_library(owl_om
    SOURCES Class.cpp
        Registry.cpp
        Vocabulary.cpp
    HEADERS Class.hpp
        Property.hpp
        Registry.hpp
        Vocabulary.hpp
        vocabularies/OWL.hpp
        vocabularies/RDF.hpp
        vocabularies/RDFS.hpp
    DEPS_PKGCONFIG base-lib
)

rock_executable(owl_om_bin Main.cpp
    DEPS owl_om)

