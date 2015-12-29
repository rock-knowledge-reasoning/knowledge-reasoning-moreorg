# CMakeLists.txt has to be located in the project folder and cmake has to be
# executed from 'project/build' with 'cmake ../'.
cmake_minimum_required(VERSION 2.6)
find_package(Rock)
rock_init(organization_model 0.2)
rock_activate_cxx11()
rock_standard_layout()


# INSTALL ONTOLOGIES
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2014/01/om-schema"
    OUTPUT_VARIABLE OM_SCHEMA_OWL)

INSTALL(FILES test/data/om-schema-v0.15.owl
    DESTINATION share/ontologies
    RENAME ${OM_SCHEMA_OWL}.owl)

# BaseCamp
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/robots/BaseCamp"
    OUTPUT_VARIABLE BASECAMP_OWL)

INSTALL(FILES test/data/om-robot-basecamp-v0.1.owl
    DESTINATION share/ontologies
    RENAME ${BASECAMP_OWL}.owl)

# COYOTE III
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/robots/CoyoteIII"
    OUTPUT_VARIABLE COYOTE_OWL)

INSTALL(FILES test/data/om-robot-coyote-iii-v0.1.owl
    DESTINATION share/ontologies
    RENAME ${COYOTE_OWL}.owl)

# Payload
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/robots/Payload"
    OUTPUT_VARIABLE PAYLOAD_OWL)

INSTALL(FILES test/data/om-robot-payload-v0.1.owl
    DESTINATION share/ontologies
    RENAME ${PAYLOAD_OWL}.owl)

# CREX
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/robots/CREX"
    OUTPUT_VARIABLE CREX_OWL)

INSTALL(FILES test/data/om-robot-crex-v0.1.owl
    DESTINATION share/ontologies
    RENAME ${CREX_OWL}.owl)

# Sherpa
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/robots/Sherpa"
    OUTPUT_VARIABLE SHERPA_OWL)

INSTALL(FILES test/data/om-robot-sherpa-v0.2.owl
    DESTINATION share/ontologies
    RENAME ${SHERPA_OWL}.owl)

# project TransTerrA
EXEC_PROGRAM(owlapi-utils
    ARGS "-c http://www.rock-robotics.org/2015/12/projects/TransTerrA"
    OUTPUT_VARIABLE TT_OWL)

INSTALL(FILES test/data/om-project-transterra.owl
    DESTINATION share/ontologies
    RENAME ${TT_OWL}.owl)


