#include "OWLLiteralInteger.hpp"

#include <boost/lexical_cast.hpp>

namespace owlapi {
namespace model {

int OWLLiteralInteger::getInteger() const
{
    return boost::lexical_cast<int>(mValue);
}

} // end namespace model
} // end namespace owlapi
