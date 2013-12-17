#include <iostream>
#include <owl_om/Class.hpp>
#include <owl_om/Vocabulary.hpp>

int main(int argc, char** argv)
{
    using namespace owl_om;

    std::cout << vocabulary::OWL::BaseUri() << std::endl;
    std::cout << vocabulary::OWL::Class() << std::endl;

    Class a("TEST");
}
