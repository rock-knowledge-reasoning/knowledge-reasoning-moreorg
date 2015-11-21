rock_testsuite(${PROJECT_NAME}-test suite.cpp
    #    test_CCF.cpp
    # test_CoalitionStructureGeneration.cpp
    # test_CorrelationClustering.cpp
    # #test_Model.cpp
    test_Algebra.cpp
    test_Facets.cpp
    test_Metrics.cpp
    test_OrganizationModel.cpp
    test_OrganizationModelAsk.cpp
    DEPS organization_model
    LIBS gecodesearch
        gecodesupport
        gecodekernel
        gecodeset
        gecodeminimodel
        gecodeint
        gecodefloat
        gecodegist
    )
