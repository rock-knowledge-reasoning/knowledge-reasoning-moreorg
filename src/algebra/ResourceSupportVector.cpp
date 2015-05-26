#include "ResourceSupportVector.hpp"
#include <math.h>
#include <sstream>

namespace organization_model {
namespace algebra {

ResourceSupportVector::ResourceSupportVector(const base::VectorXd& sizes,
    const owlapi::model::IRIList& labels)
    : mSizes(sizes)
    , mLabels(labels)
{}

ResourceSupportVector ResourceSupportVector::intersection(const ResourceSupportVector& a, const ResourceSupportVector& b)
{
    checkDimensions(a,b);

    uint32_t maxDimension = a.getNumberOfDimensions();
    base::VectorXd intersectionAB(maxDimension);

    for(uint32_t dim = 0; dim < maxDimension; ++dim)
    {
        intersectionAB(dim) = std::min(a(dim), b(dim));
    }

    return ResourceSupportVector( intersectionAB );
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
        } else {
            return false;
        }
    }
    return true;
}

bool ResourceSupportVector::fullSupportFrom(const ResourceSupportVector& other) const
{
    double dos = degreeOfSupport(other);
    if(dos >= 1)
    {
        return true;
    }
    return false;
}

bool ResourceSupportVector::partialSupportFrom(const ResourceSupportVector& other) const
{
    double dos = degreeOfSupport(other);
    if(dos > 0 && dos < 1)
    {
        return true;
    }
    return false;
}

ResourceSupportVector ResourceSupportVector::missingSupportFrom(const ResourceSupportVector& other) const
{
    base::VectorXd delta = other.mSizes - mSizes;
    uint32_t maxDimensions = delta.size();
    for(uint32_t dim = 0; dim < maxDimensions; ++dim)
    {
        double support = delta(dim);
        if( support < 0)
        {
            delta(dim) = std::abs(support);
        } else {
            delta(dim) = 0;
        }
    }
    return delta;
}

double ResourceSupportVector::degreeOfSupport(const ResourceSupportVector& other) const
{
    // The projection of vector OS onto OR will give the degree of support,
    // where OS can be interpreted as available resources, and OR as required resources
    //
    // .. ............S
    // |             .
    // |             .
    // R             .
    // |             .
    // O-----------
    //

    // http://eigen.tuxfamily.org/dox-devel/classEigen_1_1MatrixBase.html#ad10353dcb54de8fbe27619cffd7fced5
    double squaredNorm = mSizes.squaredNorm(); //dot(mSizes);
    if(squaredNorm > 0)
    {
        // [1 1] -> current support [1camera 1battery]
        // [3 1] -> current support [3cameras 1battery]
        //
        // 1*3 + 1*1 = 4
        // 4 / sqrt(2) = 2.828
        //
        // double dotProduct = mSizes.dot(other.mSizes);
        // dotProduct / mSizes.norm() refers to the unit vector, but
        // since we want to refer to the actual size of the current
        // vector we divide a second time by the norm leading to the squaredNorm
        return mSizes.dot(other.mSizes) / squaredNorm;
    } else{
        throw std::runtime_error("organization_model::algebra::ResourceSupportVector::degreeOfSupport \
                norm is null of current resource support vector -- cannot compute support degree");
    }
}

std::string ResourceSupportVector::toString() const
{
    std::stringstream ss;
    ss << mSizes;
    return ss.str();
}

void ResourceSupportVector::checkDimensions(const ResourceSupportVector& a, const ResourceSupportVector& b)
{
    if(a.getNumberOfDimensions() != b.getNumberOfDimensions())
    {
        throw std::invalid_argument("organization_model::algebra::ResourceSupportVector::intersection \
                cannot compute intersection since number of dimensions differ");
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
} // end namespace organization_model
