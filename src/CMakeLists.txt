find_package(Boost REQUIRED thread serialization)
rock_library(organization_model
    SOURCES
        AtomicAgent.cpp
        Agent.cpp
        Algebra.cpp
        Analyser.cpp
        algebra/Connectivity.cpp
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
        facets/Robot.cpp
        Functionality.cpp
        FunctionalityMapping.cpp
        FunctionalityRequirement.cpp
        Heuristics.cpp
        metrics/Redundancy.cpp
        metrics/ModelSurvivability.cpp
        Metric.cpp
        ModelPool.cpp
        ModelPoolIterator.cpp
        OrganizationModel.cpp
        OrganizationModelAsk.cpp
        OrganizationModelTell.cpp
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
        Service.cpp
        StatusSample.cpp
        Types.cpp
        utils/CoalitionStructureGeneration.cpp
        ValueBound.cpp
    HEADERS
        AtomicAgent.hpp
        Agent.hpp
        Algebra.hpp
        Analyser.hpp
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
        facets/Facet.hpp
        facets/Robot.hpp
        Functionality.hpp
        FunctionalityRequirement.hpp
        FunctionalityMapping.hpp
        Heuristics.hpp
        metrics/Redundancy.hpp
        metrics/ModelSurvivability.hpp
        Metric.hpp
        ModelPool.hpp
        ModelPoolIterator.hpp
        OrganizationModel.hpp
        OrganizationModelAsk.hpp
        OrganizationModelTell.hpp
        PropertyConstraint.hpp
        PropertyConstraintSolver.hpp
        #RandomModelGenerator.hpp
        organization_model/ActorModelLink.hpp
        organization_model/EndpointModel.hpp
        organization_model/Grounding.hpp
        organization_model/InterfaceConnection.hpp
        organization_model/Statistics.hpp
        policies/EnergyProviderPolicy.hpp
        policies/TransportProviderPolicies.hpp
        Service.hpp
        SharedPtr.hpp
        StatusSample.hpp
        Types.hpp
        reasoning/ModelBound.hpp
        reasoning/ResourceMatch.hpp
        utils/CoalitionStructureGeneration.hpp
        utils/CorrelationClustering.hpp
        vocabularies/OM.hpp
        vocabularies/Robot.hpp
        vocabularies/VRP.hpp
        ValueBound.hpp
    DEPS_PKGCONFIG numeric pddl_planner owlapi
        graph_analysis
    LIBS glpk ${Boost_THREAD_LIBRARY}
)

find_library(gecode0 NAMES gecodeflatzinc   PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode1 NAMES gecodedriver     PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode2 NAMES gecodegist       PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode3 NAMES gecodesearch     PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode4 NAMES gecodeminimodel  PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode5 NAMES gecodeset        PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode6 NAMES gecodefloat      PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode7 NAMES gecodeint        PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode8 NAMES gecodekernel     PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )
find_library(gecode9 NAMES gecodesupport    PATHS ${CMAKE_INSTALL_TARGET}/lib REQUIRED )

set(GECODE_LIBRARIES
    ${gecode0} ${gecode1} ${gecode2} ${gecode3} ${gecode4} ${gecode5} ${gecode6} ${gecode7} ${gecode8} ${gecode9}
)
target_link_libraries(organization_model ${GECODE_LIBRARIES})


#rock_executable(organization_model_bin Main.cpp
#    DEPS organization_model)

rock_executable(multiagent_caf ccf/CAF.cpp
    DEPS organization_model)

rock_executable(multiagent_scenario_analysis utils/ScenarioAnalysis.cpp
    DEPS organization_model)

rock_executable(limited_combinations utils/LimitedCombination.cpp
    DEPS organization_model)

rock_executable(organization_model-bm Benchmark.cpp
    DEPS organization_model)
