#ifndef ORGANIZATION_MODEL_VOCABULARIES_OM_HPP
#define ORGANIZATION_MODEL_VOCABULARIES_OM_HPP

#include <owlapi/Vocabulary.hpp>

namespace moreorg {
namespace vocabulary {

class OM
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2014/01/om-schema#");
    VOCABULARY_DYNAMIC_EXTENSION

    VOCABULARY_ADD_WORD(ResourceModel);
    VOCABULARY_ADD_WORD(ActorModel);
    VOCABULARY_ADD_WORD(CompositeActorModel);
    VOCABULARY_ADD_WORD(CapabilityModel);
    VOCABULARY_ADD_WORD(InterfaceModel);
    VOCABULARY_ADD_WORD(ServiceModel);

    VOCABULARY_ADD_WORD(Resource);
    VOCABULARY_ADD_WORD(Agent);
    VOCABULARY_ADD_WORD(Actor);
    VOCABULARY_ADD_WORD(Capability);
    VOCABULARY_ADD_WORD(CompositeActor);
    VOCABULARY_ADD_WORD(CompositeAgent);
    VOCABULARY_ADD_WORD(Functionality);
    VOCABULARY_ADD_WORD(Interface);
    VOCABULARY_ADD_WORD(MechanicalInterface)
    VOCABULARY_ADD_WORD(ElectricalInterface)
    VOCABULARY_ADD_WORD(ElectroMechanicalInterface)
    VOCABULARY_ADD_WORD(Location);
    VOCABULARY_ADD_WORD(Service);
    VOCABULARY_ADD_WORD(PhysicalEntity);

    VOCABULARY_ADD_WORD(Requirement);

    // Properties
    VOCABULARY_ADD_WORD(compatibleWith);
    VOCABULARY_ADD_WORD(dependsOn);
    VOCABULARY_ADD_WORD(has);
    VOCABULARY_ADD_WORD(modelledBy);
    VOCABULARY_ADD_WORD(models);
    VOCABULARY_ADD_WORD(provides);
    VOCABULARY_ADD_WORD(uses);
    VOCABULARY_ADD_WORD(fulfills);
    VOCABULARY_ADD_WORD(hasTransportCapacity);

    VOCABULARY_ADD_WORD(transportCapacity);
    VOCABULARY_ADD_WORD(transportDemand);
    VOCABULARY_ADD_WORD(transportMass);
    VOCABULARY_ADD_WORD(transportVolume);

    VOCABULARY_ADD_WORD(energyCapacity);


    VOCABULARY_ADD_WORD(nominalLength);
    VOCABULARY_ADD_WORD(nominalWidth);
    VOCABULARY_ADD_WORD(nominalHeight);

    VOCABULARY_ADD_WORD(mass);

    VOCABULARY_ADD_WORD(probabilityOfFailure);

    VOCABULARY_ADD_WORD(inferFrom);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // OWLAPI_VOCABULARIES_OM_HPP
