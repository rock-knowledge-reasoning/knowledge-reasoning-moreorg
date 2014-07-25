#ifndef OWL_OM_VOCABULARIES_OM_HPP
#define OWL_OM_VOCABULARIES_OM_HPP

namespace owl_om {
namespace vocabulary {

class OM
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2014/01/om-schema#");
    VOCABULARY_DYNAMIC_EXTENSION

    VOCABULARY_ADD_WORD(ResourceModel);
    VOCABULARY_ADD_WORD(ActorModel);
    VOCABULARY_ADD_WORD(CapabilityModel);
    VOCABULARY_ADD_WORD(InterfaceModel);
    VOCABULARY_ADD_WORD(ServiceModel);

    VOCABULARY_ADD_WORD(Resource);
    VOCABULARY_ADD_WORD(Actor);
    VOCABULARY_ADD_WORD(Capability);
    VOCABULARY_ADD_WORD(CompositeActor);
    VOCABULARY_ADD_WORD(Interface);
    VOCABULARY_ADD_WORD(Location);
    VOCABULARY_ADD_WORD(Service);

    // Properties
    VOCABULARY_ADD_WORD(compatibleWith);
    VOCABULARY_ADD_WORD(dependsOn);
    VOCABULARY_ADD_WORD(has);
    VOCABULARY_ADD_WORD(modelledBy);
    VOCABULARY_ADD_WORD(provides);
    VOCABULARY_ADD_WORD(uses);
};

} // end namespace vocabulary
} // end namespace owl_om
#endif // OWL_OM_VOCABULARIES_OM_HPP
