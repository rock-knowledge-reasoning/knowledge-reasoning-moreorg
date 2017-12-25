#ifndef ORGANIZATION_MODEL_ANALYSER_HPP
#define ORGANIZATION_MODEL_ANALYSER_HPP

#include "OrganizationModelAsk.hpp"
#include "StatusSample.hpp"
#include "Heuristics.hpp"

namespace organization_model {

/**
 * \class Analyser
 * \brief Analysis of the state of an organization model
 * \details The analyser is feed with the full state description
 * of an organization, i.e. positions over time and a description of
 * the activities
 */
class Analyser
{
public:
    typedef std::vector<StatusSample::ConstRawPtrList> SampleIndex;

    Analyser(const OrganizationModelAsk& ask);

    /**
     * Add a status sample
     */
    void add(const StatusSample& sample);

    /**
     * Create the index after adding the final status sample
     */
    void createIndex();

    /**
     * Retrieve all samples at a given time point
     */
    StatusSample::ConstRawPtrList samplesAt(size_t time);

    // QUERY INTERFACE
    Agent::List getCoalitionStructure(size_t time);

    std::vector<base::Position> getPositions(size_t time);

    // ORGANIZATIONAL METRICS
    // to be applied on to of this interface
    // https://en.wikipedia.org/wiki/Spatial_descriptive_statistics#Measures_of_spatial_dispersion

private:
    /// Resources
    OrganizationModelAsk mAsk;
    Heuristics mHeuristics;
    StatusSample::List mSamples;
    size_t mMinTime;
    size_t mMaxTime;

    SampleIndex mSampleIndex;

    /// indexing: fromTime - ToTime: key
    // queries:
    //     get information about atomic agent at time t
    //     get information about composite agents at time t

};

} // end namespace organization_model
#endif // ORGANIZATION_MODEL_ANALYSER_HPP
