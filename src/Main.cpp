#include <iostream>
#include <stdio.h>
#include <owl_om/OrganizationModel.hpp>
#include <boost/foreach.hpp>

int main(int argc, char** argv)
{
    using namespace owl_om;

    std::string o_filename;

    if(argc == 2)
    {
        o_filename = std::string(argv[1]);
    }

    if(!o_filename.empty())
    {
        OrganizationModel om( o_filename );
        om.setMaximumNumberOfLinks(2);

        using namespace owl_om;
        using namespace owl_om::vocabulary;

        om.createNewFromModel(OM::resolve("Sherpa"), true);
//        om.createNewFromModel(OM::resolve("Sherpa"), true);
//        om.createNewFromModel(OM::resolve("Sherpa"), true);
//        om.createNewFromModel(OM::resolve("Sherpa"), true);
        om.createNewFromModel(OM::resolve("CREX"), true);
        om.createNewFromModel(OM::resolve("CREX"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
//        om.createNewFromModel(OM::resolve("PayloadCamera"), true);

//        om.refresh();
//        InterfaceCombinationList d = om.generateInterfaceCombinationsCCF();
        InterfaceCombinationList d = om.generateInterfaceCombinations();

        //BOOST_FOREACH(const std::vector<InterfaceConnection>& composite, d)
        //{
        //    printf("Composite:\n");
        //    BOOST_FOREACH(const InterfaceConnection& ic, composite)
        //    {
        //        printf("    %s\n", ic.toString().c_str());
        //    }
        //}
        //printf("Resulting combinations #: %d\n", d.size());
        //Statistics stats = om.getStatistics();
        //printf("Stats: %s\n", stats.toString().c_str());
        //exit(0);


        printf("Resulting Ontology:\n%s", om.ontology()->toString().c_str());
        printf("%s", om.getStatistics().toString().c_str());
    } else {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    }

}
