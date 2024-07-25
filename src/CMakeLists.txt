find_package(gecode REQUIRED)
find_package(Boost REQUIRED thread regex system filesystem serialization program_options)
rock_library(moreorg
    SOURCES
        AtomicAgent.cpp
        Agent.cpp
        AgentLink.cpp
        AgentInterface.cpp
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
        InferenceRule.cpp
        inference_rules/AtomicAgentRule.cpp
        inference_rules/CompositeAgentRule.cpp
        io/LatexWriter.cpp
        metrics/Redundancy.cpp
        metrics/ModelSurvivability.cpp
        metrics/Probability.cpp
        metrics/ProbabilityOfFailure.cpp
        metrics/ProbabilityDensityFunction.cpp
        metrics/pdfs/ConstantPDF.cpp
        metrics/pdfs/ExponentialPDF.cpp
        metrics/pdfs/WeibullPDF.cpp
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
        policies/AllSelection.cpp
        policies/AgentSizeBasedSelection.cpp
        policies/DistributionPolicy.cpp
        policies/FunctionalityBasedSelection.cpp
        policies/PropertyBasedSelection.cpp
        policies/SelectionPolicy.cpp
        reasoning/ModelBound.cpp
        reasoning/ResourceMatch.cpp
        reasoning/ResourceInstanceMatch.cpp
        Resource.cpp
        ResourceInstance.cpp
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
        AgentLink.hpp
        AgentInterface.hpp
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
        InferenceRule.hpp
        inference_rules/AtomicAgentRule.hpp
        inference_rules/CompositeAgentRule.hpp
        io/LatexWriter.hpp
        metrics/Redundancy.hpp
        metrics/ModelSurvivability.hpp
        metrics/Probability.hpp
        metrics/ProbabilityOfFailure.hpp
        metrics/ProbabilityDensityFunction.hpp
        metrics/pdfs/ConstantPDF.hpp
        metrics/pdfs/ExponentialPDF.hpp
        metrics/pdfs/WeibullPDF.hpp
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
        policies/AllSelection.hpp
        policies/AgentSizeBasedSelection.hpp
        policies/DistributionPolicy.hpp
        policies/FunctionalityBasedSelection.hpp
        policies/PropertyBasedSelection.hpp
        policies/SelectionPolicy.hpp
        QueryCache.hpp
        Resource.hpp
        ResourceInstance.hpp
        Service.hpp
        SharedPtr.hpp
        Sample.hpp
        RequirementSample.hpp
        StatusSample.hpp
        Types.hpp
        reasoning/ModelBound.hpp
        reasoning/ResourceMatch.hpp
        reasoning/ResourceInstanceMatch.hpp
        utils/CoalitionStructureGeneration.hpp
        utils/OrganizationStructureGeneration.hpp
        utils/GecodeUtils.hpp
        vocabularies/OM.hpp
        vocabularies/OMBase.hpp
        vocabularies/Robot.hpp
        vocabularies/VRP.hpp
        ValueBound.hpp
    DEPS_PKGCONFIG
        owlapi
        numeric
        pddl_planner
        graph_analysis
        qxcfg
        muparser

    LIBS ${Boost_LIBRARIES}
        gecode::kernel
        gecode::set
        gecode::support
        gecode::search 
        gecode::int 
        gecode::float
        gecode::minimodel
)

rock_executable(moreorg-limited_combinations utils/LimitedCombination.cpp
    DEPS moreorg    
)

#rock_executable(moreorg_bin Main.cpp
#    DEPS moreorg)

rock_executable(moreorg-multiagent_caf ccf/CAF.cpp
    DEPS moreorg
)

rock_executable(moreorg-multiagent_scenario_analysis utils/ScenarioAnalysis.cpp
    DEPS moreorg
)

rock_executable(moreorg-bm Benchmark.cpp
    DEPS moreorg
)

rock_executable(moreorg-reader utils/OrganizationModelReader.cpp
    DEPS moreorg
)
