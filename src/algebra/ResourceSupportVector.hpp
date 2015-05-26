#ifndef ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP
#define ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP

#include <stdexcept>
#include <base/Eigen.hpp>
#include <owlapi/model/IRI.hpp>

namespace organization_model {
namespace algebra {

/**
 * A ResourceSupportVector describes resource availability as a vector
 * Each dimension stands for a specific resource type
 * The values provides the cardinality i.e. availability
 * of the resource which corresponding to that dimension
 *
 */
class ResourceSupportVector
{
public:
    ResourceSupportVector() {}

    /**
     *
     */
    ResourceSupportVector(const base::VectorXd& sizes, const owlapi::model::IRIList& labels = owlapi::model::IRIList());

    double& operator()(uint32_t dimension) { return mSizes(dimension); }

    const double& operator()(uint32_t dimension) const { return mSizes(dimension); }

    uint32_t getNumberOfDimensions() const { return mSizes.size(); }

    /**
     * Get intersection of two support
     */
    static ResourceSupportVector intersection(const ResourceSupportVector& a, const ResourceSupportVector& b);

    /**
     * Check if this resource support vector contains the other
     */
    bool contains(const ResourceSupportVector& other) const;

    bool fullSupportFrom(const ResourceSupportVector& other) const;

    bool partialSupportFrom(const ResourceSupportVector& other) const;

    /**
     * Compute the missing support (delta between this and other) as ResourceSupportVector
     */
    ResourceSupportVector missingSupportFrom(const ResourceSupportVector& other) const;

    /**
     * Compute the degree of support 's' with interpretation as follows:
     * s = 1, ideal support, exact number of resources are provided
     * s = 0, no support, no required resources are provided
     * s > 1, over support, more than the required resources are provided
     * 1 > s > 0, partial support, some required resources are provided
     *
     */
    double degreeOfSupport(const ResourceSupportVector& other) const;

    bool isNull() const { return mSizes.norm() == 0; }

    /**
     * Get labels of the dimensions
     * \return labels
     */
    owlapi::model::IRIList getLabels() const { return mLabels; }

    std::string toString() const;

protected:
    static void checkDimensions(const ResourceSupportVector& a, const ResourceSupportVector& b);

    static bool isNonNegative(const ResourceSupportVector& a);

private:
    base::VectorXd mSizes;
    owlapi::model::IRIList mLabels;

};

} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP
