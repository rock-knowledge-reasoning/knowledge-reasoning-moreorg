#include <organization_model/algebra/Connectivity.hpp>
#include <iostream>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <graph_analysis/GraphIO.hpp>
#include <organization_model/metrics/Redundancy.hpp>

using namespace owlapi::model;
using namespace organization_model;
using namespace graph_analysis;


// how to create an n-d representation for exploration of the interface
// compatibility landscape
//

int main(int argc, char** argv)
{
    int c;
    std::string filename;
    size_t epochs = 1;
    size_t minFeasible = 1;
    while((c = getopt(argc,argv, "f:e:m:")) != -1)
    {
        if(optarg)
        {
            switch(c)
            {
                case 'f':
                    filename = optarg;
                    break;
                case 'e':
                {
                    epochs = boost::lexical_cast<size_t>(optarg);
                    break;
                }
                case 'm':
                {
                    minFeasible = boost::lexical_cast<size_t>(optarg);
                    break;
                }
            }
        }
    }
    if(filename.empty())
    {
        std::cout << "No file given" << std::endl;
        return -1;
    }

    ModelPool modelPool;
    modelPool[vocabulary::OM::resolve("Sherpa")] = 5;
    modelPool[vocabulary::OM::resolve("CREX")] = 3;
    modelPool[vocabulary::OM::resolve("BaseCamp")] = 3;
    modelPool[vocabulary::OM::resolve("Payload")] = 10;

    OrganizationModel::Ptr om(new OrganizationModel(filename) );
    OrganizationModelAsk ask(om, modelPool, true);


    std::vector<algebra::Connectivity::Statistics> stats;
    std::cout << "# epochs: " << epochs << std::endl;
    std::cout << modelPool.toString() << std::endl;

    for(size_t i = 0; i < epochs; ++i)
    {
        BaseGraph::Ptr baseGraph;
        bool feasible = algebra::Connectivity::isFeasible(modelPool, ask, baseGraph, 60000, minFeasible);
        assert(feasible);
        if(baseGraph)
        {
            std::stringstream ss;
            ss << "/tmp/organization-model-bm-connectivity-";
            ss << i;
            ss << ".dot";

            graph_analysis::io::GraphIO::write(ss.str(), baseGraph);
        }

        stats.push_back( algebra::Connectivity::getStatistics() );
        //std::cin.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
        usleep(100);
    }

    std::cout << algebra::Connectivity::Statistics::toString(stats) << std::endl;

    Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
    IRIList functions = ask.ontology().allSubClassesOf(vocabulary::OM::Functionality());
    std::cout << std::endl << "Functions" << std::endl;
    for(const IRI& f : functions)
    {
        double value;
        try {
            IRISet functionSet;
            functionSet.insert(f);
            value = metrics->computeSharedUse(functionSet, modelPool);
        } catch(...)
        {
            value = 0;
        }
        std::cout << f.toString() << " " << value << std::endl;
    }


    return 0;
}
