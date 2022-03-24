#include "ResourceSupportVector.hpp"
#include <base-logging/Logging.hpp>
#include <math.h>
#include <moreorg/OrganizationModelAsk.hpp>
#include <sstream>

namespace moreorg {
namespace algebra {

std::map<SupportType, std::string> SupportTypeTxt = {
    {NO_SUPPORT, "no support"},
    {PARTIAL_SUPPORT, "partial support"},
    {FULL_SUPPORT, "full support"},
};

ResourceSupportVector::ResourceSupportVector()
    : mSizes()
    , mLabels(0)
{
}

ResourceSupportVector::ResourceSupportVector(
    const base::VectorXd& sizes,
    const owlapi::model::IRIList& labels)
    : mSizes(sizes)
    , mLabels(labels)
{
}

ResourceSupportVector
ResourceSupportVector::intersection(const ResourceSupportVector& a,
                                    const ResourceSupportVector& b)
{
    checkDimensions(a, b);

    uint32_t maxDimension = a.getNumberOfDimensions();
    base::VectorXd intersectionAB(maxDimension);

    for(uint32_t dim = 0; dim < maxDimension; ++dim)
    {
        intersectionAB(dim) = std::min(a(dim), b(dim));
    }

    return ResourceSupportVector(intersectionAB);
}

bool ResourceSupportVector::contains(const ResourceSupportVector& other) const
{
    checkDimensions(*this, other);

    uint32_t maxDimension = other.getNumberOfDimensions();
    for(uint32_t dim = 0; dim < maxDimension; ++dim)
    {
        if((*this)(dim) >= other(dim))
        {
            continue;
        } else
        {
            return false;
        }
    }
    return true;
}

bool ResourceSupportVector::fullSupportFrom(
    const ResourceSupportVector& other) const
{
    return other.contains(*this);
}

bool ResourceSupportVector::partialSupportFrom(
    const ResourceSupportVector& other) const
{
    return other.mSizes.dot(mSizes) != 0;
}

ResourceSupportVector ResourceSupportVector::operator*(double factor) const
{
    ResourceSupportVector scaledVector = *this;
    scaledVector.mSizes *= factor;
    return scaledVector;
}

ResourceSupportVector
ResourceSupportVector::operator+(const ResourceSupportVector& other) const
{
    ResourceSupportVector summedVector = *this;
    summedVector += other;
    return summedVector;
}

ResourceSupportVector&
ResourceSupportVector::operator+=(const ResourceSupportVector& other)
{
    if(other.mSizes.norm() == 0)
    {
        return *this;
    }

    if(other.mLabels != this->mLabels)
    {
        throw std::invalid_argument(
            "organization_mode::algebra::ResourceSupportVector::operator+"
            " cannot sum vectors since labels differ: '" +
            owlapi::model::IRI::toString(mLabels) + "' vs '" +
            owlapi::model::IRI::toString(other.mLabels) + "'");
    }
    mSizes += other.mSizes;
    return *this;
}

SupportType
ResourceSupportVector::getSupportFrom(const ResourceSupportVector& other,
                                      const OrganizationModelAsk& ask) const
{
    ResourceSupportVector a = this->embedClassRelationship(ask);
    ResourceSupportVector b = other.embedClassRelationship(ask);

    if(a.fullSupportFrom(b))
    {
        return FULL_SUPPORT;
    } else if(a.partialSupportFrom(b))
    {
        return PARTIAL_SUPPORT;
    } else
    {
        return NO_SUPPORT;
    }
}

ResourceSupportVector
ResourceSupportVector::getRatios(const ResourceSupportVector& other) const
{
    assert(other.size() != 0);
    uint32_t maxDimensions = other.size();
    ResourceSupportVector ratio(base::VectorXd::Zero(maxDimensions), mLabels);
    for(uint32_t dim = 0; dim < maxDimensions; ++dim)
    {
        double otherDim = other(dim);
        if(std::abs(otherDim) < 1E-05)
        {
            ratio(dim) = std::numeric_limits<double>::quiet_NaN();
        } else
        {
            ratio(dim) = mSizes(dim) / otherDim;
        }
    }
    return ratio;
}

ResourceSupportVector ResourceSupportVector::missingSupportFrom(
    const ResourceSupportVector& other) const
{
    base::VectorXd delta = other.mSizes - mSizes;
    uint32_t maxDimensions = delta.size();
    for(uint32_t dim = 0; dim < maxDimensions; ++dim)
    {
        double support = delta(dim);
        if(support < 0)
        {
            delta(dim) = std::abs(support);
        } else
        {
            delta(dim) = 0;
        }
    }
    return delta;
}

std::string ResourceSupportVector::toString(uint32_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent, ' ');
    ss << hspace << "ResourceSupportVector: " << std::endl;
    for(size_t i = 0; i < mLabels.size(); ++i)
    {
        if(mSizes.size() > static_cast<int>(i))
        {
            ss << hspace << "    " << mLabels[i] << ": " << mSizes(i)
               << std::endl;
        } else
        {
            ss << hspace << "    " << mLabels[i] << ": n/a" << std::endl;
        }
    }
    return ss.str();
}

ResourceSupportVector ResourceSupportVector::embedClassRelationship(
    const OrganizationModelAsk& ask) const
{
    using namespace owlapi::model;

    ResourceSupportVector supportVector = *this;

    uint32_t max = supportVector.size();
    std::vector<IRI> labels = supportVector.getLabels();
    for(uint32_t i = 0; i < max; ++i)
    {
        IRI i_model = labels[i];
        for(uint32_t a = i + 1; a < max; ++a)
        {
            IRI a_model = labels[a];

            if(ask.ontology().isSubClassOf(i_model, a_model))
            {
                supportVector(a) += supportVector(i);
            } else if(ask.ontology().isSubClassOf(a_model, i_model))
            {
                supportVector(i) += supportVector(a);
            }
        }
    }
    return supportVector;
}

void ResourceSupportVector::checkDimensions(const ResourceSupportVector& a,
                                            const ResourceSupportVector& b)
{
    if(a.getNumberOfDimensions() != b.getNumberOfDimensions())
    {
        LOG_WARN_S << "moreorg::algebra::ResourceSupportVector::intersection "
                   << " cannot compute intersection since number of dimensions "
                      "differ: "
                   << a.toString() << " vs. " << b.toString();

        throw std::invalid_argument(
            "moreorg::algebra::ResourceSupportVector::intersection \
                cannot compute intersection since number of dimensions differ: " +
            a.toString() + " vs. " + b.toString());
    }
}

bool ResourceSupportVector::isNonNegative(const ResourceSupportVector& a)
{
    uint32_t maxDim = a.getNumberOfDimensions();
    for(uint32_t dim = 0; dim < maxDim; ++dim)
    {
        if(a(dim) < 0)
        {
            return false;
        }
    }
    return true;
}

} // end namespace algebra
} // end namespace moreorg
