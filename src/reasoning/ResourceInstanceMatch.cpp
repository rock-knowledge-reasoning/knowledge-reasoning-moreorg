#include "ResourceInstanceMatch.hpp"

#include <numeric/Combinatorics.hpp>
#include <algorithm>
#include <gecode/minimodel.hh>
#include <gecode/gist.hh>

#include "../vocabularies/OM.hpp"
#include "../Agent.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace reasoning {

std::string ResourceInstanceMatch::Solution::toString(uint32_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << "Solution:" << std::endl;
    std::map<ModelBound, ResourceInstance::List>::const_iterator cit = mAssignments.begin();
    for(; cit != mAssignments.end(); ++cit)
    {
        ss << hspace << "    requirement: " << std::endl;
        ss << cit->first.toString(indent + 8) << std::endl;
        ss << hspace << "    assignment: " << std::endl;
        ss << ResourceInstance::toString(cit->second, indent + 8) << std::endl;
    }

    return ss.str();
}

ResourceInstance::List ResourceInstanceMatch::Solution::getAssignments(const owlapi::model::IRI& model) const
{
    std::map<ModelBound, ResourceInstance::List>::const_iterator cit = mAssignments.begin();
    for(;cit != mAssignments.end(); ++cit)
    {
        if(cit->first.model == model)
        {
            return cit->second;
        }
    }

    throw std::invalid_argument("moreorg::reasoning::Resource::Solution::getAssignments: no assignments for model '"
            + model.toString() + "' in solution");
}

ResourceInstanceMatch::ResourceInstanceMatch(const ModelBound::List& required,
        const ResourceInstance::List& available,
        OrganizationModelAsk ask)
    : mRequiredModelBound(required)
    , mAvailableResources(available)
    , mModelAssignment(*this, /*width --> col*/ available.size()*
            /*height --> row*/ required.size(),0,
            std::max(ModelBound::getMaxResourceCount(required),
                (uint32_t) available.size()))
{
    Gecode::Matrix<Gecode::IntVarArray> modelAssignment(mModelAssignment,
            /*width --> col*/ available.size(), /* height --> row*/ required.size());

    for(size_t ri = 0; ri < mRequiredModelBound.size(); ++ri)
    {
        const ModelBound& requiredModelBound = mRequiredModelBound[ri];
        const owlapi::model::IRI& requiredModel = requiredModelBound.model;

        LOG_DEBUG_S << "Required model: " << requiredModel;

        Gecode::IntVarArgs args;

        for(size_t ai = 0; ai < mAvailableResources.size(); ++ai)
        {
            Gecode::IntVar m = modelAssignment(ai, ri);
            args << m;

            // lower bound is always 0
            rel(*this, m, Gecode::IRT_GQ, 0);

            // upper bound
            // check if the available model supports the required, i.e. is
            // either class or subclass
            // if so, then use max required value as upper bound
            const ResourceInstance& availableResource = mAvailableResources[ai];
            const owlapi::model::IRI& availableModel = availableResource.getModel();

            if(requiredModel == availableModel || ask.ontology().isSubClassOf(availableModel, requiredModel))
            {
                LOG_DEBUG_S << "Available model to fulfill '" << requiredModel << std::endl
                    << "    " << availableModel << std::endl
                    << "    maxRequired:" << requiredModelBound.max << std::endl
                    << "    available:" << availableResource.toString() << std::endl
                    ;

                rel(*this, m, Gecode::IRT_LQ, requiredModelBound.max);
                // Instance at maximum be of count 1
                rel(*this, m, Gecode::IRT_LQ, 1);
            } else {
                // does not fulfill the requirement so force to 0
                // i.e. no support by this available model
                rel(*this, m, Gecode::IRT_LQ, 0);
            }
        }
        LOG_DEBUG_S << "Required instances of model: " << requiredModel << ", minimum: " << requiredModelBound.min;
        // Row requires a minimum of resources to fulfill this particular requirement
        // sum of available (and valid supporting ones) should be at a minimum
        // of the required
        rel(*this, sum(args) >= requiredModelBound.min);
    }


    //for(size_t ai = 0; ai < mAvailableResources.size(); ++ai)
    //{
    //    const ResourceInstance& availableResource = mAvailableResource[ai];

    //    Gecode::IntVarArgs args;
    //    for(size_t ri = 0; ri < mRequiredModelBound.size(); ++ri)
    //    {
    //        Gecode::IntVar m = modelAssignment(ai, ri);
    //        args << m;
    //    }
    //    // Maximum number of available resources should not exceed the
    //    rel(*this, sum(args) <= availableModelBound.max);
    //}

    // Set branchers -- since otherwise we will have no assigned solutions
    //branch(*this, mModelAssignment, Gecode::INT_VAR_SIZE_MAX(), Gecode::INT_VAL_SPLIT_MIN());
    branch(*this, mModelAssignment, Gecode::INT_VAR_MIN_MIN(), Gecode::INT_VAL_SPLIT_MIN());
    //branch(*this, mModelAssignment, Gecode::INT_VAR_NONE(), Gecode::INT_VAL_SPLIT_MIN());

    //Gecode::Gist::Print<ResourceInstanceMatch> p("Print solution");
    //Gecode::Gist::Options o;
    //o.inspect.click(&p);
    //Gecode::Gist::bab(this, o);

}

