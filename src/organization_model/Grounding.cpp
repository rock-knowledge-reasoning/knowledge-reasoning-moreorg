#include "Grounding.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace moreorg {

Grounding::Grounding(const RequirementsGrounding& grounding)
    : mRequirementToResourceMap(grounding)
{
}

bool Grounding::isComplete() const
{
    RequirementsGrounding::const_iterator mip =
        mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        if(!isComplete(mip->second))
        {
            return false;
        }
    }
    return true;
}

bool Grounding::isComplete(const std::vector<IRI>& partialGrounding) const
{
    std::vector<IRI>::const_iterator cit = partialGrounding.begin();
    for(; cit != partialGrounding.end(); ++cit)
    {
        if(*cit == Grounding::ungrounded())
        {
            return false;
        }
    }

    return true;
}

RequirementsGrounding Grounding::ungroundedRequirements() const
{
    RequirementsGrounding ungroundedRequirements;
    RequirementsGrounding::const_iterator mip =
        mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        const std::vector<IRI>& groundings = mip->second;
        std::vector<IRI>::const_iterator cit = groundings.begin();
        for(; cit != groundings.end(); ++cit)
        {
            if(*cit == Grounding::ungrounded())
            {
                ungroundedRequirements[mip->first].push_back(*cit);
            }
        }
    }
    return ungroundedRequirements;
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

    RequirementsGrounding::const_iterator mip =
        mRequirementToResourceMap.begin();
    for(; mip != mRequirementToResourceMap.end(); ++mip)
    {
        ss << "Requirement:" << std::endl;
        ss << "    " << mip->first << std::endl;
        ss << "Grounding:" << std::endl;
        ss << mip->second << std::endl;
    }
    return ss.str();
}

} // end namespace moreorg
} // end namespace moreorg
