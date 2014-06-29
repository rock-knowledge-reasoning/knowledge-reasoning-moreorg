#include <iostream>
#include <stdio.h>
#include <owl_om/Ontology.hpp>

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
        Ontology::Ptr ontology = Ontology::fromFile( o_filename );
        db::query::Results results = ontology->findAll("?s","?p","?o");
        printf("Results:\n%s", results.toString().c_str());
        printf("\n-----------------------------------\n");
        printf("Resulting Ontology:\n%s", ontology->toString().c_str());
    } else {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
    }
}
