#include "ResourceMatch.hpp"

namespace owlapi {
namespace csp {

ResourceMatch::ResourceMatch(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology)
    : mQuery(query)
    , mResourcePool(resourcePool)
    // Create a new integer variable with query.size() variables and the
    // domain ranging von 0 to resourcePool.size() -1 (see Gecode 4.2.1)
    , mAssignment(*this, query.size(), 0, resourcePool.size() -1)
    , mAsk(ontology)
{
    for(int i = 0; i < mAssignment.size(); ++i)
    {
        for(size_t j = 0; j < mResourcePool.size(); ++j)
        {
            owlapi::model::IRI requestedResource = mQuery[i];
            owlapi::model::IRI availableResource = mResourcePool[j];

            // This constrains to that the ith requested resource cannot be
            // assigned the jth resource in the pool
            //
            // Check if query can be fulfilled by the same model or a
            // subclass of such model
            if(requestedResource != availableResource && !mAsk.isSubclassOf(availableResource, requestedResource) )
            {
                LOG_WARN_S << availableResource << " is not " << requestedResource;
                rel(*this, mAssignment[i], Gecode::IRT_NQ, j);
            } else {
                LOG_WARN_S << availableResource << " is " << requestedResource;
            }
        }
    }
    // Each resource can only be assigned one, i.e. this make assignement
    // distinct
    distinct(*this, mAssignment);
}

ResourceMatch::ResourceMatch(bool share, ResourceMatch& s)
    : Gecode::Space(share, s)
    , mQuery(s.mQuery)
    , mResourcePool(s.mResourcePool)
    , mAsk(s.mAsk)
{
    mAssignment.update(*this, share, s.mAssignment);
}

Gecode::Space* ResourceMatch::copy(bool share)
{
    return new ResourceMatch(share,*this);
}

//virtual void constrain(const Gecode::Space& _b)
//{
//    //const ResourceMatch& b = static_cast<const ResourceMatch&>(_b);
//    //
//    //// Number of used components
//    //// Determine number of used values in b
//    //int valuesCount = 0;
//    //std::vector<int> values;
//    //for(IntVarArray::const_iterator it = b.assignments_int.begin(); it != b.assignments_int.end(); ++it)
//    //{
//    //    // if not contains...
//    //    if(std::find(values.begin(), values.end(), it->val()) == values.end())
//    //    {
//    //        values.push_back(it->val());
//    //        valuesCount++;
//    //    }
//    //}
//    //
//    //// We must have at most that many components used as the so far best solution
//    //// FIXME LQ, and stuff below
//    //nvalues(*this, assignments_int, IRT_LE, valuesCount);
//    //
//    //// If we have an equal amount of values used, the number of reconfigured components must be less
//    //BoolVar equalAmountOfValues;
//    ////nvalues(*this, assignments_int, IRT_LQ, valuesCount, equalAmountOfValues);
//    //
//    //std::cout << " Adding best search constraint. This ";
//    //print();
//    //std::cout << " must be better than so far best ";
//    //b.print();
//}


ResourceMatch* ResourceMatch::solve(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology)
{
    // Setup initial solution / problem
    ResourceMatch* matching = new ResourceMatch(query, resourcePool, ontology);

    // Setup Branch and bound search
    // alternative is depth first search
    Gecode::BAB<ResourceMatch> searchEngine(matching);
    delete matching;

    // Start search
    ResourceMatch* best = NULL;
    while(ResourceMatch* matching = searchEngine.next())
    {
        if(best != NULL)
        {
            delete best;
        }
        // Save current solution as best
        best = matching;
    }

    if(best == NULL)
    {
        throw std::runtime_error("owlapi::csp::ResourceMatch: no solution found");
    }
    return best;
}

std::string ResourceMatch::toString() const 
{
    std::stringstream ss;
    ss << "ResourceMatch: { " << std::endl;
    for(int i = 0; i < mAssignment.size(); i++)
    {
        ss << "    " << mQuery[i] << " := ";
        if(mAssignment[i].assigned())
        {
            ss << mResourcePool[mAssignment[i].val()];
        } else
        {
            ss << "position range: " << mAssignment[i];
        }
        ss << ", "<< std::endl;
    }
    ss << "}" << std::endl;
    return ss.str();
}

owlapi::model::IRI ResourceMatch::getAssignment(const owlapi::model::IRI& iri) const
{
    owlapi::model::IRIList::const_iterator cit = std::find(mQuery.begin(), mQuery.end(), iri);
    if(cit != mQuery.end())
    {
        int index = cit - mQuery.begin();
        return mResourcePool[ mAssignment[index].val() ];
    }
    throw std::runtime_error("ResourceMatch::getAssignment: there exists no assignment for '" + iri.toString() + "'");
}


} // end namespace cps
} // end namespace owlapi
