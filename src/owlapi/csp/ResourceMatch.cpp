#include "ResourceMatch.hpp"
#include <algorithm>
#include <gecode/gist.hh>

using namespace owlapi::model;

namespace owlapi {
namespace csp {

ResourceMatch::ResourceMatch(const IRIList& query, const IRIList& resourcePool, OWLOntology::Ptr ontology)
    : mQuery(query)
    , mResourcePool(resourcePool)
    // Create a new integer variable with query.size() variables and the
    // domain ranging von 0 to resourcePool.size() -1 (see Gecode 4.2.1)
    , mAssignment(*this, query.size(), 0, resourcePool.size() -1)
{
    OWLOntologyAsk ask(ontology);
    for(int i = 0; i < mAssignment.size(); ++i)
    {
        for(size_t j = 0; j < mResourcePool.size(); ++j)
        {
            IRI requestedResource = mQuery[i];
            IRI availableResource = mResourcePool[j];

            // This constrains to that the ith requested resource cannot be
            // assigned the jth resource in the pool
            //
            // Check if query can be fulfilled by the same model or a
            // subclass of such model
            if(requestedResource != availableResource && !ask.isSubclassOf(availableResource, requestedResource) )
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
{
    LOG_WARN_S << "ResourceMatch: construct";
    mAssignment.update(*this, share, s.mAssignment);
}

Gecode::Space* ResourceMatch::copy(bool share)
{
    LOG_WARN_S << "ResourceMatch: copy";
    return new ResourceMatch(share,*this);
}

void ResourceMatch::constrain(const Gecode::Space& _b)
{
}
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


ResourceMatch* ResourceMatch::solve(const IRIList& query, const IRIList& resourcePool, OWLOntology::Ptr ontology)
{
    // Setup initial solution / problem
    ResourceMatch* match = new ResourceMatch(query, resourcePool, ontology);
    ResourceMatch* solution = match->solve();
    delete match;

    return solution;
}

TypeInstanceMap ResourceMatch::convert(const std::vector<owlapi::model::OWLCardinalityRestriction::Ptr>& restrictions)
{
    TypeInstanceMap typeInstanceMap;
    int instanceId = 0;
    // We assume a compact representation of the query restrictions
    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = restrictions.begin();
    for(; cit != restrictions.end(); ++cit)
    {
        std::vector<int> instances;
        OWLCardinalityRestriction::Ptr restriction = *cit;
        uint32_t cardinality = restriction->getCardinality();
        OWLQualification qualification = restriction->getQualification();

        LOG_WARN_S << "Instance id: " << instanceId;
        for(uint32_t i = 0; i < cardinality; ++i)
        {
            instances.push_back(instanceId++);
        }
        typeInstanceMap[qualification] = instances;
    }
    return typeInstanceMap;
}

AllowedTypesMap ResourceMatch::getAllowedTypes(const TypeInstanceMap& query, const TypeInstanceMap& pool, OWLOntology::Ptr ontology)
{
    OWLOntologyAsk ask(ontology);

    AllowedTypesMap allowedTypesMap;
    TypeInstanceMap::const_iterator cit = query.begin();
    for(; cit != query.end(); ++cit)
    {
        std::vector<IRI> allowedTypes;
        TypeInstanceMap::const_iterator rit = pool.begin();
        for(; rit != pool.end(); ++rit)
        {
            if(cit->first == rit->first || ask.isSubclassOf(rit->first, cit->first))
            {
                allowedTypes.push_back(rit->first);
            }
        }
        allowedTypesMap[cit->first] = allowedTypes;
    }
    return allowedTypesMap;
}

std::vector<int> ResourceMatch::getAllowedDomain(const owlapi::model::IRI& item, const AllowedTypesMap& allowedTypes, const TypeInstanceMap& typeInstanceMap )
{
    std::vector<int> domain;
    AllowedTypesMap::const_iterator cit = allowedTypes.find(item);
    const std::vector<IRI>& types = cit->second;

    std::vector<IRI>::const_iterator tit = types.begin();
    for(; tit != types.end(); ++tit)
    {
        TypeInstanceMap::const_iterator iit = typeInstanceMap.find(*tit);
        const std::vector<int>& instances = iit->second;
        for(int i = 0; i < instances.size(); ++i)
        {
            LOG_WARN_S << "Instance: " << instances.at(i);
        }
        domain.insert(domain.end(), instances.begin(), instances.end());
    }
    return domain;
}

uint32_t ResourceMatch::getInstanceCount(const TypeInstanceMap& map)
{
    uint32_t count = 0;
    TypeInstanceMap::const_iterator cit = map.begin();
    for(; cit != map.end(); ++cit)
    {
        count += cit->second.size();
    }
    return count;
}

ResourceMatch* ResourceMatch::solve(const std::vector<OWLCardinalityRestriction::Ptr>& queryRestrictions, const std::vector<OWLCardinalityRestriction::Ptr>& resourcePoolRestrictions, OWLOntology::Ptr ontology)
{
    ResourceMatch* match = new ResourceMatch(queryRestrictions, resourcePoolRestrictions, ontology);
    ResourceMatch* solution = match->solve();
    delete match;

    return solution;

}

ResourceMatch::ResourceMatch(const std::vector<OWLCardinalityRestriction::Ptr>& queryRestrictions, const std::vector<OWLCardinalityRestriction::Ptr>& resourcePoolRestrictions, OWLOntology::Ptr ontology)
{
    TypeInstanceMap query = ResourceMatch::convert(queryRestrictions);
    TypeInstanceMap resourcePool = ResourceMatch::convert(resourcePoolRestrictions);

    LOG_WARN_S << "Number of query instances: " << getInstanceCount(query);
    LOG_WARN_S << "Domain ranges from: 0 to " << getInstanceCount(resourcePool) -1;
    Gecode::IntVarArray assignment(*this, getInstanceCount(query), 0, getInstanceCount(resourcePool) -1);

    // For all resource types in the pool, find all resources that are allowed
    // to fulfill the query
    AllowedTypesMap allowedTypesMap = ResourceMatch::getAllowedTypes(query, resourcePool, ontology);

    // 2. each query item can only be fulfilled by the same class or subclass
    // 3. query items remain distinct over the complete set

    // Compute the set of allowed types for this slot
    uint32_t index = 0;
    uint32_t assignmentIndex = 0;
    std::vector<OWLCardinalityRestriction::Ptr>::const_iterator cit = queryRestrictions.begin();
    for(; cit != queryRestrictions.end(); ++cit)
    {
        OWLCardinalityRestriction::Ptr restriction = *cit;

        std::vector<int> domain = getAllowedDomain(restriction->getQualification(), allowedTypesMap, resourcePool);
        // initialize with an array int[] and given size n, does not really work
        // as expected, create only a range [0,domain.size()] -- Gecode::IntSet set(domain[0], domain.size());
        Gecode::IntArgs args;
        std::vector<int>::const_iterator dit = domain.begin();
        for(; dit != domain.end(); ++dit)
        {
            args << *dit;
        }
        Gecode::IntSet set(args);
        LOG_WARN_S << "Set is " << set << " max: " << set.max();

        Gecode::IntVar var(*this, set);
        // we need to assign the equality all at once
        //rel(*this, assignment[0], Gecode::IRT_LQ, 1);
        //rel(*this, assignment[1], Gecode::IRT_EQ, 0);
        //rel(*this, assignment[1], Gecode::IRT_NQ, 0);
        for(int i = 0; i < restriction->getCardinality(); ++i)
        {
            LOG_WARN_S << "index: " << assignmentIndex << " -- " << assignment[assignmentIndex] << " maps to " << var;
            //dom(*this, assignment[assignmentIndex++], var);
            dom(*this, assignment[assignmentIndex++], var);
    // no limitation is as if no constraint has been defined at all
    //        dom(*this, assignment[0], 1);
        }
    }
    branch(*this, assignment, Gecode::INT_VAR_NONE(), Gecode::INT_VAL_MIN());

    //    // In order to define the domain for a query element
    //    std::vector<int> allowed;
    //    const std::vector<IRI>& allowedTypes = allowedTypesMap[restriction->getQualification()];
    //    std::vector<IRI>::const_iterator ait = allowedTypes.begin();


    //    // Instances that will be allowed for this requirement
    //    Gecode::IntVarArgs instances;
    //    std::vector<IRI>::const_iterator ait = allowedTypes.begin();
    //    for(; ait != allowedTypes.end(); ++ait)
    //    {
    //    
    //        IRI type = *ait;
    //        std::vector<int> allowedInstances = resourcePool[type];

    //        std::vector<int>::const_iterator aiit = allowedInstances.begin();
    //        for(; aiit != allowedInstances.end(); ++aiit)
    //        {
    //            allowed.push_back(*aiit);
    //            instances << Gecode::IntVar(*this, *aiit,*aiit);
    //        }
    //    }

    //    // Collection from which type can be assigned
    //    int* intSet = &allowed[0];
    //    Gecode::IntArgs instanceSelection(allowed);

    //    // 1. the cardinality restrictions do apply
    //    uint32_t cardinality = restriction->getCardinality();
    //    for(uint32_t i = 0; i < cardinality; ++i)
    //    {
    //        std::vector<int>::const_iterator aiit = allowed.begin();
    //        for(; aiit != allowed.end(); ++aiit)
    //        {
    //            rel(*this, assignment[index++], Gecode::IRT_EQ, *aiitd);
    //        }
    //    }

    //    switch(restriction->getCardinalityRestrictionType())
    //    {
    //        case OWLCardinalityRestriction::MIN:
    //            count(*this, assignment, Gecode::IntSet(cardinality, Gecode::Int::Limits::max), instanceSelection);
    //            break;
    //        case OWLCardinalityRestriction::MAX:
    //            count(*this, assignment, Gecode::IntSet(0, cardinality), instanceSelection);
    //            break;
    //        case OWLCardinalityRestriction::EXACT:
    //            LOG_WARN_S << "Cardinality: " << cardinality << " for " << instanceSelection;
    //            count(*this, assignment, Gecode::IntSet(cardinality, cardinality), instanceSelection);
    //            break;
    //        default:
    //            throw std::runtime_error("ResourceMatch: internal error -- unknown OWLCardinalityRestrictionType provided");
    //    }
    //}

    // Mhm, only if solution is specific
    count(*this, assignment, 1, Gecode::IRT_EQ, 1);
    //count(*this, assignment, 0, Gecode::IRT_EQ, 1);
    //count(*this, assignment, Gecode::IntSet(1,1), Gecode::IRT_LQ, 1);
    //count(*this, assignment, Gecode::IntSet(0,1), Gecode::IntArgs() << 0 << 1);
    //count(*this, assignment, Gecode::IntSet(0,10), Gecode::IntArgs() << 1);
    //distinct(*this, assignment, Gecode::ICL_DOM);
    mAssignment.update(*this, false, assignment); 

    Gecode::Gist::Print<ResourceMatch> p("Print solution");
    Gecode::Gist::Options o;
    o.inspect.click(&p);
    Gecode::Gist::dfs(this, o);

}
void ResourceMatch::print(std::ostream& os) const
{
    os << this->toString() << std::endl;
}

ResourceMatch* ResourceMatch::solve()
{
    // Setup Branch and bound search
    // alternative is depth first search
    Gecode::BAB<ResourceMatch> searchEngine(this);
    //Gecode::DFS<ResourceMatch> searchEngine(this);

    // Start search
    ResourceMatch* best = NULL;
    while(ResourceMatch* matching = searchEngine.next())
    {
        LOG_WARN_S << "RUN SOLVER";
        delete best;
        // Save current solution as best
        best = matching;
    }
    LOG_WARN_S << "NO SOLUTION";

    if(best == NULL)
    {
        throw std::runtime_error("owlapi::csp::ResourceMatch: no solution found");
    }
    best->status();
    return best;
}

std::string ResourceMatch::toString() const 
{
    std::stringstream ss;
    ss << "ResourceMatch: { " << std::endl;
    for(int i = 0; i < mAssignment.size(); i++)
    {
        //ss << "    " << mQuery[i] << " := ";
        //if(mAssignment[i].assigned())
        //{
        //    ss << mResourcePool[mAssignment[i].val()];
        //} else
        //{
        //    ss << "position range: " << mAssignment[i];
        //}
        ss << "    " << i << " --> " << mAssignment[i];
        ss << ", "<< std::endl;
    }
    ss << "}" << std::endl;
    return ss.str();
}

IRI ResourceMatch::getAssignment(const IRI& iri) const
{
    IRIList::const_iterator cit = std::find(mQuery.begin(), mQuery.end(), iri);
    if(cit != mQuery.end())
    {
        int index = cit - mQuery.begin();
        return mResourcePool[ mAssignment[index].val() ];
    }
    throw std::runtime_error("ResourceMatch::getAssignment: there exists no assignment for '" + iri.toString() + "'");
}


} // end namespace cps
} // end namespace owlapi
