#ifndef ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP
#define ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP

#include <stdexcept>
#include <base/Eigen.hpp>
#include <owlapi/model/IRI.hpp>

namespace organization_model {

class OrganizationModelAsk;

namespace algebra {

/**
 * A ResourceSupportVector describes resource availability as a vector
 * Each dimension stands for a specific resource type
 * The values provides the cardinality i.e. availability
 * of the resource which corresponding to that dimension
 *
 * The ResourceSupportVector can accurately describe interdependencies
 * only when there are no overlapping dimensions, e.g., when
 * one dimension is the superclass of another computing the redunancy
 * needs to be done differently (and with a potential error).
 * Assuming the following situation:
 * Required resource are described by a vector: [1 1 1]^T, where
 * the corresponding (class labels) are [A B C]^T.
 * A is superclass of B, B is superclass of C. 
 * Available resources are described by: [0 0 3]^T, so that 
 * the requirements can be exactly covered.
 * However, using the dot product will fail since there is no exact match. 
 * Thus, we update the required resources that each dimension takes into 
 * account the required subclass instances:
 * [(1+2) (1+1) 1] = [3 2 1], and available accordingly [3 3 3].
 * It can be easily seen that the maximum achievable redundancy will be
 * exceeded with this update vector, thus we bound it based on the maximum
 * to be expected redundancy (0+0+3)/(1+1+1) = 1
 *
 */
enum SupportType { NO_SUPPORT, PARTIAL_SUPPORT, FULL_SUPPORT };

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

    SupportType getSupportFrom(const ResourceSupportVector& other, const OrganizationModelAsk& ask) const;

    /**
     * Compute the ratios per dimension, i.e. new(0) = this(0) / other(0);
     */
    ResourceSupportVector getRatios(const ResourceSupportVector& other) const;

    /**
     * Compute the missing support (delta between this and other) as ResourceSupportVector
     * ignores dimension where sufficient support is given
     */
    ResourceSupportVector missingSupportFrom(const ResourceSupportVector& other) const;

    bool isNull() const { return mSizes.norm() == 0; }

    /**
     * Get labels of the dimensions
     * \return labels
     */
    owlapi::model::IRIList getLabels() const { return mLabels; }

    std::string toString() const;

    size_t size() const { return mSizes.size(); }

    ResourceSupportVector embedClassRelationship(const OrganizationModelAsk& ask) const;

    ResourceSupportVector operator*(double factor) const;

protected:
    static void checkDimensions(const ResourceSupportVector& a, const ResourceSupportVector& b);

    static bool isNonNegative(const ResourceSupportVector& a);

    bool fullSupportFrom(const ResourceSupportVector& other) const;

    bool partialSupportFrom(const ResourceSupportVector& other) const;


private:
    base::VectorXd mSizes;
    owlapi::model::IRIList mLabels;

};

} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP
