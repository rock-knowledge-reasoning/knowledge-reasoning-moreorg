pkg_check_modules(GECODE gecode)
if(NOT GECODE_FOUND)
    set(GECODE_COMPONENTS
            flatzinc
            driver
            gist
            search
            minimodel
            set
            float
            int
            kernel
            support
    )

    foreach(GECODE_COMPONENT ${GECODE_COMPONENTS})
        find_library(GECODE_${GECODE_COMPONENT}_LIBRARY_PATH
            NAMES gecode${GECODE_COMPONENT}
            HINTS ${CMAKE_INSTALL_TARGET}/lib
                /opt/rock/$ENV{ROCK_DEB_RELEASE_NAME}/lib
            REQUIRED
        )
        get_filename_component(GECODE_${GECODE_COMPONENT}_LINK_DIRECTORY
            ${GECODE_${GECODE_COMPONENT}_LIBRARY_PATH} DIRECTORY)
        get_filename_component(GECODE_${GECODE_COMPONENT}_LIBRARY
            ${GECODE_${GECODE_COMPONENT}_LIBRARY_PATH} NAME)
        list(APPEND GECODE_LIBRARIES ${GECODE_${GECODE_COMPONENT}_LIBRARY})
        link_directories(${GECODE_${GECODE_COMPONENT}_LINK_DIRECTORY})
        message(STATUS "Found gecode library: ${GECODE_${GECODE_COMPONENT}_LIBRARY} \
             in ${GECODE_${GECODE_COMPONENT}_LINK_DIRECTORY}")
    endforeach()
else()
    message(STATUS "Found gecode libraries in ${GECODE_LIBRARY_DIRS}")
    link_directories(${GECODE_LIBRARIES_DIRS})
endif()

rock_testsuite(${PROJECT_NAME}-test suite.cpp
    #    test_CCF.cpp
    # test_CoalitionStructureGeneration.cpp
    # test_CorrelationClustering.cpp
    test_Algebra.cpp
    #test_Analyser.cpp
    test_Exporter.cpp
    test_CSP.cpp
    test_Heuristics.cpp
    test_Lego.cpp
    test_ModelPool.cpp
    test_Facades.cpp
    test_Metrics.cpp
    test_OrganizationModel.cpp
    test_OrganizationModelAsk.cpp
    test_Resource.cpp
    test_PropertyConstraintSolver.cpp
    #test_RandomModelGenerator.cpp
    DEPS moreorg
)
