#ifndef OWLAPI_CSP_RESOURCE_MATCHING_HPP
#define OWLAPI_CSP_RESOURCE_MATCHING_HPP

#include <gecode/int.hh>
#include <gecode/search.hh>

#include <owl_om/owlapi/model/OWLOntology.hpp>
#include <owl_om/owlapi/model/OWLOntologyAsk.hpp>
#include <base/Logging.hpp>

namespace owlapi {
namespace csp {


class ResourceMatching : public Gecode::Space
{
    owlapi::model::IRIList mQuery;
    owlapi::model::IRIList mResourcePool;

    owlapi::model::OWLOntologyAsk mAsk;

    /**
     * Assignments of query resources to pool resources. This is what has to be solved.
     */
    Gecode::IntVarArray mAssignment;

public:

    ResourceMatching(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology)
        : mQuery(query)
        , mResourcePool(resourcePool)
        , mAssignment(*this, query.size(), 0, resourcePool.size() -1)
        , mAsk(ontology)
    {
        for(size_t i = 0; i < mAssignment.size(); ++i)
        {
            for(size_t j = 0; j < mResourcePool.size(); ++j)
            {
                owlapi::model::IRI requestedResource = mQuery[i];
                owlapi::model::IRI availableResource = mResourcePool[j];

                // This constrains to that the ith requested resource cannot be
                // assigned the jth resource in the pool
                if(requestedResource != availableResource && !mAsk.isSubclassOf(availableResource, requestedResource) )
                {
                    LOG_WARN_S << availableResource << " is not " << requestedResource;
                    rel(*this, mAssignment[i], Gecode::IRT_NQ, j);
                } else {
                    LOG_WARN_S << availableResource << " is " << requestedResource;
                }
            }
        }
    }

    /**
     * Search support
     * This copy constructor is required for the search engine
     * and it has to provide a deep copy
     */
    ResourceMatching(bool share, ResourceMatching& s)
        : Gecode::Space(share, s)
        , mQuery(s.mQuery)
        , mResourcePool(s.mResourcePool)
        , mAsk(s.mAsk)
    {
        mAssignment.update(*this, share, s.mAssignment);
    }

    /**
     * This method is called by the search engine
     */
    virtual Gecode::Space* copy(bool share)
    {
        return new ResourceMatching(share,*this);
    }

    ///*
    // * constrain function for best solution search. the
    // * currently best solution _b is passed and we have to constraint that this solution can only
    // * be better than b, for it to be excluded if it isn't
    // */
    //virtual void constrain(const Gecode::Space& _b)
    //{
    //    //const ResourceMatching& b = static_cast<const ResourceMatching&>(_b);
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

    /**
     * Construct a solution with an initial situation to search
     */
    static ResourceMatching* solve(owlapi::model::IRIList query, owlapi::model::IRIList resourcePool, owlapi::model::OWLOntology::Ptr ontology)
    {
        // Setup initial solution / problem
        ResourceMatching* matching = new ResourceMatching(query, resourcePool, ontology);

        // Setup Branch and bound search
        // alternative is depth first search
        Gecode::BAB<ResourceMatching> searchEngine(matching);
        delete matching;

        // Start search
        ResourceMatching* best = NULL;
        while(ResourceMatching* matching = searchEngine.next())
        {
            if(best != NULL)
            {
                delete best;
            }
            // Save current solution as best
            best = matching;
            LOG_DEBUG_S << "FOUND solution" << best->toString();
            break;
        }

        if(best == NULL)
        {
            throw std::runtime_error("owlapi::csp::ResourceMatching: no solution found");
        }
        return best;
    }

    std::string toString() const 
    {
        std::stringstream ss;
        ss << "ResourceMatching: { " << std::endl;
        for(int i = 0; i < mAssignment.size(); i++)
        {
            ss << "\t" << mQuery[i] << "=";
            if(mAssignment[i].assigned())
            {
                ss << "value: " << mResourcePool[mAssignment[i].val()];
            } else
            {
                ss << "assign: " << mAssignment[i];
            }
            ss << ", "<< std::endl;
        }
        ss << "}" << std::endl;
        return ss.str();
    }

};

} // end namespace cps
} // end namespace owlapi
#endif // OWLAPI_CSP_RESOURCE_MATCHING_HPP
