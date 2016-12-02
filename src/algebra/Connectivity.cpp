#include "Connectivity.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <numeric/Combinatorics.hpp>
#include <gecode/minimodel.hh>

using namespace owlapi::model;

namespace organization_model {
namespace algebra {

Connectivity::Connectivity(const ModelPool& modelPool, const OrganizationModelAsk& ask, const owlapi::model::IRI& interfaceBaseClass)
    : mModelPool(modelPool)
    , mAsk(ask.ontology())
    , mInterfaceBaseClass(interfaceBaseClass)
    , mModelCombination(mModelPool.toModelCombination())
{
    assert(!mModelCombination.empty());
    // Identify interfaces -- we assume here ElectoMechanicalInterface
    IRIList::const_iterator mit = mModelCombination.begin();
    for(; mit != mModelCombination.end(); ++mit)
    {
        const IRI& model = *mit;
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = mAsk.getCardinalityRestrictions(model, mInterfaceBaseClass);

        owlapi::model::IRIList interfaces;
        std::vector<OWLCardinalityRestriction::Ptr>::const_iterator rit = restrictions.begin();
        for(; rit != restrictions.end(); ++rit)
        {
            const OWLCardinalityRestriction::Ptr& restriction = *rit;
            if( restriction->getCardinalityRestrictionType() == OWLCardinalityRestriction::MAX)
            {
                for(size_t i = 0; i < restriction->getCardinality(); ++i)
                {
                    interfaces.push_back(restriction->getQualification());
                }
            } else {
                LOG_WARN_S << "Found a minimum cardinality restrictions for a resource -- was expecting a max cardinality constraint";
            }
        }
        assert(false);

        if(interfaces.empty())
        {
            throw std::invalid_argument("organization_model::algebra::Connecticity:"
                    " cannot construct problem since model '" + model.toString() + "' does not have any associated interfaces of type '" + interfaceBaseClass.toString() );
        }

        std::pair<IRI, IRIList> interfacesOfModel(model, interfaces);
        uint32_t startRange = mInterfaces.size();
        mInterfaces.insert(mInterfaces.end(), interfaces.begin(), interfaces.end());
        uint32_t endRange = mInterfaces.size() - 1;

        // for each model register start + end index so that we can define the
        // constraints between different systems more easily
        mInterfaceIndexRanges.push_back( IndexRange(startRange, endRange) );
        mInterfaceMapping.push_back(interfacesOfModel);
    }

    Gecode::IntVarArray connections(*this, mInterfaces.size()*mInterfaces.size(),0,1);
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections, mInterfaces.size(), mInterfaces.size());

    //          a0-0 a0-1 a0-2 a0-3 a1-0   a1-1   a1-2
    //  a0-0     [0]  [0]  [0]  [0] [0,1]  [0,1]  [0,1]
    //  a0-1     [0]  [0]
    //  a0-2     [0]       [0]
    //  a0-3                    [0]
    //  a1-0                        [0]
    //  a1-1                             [0]
    //  a1-2                                   [0]

    /// Symmetry of matrix -- since if interface A is compatible to B, so is B
    /// compatible to A
    Gecode::IntVarArgs exactNumberOfConnections;
    for(uint32_t s = 0; s < mInterfaces.size(); ++s)
    {
        for(uint32_t t = s; t < mInterfaces.size(); ++t)
        {
            Gecode::IntVar v = connectionMatrix(s,t);
            Gecode::IntVar vSym = connectionMatrix(t,s);

            rel(*this, v == vSym);
            if(t == s)
            {
                rel(*this, v, Gecode::IRT_EQ, 0);
            }

            exactNumberOfConnections << v;
        }
    }

    rel(*this, sum(exactNumberOfConnections) == mInterfaceIndexRanges.size() - 1);

