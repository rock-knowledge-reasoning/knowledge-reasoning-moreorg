#include <organization_model/algebra/Connectivity.hpp>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <graph_analysis/GraphIO.hpp>
#include "metrics/Redundancy.hpp"
#include "ModelPoolIterator.hpp"

using namespace owlapi::model;
using namespace organization_model;
using namespace graph_analysis;

struct Spec
{
    owlapi::model::IRI organizationModelIRI;
    ModelPool from;
    ModelPool to;
    ModelPool stepSize;

    std::string toString() const
    {
        std::stringstream ss;
        ss << "model: " << organizationModelIRI << std::endl;
        ss << "from: "  << from.toString(4) << std::endl;
        ss << "to: " << to.toString(4) << std::endl;
        ss << "stepsize: " << stepSize.toString(4) << std::endl;
        return ss.str();
    }
};

Spec loadSpec(const std::string& filename)
{
    Spec spec;
    std::ifstream specFile(filename);
    if(specFile.is_open())
    {
        std::string line;
        std::string delimiter = " ";
        while(getline(specFile, line))
        {
            // if line start with # then this is a comment
            if(line.find("#") == 0 || line.empty())
            {
                continue;
            }
            // First line defines the organization model ontology IRI
            if(spec.organizationModelIRI == IRI())
            {
                spec.organizationModelIRI = IRI(line);
            } else {
                boost::char_separator<char> sep(" ");
                boost::tokenizer< boost::char_separator<char> > tokens(line, sep);

                std::vector<std::string> columns(tokens.begin(), tokens.end());
                if(columns.size() < 2)
                {
                    std::invalid_argument("organization_model::Benchmark: invalid column: '" + line + "' in spec");
                }

                // Column: iri <from> <to> <stepsize>
                std::string iri = columns.at(0);
                std::string numberColumn = columns.at(1);
                spec.from[owlapi::model::IRI(iri)] = boost::lexical_cast<uint32_t>(columns.at(1));
                if(columns.size() >= 3)
                {
                    spec.to[owlapi::model::IRI(iri)] = boost::lexical_cast<uint32_t>(columns.at(2));
                } else {
                    spec.to = spec.from;
                }
                if(columns.size() >= 4)
                {
                    spec.stepSize[owlapi::model::IRI(iri)] = boost::lexical_cast<uint32_t>(columns.at(3));
                }
            }
        }
        specFile.close();
    } else {
        throw std::runtime_error("organization_model::Benchmark: failed to load spec from: " + filename );
    }
    return spec;
}

std::vector< algebra::Connectivity::Statistics> runModelPoolTest(const OrganizationModel::Ptr& om, const ModelPool& modelPool,
        size_t epochs,
        size_t minFeasible)
{
    OrganizationModelAsk ask(om, modelPool, true);
    std::vector<algebra::Connectivity::Statistics> stats;
    std::cout << "# epochs: " << epochs << std::endl;
    std::cout << modelPool.toString() << std::endl;

    for(size_t i = 0; i < epochs; ++i)
    {
        BaseGraph::Ptr baseGraph;
        bool feasible = algebra::Connectivity::isFeasible(modelPool, ask, baseGraph, 60000, minFeasible);
        if(baseGraph)
        {
            std::stringstream ss;
            ss << "/tmp/organization-model-bm-connectivity-";
            ss << i;
            ss << ".dot";

            graph_analysis::io::GraphIO::write(ss.str(), baseGraph);
        }

        stats.push_back( algebra::Connectivity::getStatistics() );
    }

    //std::cout << algebra::Connectivity::Statistics::toString(stats) << std::endl;

    //Metric::Ptr metrics = Metric::getInstance(metrics::REDUNDANCY, ask);
    //IRIList functions = ask.ontology().allSubClassesOf(vocabulary::OM::Functionality());
    //std::cout << std::endl << "Functions" << std::endl;
    //for(const IRI& f : functions)
    //{
    //    double value;
    //    try {
    //        IRISet functionSet;
    //        functionSet.insert(f);
    //        value = metrics->computeSharedUse(functionSet, modelPool);
    //    } catch(...)
    //    {
    //        value = 0;
    //    }
    //    std::cout << f.toString() << " " << value << std::endl;
    //}

    return stats;
}


// how to create an n-d representation for exploration of the interface
// compatibility landscape
//

int main(int argc, char** argv)
{
    int c;
    std::string filename;
    std::string specfile;
    std::string logfile = "/tmp/organization-model-benchmark.log";
    size_t epochs = 1;
    size_t minFeasible = 1;
    while((c = getopt(argc,argv, "o:e:m:s:l:")) != -1)
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
                   break;
                }
                // l
                case 'l':
                {
                    logfile = optarg;
                    break;
                }
            }
        }
    }
    if(filename.empty() && specfile.empty())
    {
        std::cout << "No file or specfile given" << std::endl;
        return -1;
    }

    Spec spec;
    if(!specfile.empty())
    {
        spec = loadSpec(specfile);
        std::cout << "Spec loaded: " << spec.toString() << std::endl;
    } else
    {
        spec.from[vocabulary::OM::resolve("Sherpa")] = 5;
        spec.from[vocabulary::OM::resolve("CREX")] = 3;
        spec.from[vocabulary::OM::resolve("BaseCamp")] = 3;
        spec.from[vocabulary::OM::resolve("Payload")] = 10;

        spec.to = spec.from;
    }

    OrganizationModel::Ptr om;
    if(!filename.empty())
    {
        om = OrganizationModel::Ptr(new OrganizationModel(filename) );
    } else {
        om = OrganizationModel::Ptr(new OrganizationModel(spec.organizationModelIRI));
    }

    std::cout << "Logging into: " << logfile << std::endl;
    std::stringstream log;
    log << "# number of epochs: " << epochs << std::endl;
    log << "# minfeasible: " << minFeasible << std::endl;
    log << "# [model #] " << algebra::Connectivity::Statistics::getStatsDescription() << std::endl;

    ModelPoolIterator mit(spec.from, spec.to, spec.stepSize);
    while(mit.next())
    {
        ModelPool current = mit.current();
        std::vector<algebra::Connectivity::Statistics> stats = runModelPoolTest(om, current, epochs, minFeasible);

        std::vector<numeric::Stats<double> > numericStats = algebra::Connectivity::Statistics::compute(stats);
        // record the number of model instances
        ModelPool::const_iterator cit = current.begin();
        for(; cit != current.end(); ++cit)
        {
            log << cit->second;
            log << " ";
        }
        for(const numeric::Stats<double>& s : numericStats)
        {
            log << s.mean()
                << " "
                << s.stdev()
                << " ";
        }
        log << std::endl;
    }

    std::cout << log.str() << std::endl;

    std::ofstream saveLog(logfile, std::ofstream::out);
    saveLog << log.str();
    saveLog.close();

    return 0;
}
