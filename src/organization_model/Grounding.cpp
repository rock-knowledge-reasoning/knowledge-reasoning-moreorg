#include "Grounding.hpp"

namespace owl_om {
namespace organization_model {

Grounding::Grounding(const RequirementsGrounding& grounding)
    : mRequirementToResourceMap(grounding)
{}

bool Grounding::isComplete() const
{
    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        if(mip->second == Grounding::ungrounded())
        {
            return false;
        }
    }
    return true;
}

IRIList Grounding::ungroundedRequirements() const
{
    IRIList requirements;
    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        if(mip->second == Grounding::ungrounded())
        {
            requirements.push_back( mip->second );
        }
    }
    return requirements;
}

IRI Grounding::ungrounded()
{
    static IRI iri("?");
    return iri;
}

std::string Grounding::toString() const
{
    std::stringstream ss;
    ss << "Grounding:" << std::endl;

    RequirementsGrounding::const_iterator mip = mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        ss << "    " << mip->first  << " -> " << mip->second << std::endl;
    }
    return ss.str();
}

} // end namespace organization_model
} // end namespace owl_om