ResourceInstanceMatch::ResourceInstanceMatch(ResourceInstanceMatch& other)
    : Gecode::Space(other)
    , mRequiredModelBound(other.mRequiredModelBound)
    , mAvailableResources(other.mAvailableResources)
{
    mModelAssignment.update(*this,other.mModelAssignment);
}

Gecode::Space* ResourceInstanceMatch::copy()
{
    return new ResourceInstanceMatch(*this);
}

ResourceInstanceMatch::Solution ResourceInstanceMatch::solve(const ModelBound::List& required, const ResourceInstance::List& available, const OrganizationModelAsk& ask)
{
    LOG_INFO_S << "Solve:" << std::endl
        << "    required: " << std::endl
        << "    " << ModelBound::toString(required, 8) << std::endl
        << "    available: " << std::endl
        << "    " << ResourceInstance::toString(available, 8) << std::endl;

    ResourceInstanceMatch* match = new ResourceInstanceMatch(required, available, ask);
    ResourceInstanceMatch* solvedMatch = match->solve();
    delete match;
    match = NULL;

    Solution solution = solvedMatch->getSolution();
    delete solvedMatch;
    solvedMatch = NULL;
    return solution;
}


ResourceInstanceMatch::Solution ResourceInstanceMatch::getSolution() const
{
    Gecode::Matrix<Gecode::IntVarArray> modelAssignment(mModelAssignment, mAvailableResources.size(), mRequiredModelBound.size());

    Solution solution;
    for(size_t mi = 0; mi < mAvailableResources.size(); ++mi)
    {
        // Check if resource requirements holds
        for(size_t i = 0; i < mRequiredModelBound.size(); ++i)
        {

            Gecode::IntVar var = modelAssignment(mi, i);
            if(!var.assigned())
            {
                throw std::runtime_error("moreorg::reasoning::ResourceInstanceMatch::getSolution: value has not been assigned");
            }

            Gecode::IntVarValues v( var );
            if(v.val() != 0)
            {
                solution.addAssignment(mRequiredModelBound[i], mAvailableResources[mi]);
            }
        }

    }
    return solution;
}


void ResourceInstanceMatch::print(std::ostream& os) const
{
    os << this->toString() << std::endl;
}


ResourceInstanceMatch* ResourceInstanceMatch::solve()
{
    // Setup Branch and bound search
    // alternative is depth first search
    Gecode::BAB<ResourceInstanceMatch> searchEngine(this);
    //Gecode::DFS<ResourceInstanceMatch> searchEngine(this);

    // Start search
    ResourceInstanceMatch* best = NULL;
    while(ResourceInstanceMatch* matching = searchEngine.next())
    {
        delete best;
        // Save current solution as best
        best = matching;

        //We do not need optimization of the result,
        //just trying to validate sufficient condition,
        //so take first solution
        break;
    }

    if(best == NULL)
    {
        throw std::runtime_error("moreorg::reasoning::ResourceInstanceMatch: no solution found");
    }

    return best;
}

std::string ResourceInstanceMatch::toString() const
{
    std::stringstream ss;
    ss << "ResourceInstanceMatch: #" << mModelAssignment;
    return ss.str();
}

ModelBound::List ResourceInstanceMatch::toModelBoundList(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions)
{
    using namespace owlapi::model;
    std::map<owlapi::model::IRI, OWLCardinalityRestriction::MinMax> bounds = OWLObjectCardinalityRestriction::getBounds(restrictions);

    ModelBound::List modelBounds;
    std::map<owlapi::model::IRI, OWLCardinalityRestriction::MinMax>::const_iterator cit = bounds.begin();
    for(; cit != bounds.end(); ++cit)
    {
        modelBounds.push_back( ModelBound(cit->first, cit->second.first, cit->second.second) );
    }

    return modelBounds;
}

ResourceInstance::List ResourceInstanceMatch::Solution::removeAssignmentsFromList(ResourceInstance::List &availableList)
        {
            ResourceInstance::List available(availableList);
            for (const auto pair : mAssignments)
            {
                for (auto ri : pair.second)
                {
                    auto listIterator = std::find_if(available.begin(), available.end(), [&ri](const ResourceInstance &availableAgent)
                    {
                        return ri.getName() == availableAgent.getName();
                    });

                    available.erase(listIterator);
                }
            }
            
            return available;
        }

bool ResourceInstanceMatch::isSupporting(const Agent& agent,
        const owlapi::model::IRI& serviceModel,
        const OrganizationModelAsk& ask,
        const owlapi::model::IRI& objectProperty)
{
    ResourceInstance::List providerResources = ask.getRelated(agent, objectProperty);
    std::vector<OWLCardinalityRestriction::Ptr> serviceRestrictions = ask.ontology().getCardinalityRestrictions(serviceModel, objectProperty);

    return isSupporting(providerResources, serviceRestrictions, ask);
}

bool ResourceInstanceMatch::isSupporting(const ResourceInstance::List& providerResources,
        const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& serviceRestrictions,
        const OrganizationModelAsk& ask)
{
    try {
        ModelBound::List requirements = toModelBoundList(serviceRestrictions);
        ResourceInstanceMatch::Solution fulfillment = ResourceInstanceMatch::solve(requirements, providerResources, ask);
        return true;
    } catch(const std::runtime_error& e)
    {
        return false;
    }
}

} // end namespace reasoning
} // end namespace moreorg
