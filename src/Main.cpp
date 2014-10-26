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
        om.createNewFromModel(OM::resolve("BaseCamp"), true);
        om.createNewFromModel(OM::resolve("BaseCamp"), true);
        om.createNewFromModel(OM::resolve("BaseCamp"), true);
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
        om.createNewFromModel(OM::resolve("PayloadCamera"), true);

        om.refresh(true);
        std::cout << om.getStatistics();
    } else {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    }

}
