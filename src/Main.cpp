#include <iostream>
#include <stdio.h>
#include <owl_om/OrganizationModel.hpp>

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

        using namespace owl_om;
        using namespace owl_om::vocabulary;

        om.createNewFromModel(OM::resolve("Sherpa"), true);
        om.createNewFromModel(OM::resolve("CREX"), true);
        om.createNewFromModel(OM::resolve("CREX"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);

        om.refresh();

        printf("Resulting Ontology:\n%s", om.ontology()->toString().c_str());
        printf("%s", om.getStatistics().toString().c_str());
    } else {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    }

}
