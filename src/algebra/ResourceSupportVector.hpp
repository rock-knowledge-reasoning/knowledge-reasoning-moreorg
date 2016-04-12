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
 * of the resource which corresponds to that dimension.
 *
 * The ResourceSupportVector can accurately describe interdependencies
 * only when there are no overlapping dimensions, e.g., when
 * one dimension is the superclass of another computing the redundancy
 * will potentially contain an error, e.g.,
 * Assuming the following situation:
 *
 * Required resources are described by a vector: [1 1 1]^T, where
 * the corresponding (class labels) are [A B C]^T.
 * A is superclass of B, B is superclass of C.
 * Available resources are described by: [0 0 3]^T, so that
 * the requirements can be exactly covered.
 * However, using the dot product will fail since there is no exact match.
 * Thus, we update the required resources that each dimension takes into
 * account the required subclass instances:
 * [(1+2) (1+1) 1] = [3 2 1], i.e. three class of A are required two of B and 1
 * of C, and available accordingly [3 3 3].
 * It can be easily seen that the maximum achievable redundancy will be
 * exceeded with this update vector, thus we bound it based on the maximum
 * to be expected redundancy (0+0+3)/(1+1+1) = 1
 *
 */

/// The SupportType reflect the functional support provided by a set of
/// countable resources given some requirements
enum SupportType { NO_SUPPORT, PARTIAL_SUPPORT, FULL_SUPPORT };

extern std::map<SupportType, std::string> SupportTypeTxt;

class ResourceSupportVector
{
public:
    ResourceSupportVector();

    /**
     * Construct ResourceSupportVector
     * \param sizes Vector of cardinalities
     * \param labels Vector of labels describing the row
     */
    ResourceSupportVector(const base::VectorXd& supportValue, const owlapi::model::IRIList& labels = owlapi::model::IRIList());

    /**
     * Retrieve the support value of a given dimension and allow manipulation
     * \return reference to support value entry
     */
    double& operator()(uint32_t dimension) { return mSizes(dimension); }

    /**
     * Retrieve the support value of a given dimension
     * \return support value entry
     */
    const double& operator()(uint32_t dimension) const { return mSizes(dimension); }

    /**
     * Get the number of dimensions of this vector
     * \return number of dimensions
     */
    uint32_t getNumberOfDimensions() const { return mSizes.size(); }

    /**
     * Get intersection of two ResourceSupportVector -- providing the minimum
     * value for each dimension
     * \return ResourceSupportVector as [ min(a0,b0), min(a1,b1), min(a2,b2),
     * ... ]
     */
    static ResourceSupportVector intersection(const ResourceSupportVector& a, const ResourceSupportVector& b);

    /**
     * Check if this resource support vector contains the other, i.e. if the
     * following holds
     \f[
        V_{this}(i) \geq V_{other}(i) \for i = 1 \dots |V_{this}|
     \f]
     */
    bool contains(const ResourceSupportVector& other) const;

    /**
     * Check the support of a given requirement from another vector
     * \return one of FULL_SUPPORT, PARTIAL_SUPPORT or NO_SUPPORT
     */
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

    /**
     * Test if this ResourceSupportVector refers to a null vector (norm of the
     * underlying vector equals 0)
     */
    bool isNull() const { return mSizes.norm() == 0; }

    /**
     * Get labels of the dimensions
     * \return labels
     */
    owlapi::model::IRIList getLabels() const { return mLabels; }

    /**
     * Stringify object
     * \param indent Indentation in number of spaces
     * \return stringified object
     */
    std::string toString(uint32_t indent = 0) const;

    /**
     * Get the size of this vector
     */
    size_t size() const { return mSizes.size(); }

    /**
     * Extend the ResourceSupportVector to embed the class relationships in the
     * resource vector, e.g.,
     * if the original vector is
     * [1 2] for [BaseModel DerivedModel] the resulting vector will be [3 2]
     */
    ResourceSupportVector embedClassRelationship(const OrganizationModelAsk& ask) const;

    /**
     * Scale a ResourceSupportVector by a given factor
     * \param factor scale factor
     */
    ResourceSupportVector operator*(double factor) const;

    /**
     * Add two resource support vectors
     */
    ResourceSupportVector operator+(const ResourceSupportVector& other) const;

    /**
     * Add two resource support vectors
     */
    ResourceSupportVector& operator+=(const ResourceSupportVector& other);

protected:
    /**
     * Check if two ResourceSupportVector have the same dimension
     * \return True if both vectors have the same dimension, false otherwise
     */
    static void checkDimensions(const ResourceSupportVector& a, const ResourceSupportVector& b);

    /**
     * Check if ResourceSupportVector has only positive entries
     * \return True if ResourceSupportVector has only positive entries, false
     * otherwise
     */
    static bool isNonNegative(const ResourceSupportVector& a);

    /**
     * Check if requirements given by this object are supported by the
     * ResourceSupportVector given by other
     */
    bool fullSupportFrom(const ResourceSupportVector& other) const;

    /**
     * Check if requirements given by this object are partially supported by the
     * ResourceSupportVector given by other
     */
    bool partialSupportFrom(const ResourceSupportVector& other) const;


private:
    /// Vector where each dimension refers to a distinct resource model
    /// the actual values refer to the availability
    base::VectorXd mSizes;
    /// Labels of the models that correspond to each dimension
    owlapi::model::IRIList mLabels;

};

} // end namespace algebra
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ALGEBRA_RESOURCE_SUPPORT_VECTOR_HPP
