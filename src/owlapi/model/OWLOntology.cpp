#include "OWLOntology.hpp"
#include <owl_om/owlapi/KnowledgeBase.hpp>

namespace owlapi {
namespace model {

OWLOntology::OWLOntology()
    : mpKnowledgeBase(new KnowledgeBase())
{}

OWLOntology::~OWLOntology()
{
    delete mpKnowledgeBase;
}

} // end namespace model
} // end namespace owlapi
