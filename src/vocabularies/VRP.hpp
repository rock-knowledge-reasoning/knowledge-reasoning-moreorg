#ifndef ORGANIZATION_MODEL_VOCABULARIES_VRP_HPP
#define ORGANIZATION_MODEL_VOCABULARIES_VRP_HPP

#include <owlapi/Vocabulary.hpp>

namespace organization_model {
namespace vocabulary {

class VRP
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2017/11/vrp#");
    VOCABULARY_DYNAMIC_EXTENSION

    VOCABULARY_ADD_WORD(Commodity);
    VOCABULARY_ADD_WORD(Vehicle);
};

} // end namespace vocabulary
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_VOCABULARIES_VRP_HPP
