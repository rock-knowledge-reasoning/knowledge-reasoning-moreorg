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
using namespace organization_model;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;

    po::options_description description("allowed options");
    description.add_options()
        ("help","describe arguments")
        ("om", po::value<std::string>(), "path or iri to the organization model")
        ("latex", "Activate latex export")
        ("output", po::value<size_t>(), "Path to the output file (default /tmp/organization_model-ccf-analysis.log")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, description), vm);
    po::notify(vm);

    if(vm.count("help"))
    {
        std::cout << description << std::endl;
        exit(1);
    }

    std::string outputFilename = "/tmp/organization_model-reader.txt";
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
        for(const owlapi::model::IRI& agentType : agentTypes)
        {
            try {
                ModelPool modelPool;
                modelPool[agentType] = 1;
                OrganizationModelAsk lask(organizationModel, modelPool, false);
                facades::Robot robot = facades::Robot::getInstance(agentType, lask);
                ss << io::LatexWriter::toString(robot);
                ss << std::endl;
            } catch(const std::runtime_error& e)
            {
                std::cerr << e.what();
            }
        }
        std::cout << ss.str();
    }

    return 0;
}
