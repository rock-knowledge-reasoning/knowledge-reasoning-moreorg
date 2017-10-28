#include <organization_model/algebra/Connectivity.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <graph_analysis/GraphIO.hpp>
#include <organization_model/metrics/Redundancy.hpp>

using namespace owlapi::model;
using namespace organization_model;
using namespace graph_analysis;

void loadSpec(const std::string& filename, ModelPool& modelPool, IRI& organizationModelIRI)
{
    std::ifstream spec(filename);
    if(spec.is_open())
    {
        std::string line;
        std::string delimiter = " ";
        while(getline(spec, line))
        {
            // if line start with # then this is a comment
            if(line.find("#") == 0 || line.empty())
            {
                continue;
            }
            // First line defines the organization model ontology IRI
            if(organizationModelIRI == IRI())
            {
                organizationModelIRI = IRI(line);
                std::cout << "Read orga iri: " << organizationModelIRI << std::endl;
            } else {
                size_t spacePos = line.find(delimiter);
                std::string iri = line.substr(0, spacePos);
                std::string count = line.substr(spacePos+1);
                std::cout << "Read: iri:" << iri << " '" << count << "'" << std::endl;
                modelPool[owlapi::model::IRI(iri)] = boost::lexical_cast<uint32_t>(count);
            }
        }
        spec.close();
    } else {
        throw std::runtime_error("organization_model::Benchmark: failed to load spec from: " + filename );
    }
}


// how to create an n-d representation for exploration of the interface
// compatibility landscape
//

int main(int argc, char** argv)
{
    int c;
    std::string filename;
    std::string specfile;
    size_t epochs = 1;
    size_t minFeasible = 1;
    while((c = getopt(argc,argv, "o:e:m:s:")) != -1)
    {
        if(optarg)
        {
            switch(c)
            {
                // filename for organization model
                case 'o':
                    filename = optarg;
                    break;
                // epoch
                case 'e':
                {
                    epochs = boost::lexical_cast<size_t>(optarg);
                    break;
                }
                // minfeasible
                case 'm':
                {
                    minFeasible = boost::lexical_cast<size_t>(optarg);
                    break;
                }
                // spec
                case 's':
                {
                   specfile = optarg;
                }
            }
        }
    }
    if(filename.empty() && specfile.empty())
    {
        std::cout << "No file or specfile given" << std::endl;
        return -1;
    }

    ModelPool modelPool;
    IRI organizationModelIRI;
    if(!specfile.empty())
    {
        loadSpec(specfile, modelPool, organizationModelIRI);
    } else
    {
        modelPool[vocabulary::OM::resolve("Sherpa")] = 5;
        modelPool[vocabulary::OM::resolve("CREX")] = 3;
        modelPool[vocabulary::OM::resolve("BaseCamp")] = 3;
        modelPool[vocabulary::OM::resolve("Payload")] = 10;
    }

    OrganizationModel::Ptr om;
    if(!filename.empty())
    {
            om = OrganizationModel::Ptr(new OrganizationModel(filename) );
    } else {
        om = OrganizationModel::Ptr(new OrganizationModel(organizationModelIRI));
    }

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
