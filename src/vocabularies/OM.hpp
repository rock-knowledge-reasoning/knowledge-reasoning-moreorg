#ifndef OWL_OM_VOCABULARIES_OM_HPP
#define OWL_OM_VOCABULARIES_OM_HPP

namespace owl_om {
namespace vocabulary {

class OM
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2014/01/om-schema#");
    VOCABULARY_DYNAMIC_EXTENSION
    VOCABULARY_ADD_WORD(Actor);
    VOCABULARY_ADD_WORD(ActorModel);
    VOCABULARY_ADD_WORD(CompositeActor);
    VOCABULARY_ADD_WORD(Interface);
    VOCABULARY_ADD_WORD(Service);
    VOCABULARY_ADD_WORD(ServiceModel);
    VOCABULARY_ADD_WORD(Location);

    // Properties
    VOCABULARY_ADD_WORD(dependsOn);
    VOCABULARY_ADD_WORD(has);
    VOCABULARY_ADD_WORD(provides);
    VOCABULARY_ADD_WORD(uses);
    VOCABULARY_ADD_WORD(compatibleWith);
    VOCABULARY_ADD_WORD(modelledBy);
};

} // end namespace vocabulary
} // end namespace owl_om
#endif // OWL_OM_VOCABULARIES_OM_HPP
