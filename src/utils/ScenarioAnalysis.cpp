#include "organization_model/ccf/Scenario.hpp"

#include <set>
#include <iostream>
#include <sstream>
#include <math.h>
#include <base/Time.hpp>
#include <boost/program_options.hpp>

int main(int argc, char** argv)
{
    using namespace multiagent::ccf;

    namespace po = boost::program_options;

    po::options_description description("allowed options");
    description.add_options()
        ("help","describe arguments")
        ("configuration", po::value<std::string>(), "Path to the scenario configuration file")
        ("coalition_size", po::value<size_t>(), "Maximum size of coalition (default: 2)")
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

    size_t maxCoalitionSize = 2;
    if(vm.count("coalition_size"))
    {
        maxCoalitionSize = vm["coalition_size"].as<size_t>();
    }

    std::string outputFilename = "/tmp/organization_model-ccf-analysis.log";
    if(vm.count("output"))
    {
        outputFilename = vm["output"].as<std::string>();
    }

    std::string configurationFilename;
    Scenario scenario;
    if(vm.count("configuration"))
    {
        configurationFilename = vm["configuration"].as<std::string>();
        scenario = Scenario::fromFile(configurationFilename);
    } else {
        scenario = Scenario::fromConsole();
    }

    std::stringstream ss;
    std::cout << scenario.rowDescription() << std::endl;
    ss << scenario.rowDescription() << std::endl;;

    for(size_t coalitionSize = 2; coalitionSize <= maxCoalitionSize; ++coalitionSize)
    {
        base::Time start = base::Time::now();
        Scenario s = scenario;
        s.compute(coalitionSize);
        ss << s.report();
        std::cout << s.report();
        start = base::Time::now();
        //size_t count = s.createCompositeActorTypes(coalitionSize);
        //std::cout << "Agent types: " << count << std::endl;
    }
    std::cout << ss.str();
    return 0;
}