    // for all interfaces check compatibility and set domain (0) or (0,1)
    // accordingly 1 means connection (can be) established
    // Agent A
    for(size_t a0 = 0; a0 < mInterfaceIndexRanges.size(); ++a0)
    {
        IndexRange a0InterfaceIndexes = mInterfaceIndexRanges[a0];

        // Agent B
        for(size_t a1 = a0; a1 < mInterfaceIndexRanges.size(); ++a1)
        {
            IndexRange a1InterfaceIndexes = mInterfaceIndexRanges[a1];

            Gecode::IntVarArgs interfaceUsage;

            // Interfaces of Agent A
            for(size_t i0 = a0InterfaceIndexes.first; i0 <= a0InterfaceIndexes.second; ++i0)
            {
                const IRI& interfaceModel0 = mInterfaces[i0];

                // Interfaces of Agent B
                for(size_t i1 = a1InterfaceIndexes.first; i1 <= a1InterfaceIndexes.second; ++i1)
                {
                    const IRI& interfaceModel1 = mInterfaces[i1];

                    Gecode::IntVar v = connectionMatrix(i0,i1);
                    interfaceUsage << v;

                    if(a1 == a0) // same agent
                    {
                        LOG_DEBUG_S << "Same agents that shall be connected";
                        // no connection possible within the same agent
                        rel(*this, v, Gecode::IRT_EQ, 0);
                    } else if( mAsk.isRelatedTo(interfaceModel0, vocabulary::OM::compatibleWith(), interfaceModel1))
                    {
                        LOG_DEBUG_S << interfaceModel0.toString() << " isCompatibleWith " << interfaceModel1.toString();
                        rel(*this, v, Gecode::IRT_LQ,1);
                    } else {
                        // no connection possible between these two models
                        LOG_DEBUG_S << interfaceModel0.toString() << " isNotCompatibleWith " << interfaceModel1.toString();
                        rel(*this, v, Gecode::IRT_EQ, 0);
                    }

                }
            }

            // There should be maximum one connection between two systems
            rel(*this, sum( interfaceUsage ) <= 1);
        }

    }
    mConnections = connections;

    LOG_INFO_S << "Connectivity: after initial propagation" << toString();

    branch(*this, mConnections, Gecode::INT_VAR_SIZE_MAX(), Gecode::INT_VAL_SPLIT_MIN());
    branch(*this, mConnections, Gecode::INT_VAR_MIN_MIN(), Gecode::INT_VAL_SPLIT_MIN());
    branch(*this, mConnections, Gecode::INT_VAR_NONE(), Gecode::INT_VAL_SPLIT_MIN());
}

Connectivity::Connectivity(bool share, Connectivity& other)
    : Gecode::Space(share, other)
    , mModelPool(other.mModelPool)
    , mAsk(other.mAsk)
    , mModelCombination(other.mModelCombination)
    , mInterfaces(other.mInterfaces)
    , mInterfaceMapping(other.mInterfaceMapping)
{
    mConnections.update(*this, share, other.mConnections);
}

Gecode::Space* Connectivity::copy(bool share)
{
    return new Connectivity(share, *this);
}

bool Connectivity::isFeasible(const ModelPool& modelPool, const OrganizationModelAsk& ask)
{
    // For a single system this check is trivially true
    if(modelPool.numberOfInstances() < 2)
    {
        return true;
    }
    try {
        Connectivity* connectivity = new Connectivity(modelPool, ask);
        Gecode::BAB<Connectivity> searchEngine(connectivity);

        Connectivity* current = searchEngine.next();
        if(current)
        {
            LOG_DEBUG_S << "Connection is feasible: found solution " << current->toString();

            delete current;
            delete connectivity;
            return true;
        }
    } catch(const std::invalid_argument& e)
    {
        // When there is no connection interface then the construction of
        // connectivity fails, thus a connection is not feasible
        LOG_WARN_S << e.what();
    }
    LOG_DEBUG_S << "Connection is not feasible";
    return false;
}

std::string Connectivity::toString() const
{
    std::stringstream ss;
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections, mInterfaces.size(), mInterfaces.size());
    for(uint32_t r = 0; r < mInterfaces.size(); ++r)
    {
        ss << std::endl << "interface #" << r << " " ;
        for(uint32_t c = 0; c < mInterfaces.size(); ++c)
        {
            ss << "(" << r << "/" << c << ")=" << connectionMatrix(r,c) << " ";
        }
        ss << std::endl;
    }
    return ss.str();
}

} // end namespace algebra
} // end namespace organization_model
