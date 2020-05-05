#include "../OrganizationModelAsk.hpp"
#include "../vocabularies/OM.hpp"
#include "../facades/Robot.hpp"
#include "../io/LatexWriter.hpp"

#include <set>
#include <iostream>
#include <sstream>
#include <math.h>
#include <boost/program_options.hpp>

using namespace owlapi::model;
using namespace moreorg;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description description("allowed options");
    description.add_options()
        ("help","describe arguments")
        ("om", po::value<std::string>(), "path or iri to the organization model")
        ("latex", "Activate latex export")
        ("compact", "Activate latex export in a single table for all agents")
        ("columns", po::value<size_t>(), "Number of columns of the latex table, when in compact mode")
        ("output", po::value<size_t>(), "Path to the output file (default /tmp/moreorg-ccf-analysis.log")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
        std::cout << description << std::endl;
        exit(1);
    }

    std::string outputFilename = "/tmp/moreorg-reader.txt";
    if(vm.count("output"))
    {
        outputFilename = vm["output"].as<std::string>();
    }

    OrganizationModel::Ptr organizationModel;
    if(vm.count("om"))
    {
        std::string om = vm["om"].as<std::string>();
        if(om.substr(0,7) == "http://")
        {
            owlapi::model::IRI iri(om);
            organizationModel = OrganizationModel::getInstance(iri);
        } else {
            organizationModel = OrganizationModel::getInstance(om);
        }
    } else {
        std::cout << description << std::endl;
        exit(1);
    }

    if(vm.count("latex"))
    {
        OrganizationModelAsk ask(organizationModel);
        owlapi::model::IRIList agentTypes = ask.ontology().allSubClassesOf(vocabulary::OM::Actor());
        std::stringstream ss;
        std::vector<facades::Robot> robots;
        for(const owlapi::model::IRI& agentType : agentTypes)
        {
            try {
                ModelPool modelPool;
                modelPool[agentType] = 1;
                OrganizationModelAsk lask(organizationModel, modelPool, false);
                facades::Robot robot = facades::Robot::getInstance(agentType, lask);
                if(!vm.count("compact"))
                {
                    ss << io::LatexWriter::toString(robot);
                    ss << std::endl;
                } else {
                    robots.push_back(robot);
                }
            } catch(const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }

        if(vm.count("compact"))
        {
            size_t columns = robots.size();
            if(vm.count("columns"))
            {
                columns = vm["columns"].as<size_t>();
            }
            std::vector<facades::Robot> subGroup;
            for(size_t i = 0; i < robots.size(); ++i)
            {
                subGroup.push_back(robots[i]);
                if((i+1)%columns == 0)
                {
                    ss << io::LatexWriter::toString(subGroup);
                    subGroup.clear();
                }
            }
            if(!subGroup.empty())
            {
                ss << io::LatexWriter::toString(subGroup);
            }

        }
        std::cout << ss.str();
    }

    return 0;
}
