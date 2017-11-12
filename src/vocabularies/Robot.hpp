#ifndef ORGANIZATION_MODEL_VOCABULARIES_ROBOT_HPP
#define ORGANIZATION_MODEL_VOCABULARIES_ROBOT_HPP

#include <owlapi/Vocabulary.hpp>

namespace organization_model {
namespace vocabulary {

class Robot
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2014/01/om-schema#");
    VOCABULARY_DYNAMIC_EXTENSION

    // Properties
    VOCABULARY_ADD_WORD(minAcceleration);
    VOCABULARY_ADD_WORD(maxAcceleration);
    VOCABULARY_ADD_WORD(nominalAcceleration);

    VOCABULARY_ADD_WORD(minVelocity);
    VOCABULARY_ADD_WORD(maxVelocity);
    VOCABULARY_ADD_WORD(nominalVelocity);

    VOCABULARY_ADD_WORD(energy);
    VOCABULARY_ADD_WORD(energyCapacity);
    VOCABULARY_ADD_WORD(supplyVoltage);
    VOCABULARY_ADD_WORD(nominalPowerConsumption);

    VOCABULARY_ADD_WORD(transportCapacity);
    VOCABULARY_ADD_WORD(transportDemand);

    VOCABULARY_ADD_WORD(mass);
};

} // end namespace vocabulary
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_VOCABULARIES_ROBOT_HPP
