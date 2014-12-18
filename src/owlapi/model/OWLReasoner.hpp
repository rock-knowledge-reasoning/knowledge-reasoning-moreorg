#ifndef OWLAPI_MODEL_OWL_REASONER_HPP
#define OWLAPI_MODEL_OWL_REASONER_HPP

namespace owlapi {
namespace model {

class OWLReasoner
{
public:
    virtual void dispose() {}

    /**
     * Flushes any changes stored in the buffer, which causes the reasoner to take
     * into consideration the changes of the current root ontology specified by the changes
     */
    void flush() {}



};

} // end namespace model
} // end namespace owlapi
#endif // OWLAPI_MODEL_OWL_REASONER_HPP
