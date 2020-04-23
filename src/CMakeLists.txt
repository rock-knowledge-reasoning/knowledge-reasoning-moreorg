pkg_check_modules(GECODE REQUIRED gecode)
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

find_package(Boost REQUIRED thread serialization program_options)
rock_library(organization_model
    SOURCES
        AtomicAgent.cpp
        Agent.cpp
        Algebra.cpp
        Analyser.cpp
        algebra/Connectivity.cpp
        algebra/CompositionFunction.cpp
        algebra/ResourceSupportVector.cpp
        ccf/Actor.cpp
        ccf/CombinedActor.cpp
        ccf/Interface.cpp
        ccf/InterfaceType.cpp
        ccf/Link.cpp
        ccf/LinkGroup.cpp
        ccf/LinkType.cpp
        ccf/Scenario.cpp
        exporter/PDDLExporter.cpp
        facades/Facade.cpp
        facades/Robot.cpp
        FunctionalityMapping.cpp
        Heuristics.cpp
        io/LatexWriter.cpp
        metrics/Redundancy.cpp
        metrics/ModelSurvivability.cpp
        Metric.cpp
        ModelPool.cpp
        ModelPoolIterator.cpp
        OrganizationModel.cpp
        OrganizationModelAsk.cpp
        OrganizationModelTell.cpp
        QueryCache.cpp
        Policy.cpp
        PropertyConstraint.cpp
        PropertyConstraintSolver.cpp
        #RandomModelGenerator.cpp
        organization_model/ActorModelLink.cpp
        organization_model/EndpointModel.cpp
        organization_model/Grounding.cpp
        organization_model/InterfaceConnection.cpp
        organization_model/Statistics.cpp
        policies/EnergyProviderPolicy.cpp
        policies/TransportProviderPolicy.cpp
        reasoning/ModelBound.cpp
        reasoning/ResourceMatch.cpp
        Resource.cpp
        Service.cpp
        Sample.cpp
        RequirementSample.cpp
        StatusSample.cpp
        Types.cpp
        utils/CoalitionStructureGeneration.cpp
        utils/OrganizationStructureGeneration.cpp
        utils/GecodeUtils.cpp
        ValueBound.cpp
    HEADERS
        AtomicAgent.hpp
        Agent.hpp
        Algebra.hpp
        Analyser.hpp
        algebra/CompositionFunction.hpp
        algebra/Connectivity.hpp
        algebra/ResourceSupportVector.hpp
        ccf/Actor.hpp
        ccf/CombinedActor.hpp
        ccf/Interface.hpp
        ccf/InterfaceType.hpp
        ccf/Link.hpp
        ccf/LinkGroup.hpp
        ccf/LinkType.hpp
        ccf/Scenario.hpp
        exporter/PDDLExporter.hpp
        facades/Facade.hpp
        facades/Robot.hpp
        FunctionalityMapping.hpp
        Heuristics.hpp
        io/LatexWriter.hpp
        metrics/Redundancy.hpp
        metrics/ModelSurvivability.hpp
        Metric.hpp
        ModelPool.hpp
        ModelPoolIterator.hpp
        OrganizationModel.hpp
        OrganizationModelAsk.hpp
        OrganizationModelTell.hpp
        Policy.hpp
        PropertyConstraint.hpp
        PropertyConstraintSolver.hpp
        #RandomModelGenerator.hpp
        organization_model/ActorModelLink.hpp
        organization_model/EndpointModel.hpp
        organization_model/Grounding.hpp
        organization_model/InterfaceConnection.hpp
        organization_model/Statistics.hpp
        policies/EnergyProviderPolicy.hpp
        policies/TransportProviderPolicy.hpp
        QueryCache.hpp
        Resource.hpp
        Service.hpp
        SharedPtr.hpp
        Sample.hpp
        RequirementSample.hpp
        StatusSample.hpp
        Types.hpp
        reasoning/ModelBound.hpp
        reasoning/ResourceMatch.hpp
        utils/CoalitionStructureGeneration.hpp
        utils/OrganizationStructureGeneration.hpp
        utils/GecodeUtils.hpp
        vocabularies/OM.hpp
        vocabularies/Robot.hpp
        vocabularies/VRP.hpp
        ValueBound.hpp
    DEPS_PKGCONFIG
        owlapi
        numeric
        pddl_planner
        graph_analysis
        qxcfg
    LIBS ${Boost_THREAD_LIBRARY}
        ${GECODE_LIBRARIES}
)

target_link_libraries(organization_model ${GECODE_LIBRARIES})

rock_executable(limited_combinations utils/LimitedCombination.cpp
    DEPS organization_model)

#rock_executable(organization_model_bin Main.cpp
#    DEPS organization_model)

rock_executable(multiagent_caf ccf/CAF.cpp
    DEPS organization_model)

rock_executable(multiagent_scenario_analysis utils/ScenarioAnalysis.cpp
    DEPS organization_model
    LIBS ${Boost_LIBRARIES})

rock_executable(organization_model-bm Benchmark.cpp
    DEPS organization_model)

rock_executable(organization_model-reader utils/OrganizationModelReader.cpp
    DEPS organization_model
    LIBS ${Boost_LIBRARIES})
