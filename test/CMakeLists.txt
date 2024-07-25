find_package(gecode REQUIRED)
find_package(Boost REQUIRED thread regex system filesystem serialization program_options)

rock_testsuite(${PROJECT_NAME}-test suite.cpp
    #    test_CCF.cpp
    # test_CoalitionStructureGeneration.cpp
    # test_CorrelationClustering.cpp
    test_Algebra.cpp
    #test_Analyser.cpp
    test_Exporter.cpp
    test_CSP.cpp
    test_Heuristics.cpp
    test_InferenceRule.cpp
    test_Lego.cpp
    test_ModelPool.cpp
    test_Facades.cpp
    test_Metrics.cpp
    test_OrganizationModel.cpp
    test_OrganizationModelAsk.cpp
    test_Policy.cpp
    test_Resource.cpp
    test_PropertyConstraintSolver.cpp
    #test_RandomModelGenerator.cpp
    DEPS moreorg
)
