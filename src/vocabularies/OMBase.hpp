#ifndef ORGANIZATION_MODEL_VOCABULARIES_OMBASE_HPP
#define ORGANIZATION_MODEL_VOCABULARIES_OMBASE_HPP

#include <owlapi/Vocabulary.hpp>

namespace moreorg {
namespace vocabulary {

class OMBase
{
public:
    VOCABULARY_BASE_IRI("http://www.rock-robotics.org/2014/01/om-base#");
    VOCABULARY_DYNAMIC_EXTENSION

    VOCABULARY_ADD_WORD(ProbabilityDensityFunction);
    VOCABULARY_ADD_WORD(ConstantPDF);
    VOCABULARY_ADD_WORD(WeibullPDF);
    VOCABULARY_ADD_WORD(ExponentialPDF);
    VOCABULARY_ADD_WORD(p);
    VOCABULARY_ADD_WORD(alpha);
    VOCABULARY_ADD_WORD(beta);
    VOCABULARY_ADD_WORD(lambda);
};

} // end namespace vocabulary
} // end namespace owlapi
#endif // ORGANIZATION_MODEL_VOCABULARIES_OMBASE_HPP
