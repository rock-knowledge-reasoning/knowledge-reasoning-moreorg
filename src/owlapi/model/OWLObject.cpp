#include "OWLObject.hpp"
#include "OWLAxiomVisitor.hpp"

namespace owlapi {
namespace model {

void OWLObject::accept(boost::shared_ptr<OWLAxiomVisitor> visitor)
{
    throw NotImplemented("owlapi::model::OWLObject::accept");
}

bool OWLObject::isBottomEntity()
{
    throw NotImplemented("owlapi::model::OWLObject::isBottomEntity");
}

bool OWLObject::isTopEntity()
{
    throw NotImplemented("owlapi::model::OWLObject::isTopEntity");
}

} // end namespace model
} // end namespace owlapi
