#include <boost/test/unit_test.hpp>
#include "test_utils.hpp"

#include <organization_model/utils/CorrelationClustering.hpp>

using namespace numeric;

BOOST_AUTO_TEST_CASE(it_should_compute_cluster_correlation)
{
    std::vector<double> edgeWeights;
    edgeWeights.push_back(1.0);
    edgeWeights.push_back(-1.0);
    edgeWeights.push_back(-1.0);
    CorrelationClustering cc(edgeWeights);
    std::vector<double> solution = cc.getEdgeActivation();

    BOOST_TEST_MESSAGE("EdgeActivation: " << CorrelationClustering::toString(solution));
}
