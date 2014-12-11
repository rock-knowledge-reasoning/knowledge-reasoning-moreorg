#include "OWLLiteralDouble.hpp"

#include <boost/lexical_cast.hpp>

namespace owlapi {
namespace model {

double OWLLiteralDouble::getDouble() const
{
    return boost::lexical_cast<double>(mValue);
}

} // end namespace model
} // end namespace owlapi
