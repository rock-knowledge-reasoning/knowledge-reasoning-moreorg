#include <iostream>
#include <stdio.h>
#include <owl_om/db/KnowledgeBase.hpp>

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
        db::KnowledgeBase::Ptr kb = db::KnowledgeBase::fromFile( o_filename );
        db::query::Results results = kb->findAll("?s","?p","?o");
        printf("Results: %s", results.toString().c_str());
    } else {
        fprintf(stderr, "Please provide a filename");
    }
}
