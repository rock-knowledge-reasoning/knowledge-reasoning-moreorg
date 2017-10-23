#include "Connectivity.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <numeric/Combinatorics.hpp>
#include <gecode/minimodel.hh>
#include <gecode/int.hh>
#include <graph_analysis/GraphIO.hpp>

using namespace owlapi::model;

namespace organization_model {
namespace algebra {

Connectivity::Connectivity(const ModelPool& modelPool,
        const OrganizationModelAsk& ask,
        const owlapi::model::IRI& interfaceBaseClass
        )
    : mModelPool(modelPool)
    , mAsk(ask.ontology())
    , mInterfaceBaseClass(interfaceBaseClass)
    , mModelCombination(mModelPool.toModelCombination())
{
    assert(!mModelCombination.empty());
    // Identify interfaces -- we assume here ElectroMechanicalInterface
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

            Gecode::IntVarArgs agentInterconnection;

            // Interfaces of Agent A
            for(size_t i0 = a0InterfaceIndexes.first; i0 <= a0InterfaceIndexes.second; ++i0)
            {
                const IRI& interfaceModel0 = mInterfaces[i0];

                // Interfaces of Agent B
                for(size_t i1 = a1InterfaceIndexes.first; i1 <= a1InterfaceIndexes.second; ++i1)
                {
                    const IRI& interfaceModel1 = mInterfaces[i1];

                    Gecode::IntVar v = connectionMatrix(i0,i1);
                    agentInterconnection << v;

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
            rel(*this, sum( agentInterconnection ) <= 1);
        }
    }

    // This constraint implicitly holds through the other constraints
    // of # of overall links, agent interconnection and max one connection per
    // interface
    //for(size_t a = 0; a < mInterfaceIndexRanges.size(); ++a)
    //{
    //    IndexRange aInterfaceIndexes = mInterfaceIndexRanges[a];
    //    Gecode::IntVarArgs agentConnections;
    //    // Interfaces of Agent A
    //    for(size_t i = aInterfaceIndexes.first; i <= aInterfaceIndexes.second; ++i)
    //    {
    //        agentConnections << connectionMatrix.col(i);
    //    }

    //    // there should be at least one outgoing connection to another system
    //    rel(*this, sum(agentConnections) >= 1);
    //}

    // Maximum of one connection per interface
    for(size_t c = 0; c < mInterfaces.size(); ++c)
    {
        Gecode::IntVarArgs interfaceUsage;

        for(size_t r = 0; r < mInterfaces.size(); ++r)
        {
            Gecode::IntVar v = connectionMatrix(c,r);
            interfaceUsage << v;
        }

        // There should be maximum one connection per interface
        rel(*this, sum( interfaceUsage ) <= 1);
    }

    mConnections = connections;

    LOG_INFO_S << "Connectivity: after initial propagation" << toString();

    // Avoid computation of solutions that are redunant
    // Gecode documentation says however in 8.10.2 that "Symmetry breaking by
    // LDSB is not guaranteed to be complete. That is, a search may still return
    // two distinct solutions that are symmetric."
    //
    Gecode::Symmetries symmetries = identifySymmetries(connections);

    // Ideally prefer the assignment of feasible 'connections' for a system,
    // which is the MAX of the domain -> 1
    // Propagation will set the other invalid connections to 0, thus speeding up
    // the assignment process

    //Gecode::Rnd rnd;
    //rnd.time();
    //// Which variable to pick
    //branch(*this, mConnections, Gecode::INT_VAR_RND(rnd), Gecode::INT_VAL_MAX(), symmetries);
    branch(*this, mConnections, Gecode::INT_VAR_MERIT_MIN(&merit), Gecode::INT_VAL_MAX(), symmetries);
}

Connectivity::Connectivity(bool share, Connectivity& other)
    : Gecode::Space(share, other)
    , mModelPool(other.mModelPool)
    , mAsk(other.mAsk)
    , mModelCombination(other.mModelCombination)
    , mInterfaces(other.mInterfaces)
    , mInterfaceMapping(other.mInterfaceMapping)
    , mInterfaceIndexRanges(other.mInterfaceIndexRanges)
{
    mConnections.update(*this, share, other.mConnections);
}

Gecode::Space* Connectivity::copy(bool share)
{
    return new Connectivity(share, *this);
}

bool Connectivity::isComplete() const
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections, mInterfaces.size(), mInterfaces.size());

    using namespace graph_analysis;
    Vertex::PtrList vertices;
    // Currently testing connectivity is implemented only for lemon
    mpBaseGraph = BaseGraph::getInstance(BaseGraph::LEMON_DIRECTED_GRAPH);
    for(size_t i = 0; i < mInterfaceMapping.size(); ++i)
    {
        Vertex::Ptr v(new Vertex(mInterfaceMapping[i].first.getFragment()));
        mpBaseGraph->addVertex(v);

        vertices.push_back(v);
    }

    for(size_t a0 = 0; a0 < mInterfaceIndexRanges.size() - 1 ; ++a0)
    {
        IndexRange a0InterfaceIndexes = mInterfaceIndexRanges[a0];

        for(size_t a1 = a0+1; a1 < mInterfaceIndexRanges.size(); ++a1)
        {
            IndexRange a1InterfaceIndexes = mInterfaceIndexRanges[a1];

            bool connectionFound = false;
            // check if any of the interface link has a value of one:
            // Interfaces of Agent A
            for(size_t i0 = a0InterfaceIndexes.first; i0 <= a0InterfaceIndexes.second; ++i0)
            {
                const IRI& interfaceModel0 = mInterfaces[i0];

                // Interfaces of Agent B
                for(size_t i1 = a1InterfaceIndexes.first; i1 <= a1InterfaceIndexes.second; ++i1)
                {
                    const IRI& interfaceModel1 = mInterfaces[i1];

                    Gecode::IntVar v = connectionMatrix(i0,i1);
                    if(!v.assigned())
                    {
                        throw std::runtime_error("organization_model::algebra::Connectivity::checkGraphCompleteness: expected value to be assiged");
                    }

                    if(v.val() == 1)
                    {
                        // connection exists between these two systems
                        connectionFound = true;
                        Edge::Ptr e0(new Edge(vertices[a0],vertices[a1]));
                        e0->setLabel(interfaceModel0.getFragment());
                        Edge::Ptr e1(new Edge(vertices[a1],vertices[a0]));
                        e1->setLabel(interfaceModel1.getFragment());
                        mpBaseGraph->addEdge(e0);
                        mpBaseGraph->addEdge(e1);
                        break;
                    }
                }

                if(connectionFound)
                {
                    break;
                }
            }
        }
    }

    if(!mpBaseGraph->isConnected())
    {
        return false;
    } else {
        return true;
    }
}

bool Connectivity::isFeasible(const ModelPool& modelPool,
        const OrganizationModelAsk& ask,
        double timeoutInMs)
{
    // For a single system this check is trivially true
    if(modelPool.numberOfInstances() < 2)
    {
        return true;
    }
    try {
        Connectivity* connectivity = new Connectivity(modelPool, ask);
        Gecode::Search::Options options;
        if(timeoutInMs > 0)
        {
            options.stop = Gecode::Search::Stop::time(timeoutInMs);
        }
        Gecode::DFS<Connectivity> searchEngine(connectivity, options);

        Connectivity* current = NULL;
        Connectivity* last = NULL;
        while((current = searchEngine.next()))
        {
            if(current->isComplete())
            {
                LOG_DEBUG_S << "Connection is feasible: found solution " << current->toString();
                graph_analysis::io::GraphIO::write("/tmp/organization-model-connectivity-test-success.dot", current->mpBaseGraph);
                delete last;
                delete current;
                delete connectivity;
                return true;
            } else {
                LOG_DEBUG_S << "Connection is not feasible";
                delete last;
                last = current;
            }
        }

        if(last)
        {
            graph_analysis::io::GraphIO::write("/tmp/organization-model-connectivity-test-failure.dot", current->mpBaseGraph);
        }
        delete last;
        delete connectivity;
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
    std::vector< std::pair<size_t, size_t> > links;
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections, mInterfaces.size(), mInterfaces.size());
    for(uint32_t r = 0; r < mInterfaces.size(); ++r)
    {
        ss << std::endl << "interface #" << r << " " ;
        for(uint32_t c = r; c < mInterfaces.size(); ++c)
        {
            Gecode::IntVar var = connectionMatrix(r,c);
            ss << "(" << r << "/" << c << ")=" << var << " ";
            if(var.assigned() && var.val() == 1)
            {
                links.push_back(std::pair<size_t,size_t>(r,c));
            }
        }
        ss << std::endl;
    }
    ss << "Established links (" << links.size() << "):" << std::endl;
    for(uint32_t r = 0; r < links.size(); ++r)
    {
        ss << links[r].first << "/" << links[r].second;
        ss << std::endl;
    }
    return ss.str();
}

Gecode::Symmetries Connectivity::identifySymmetries(Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections, mInterfaces.size(), mInterfaces.size());

    Gecode::Symmetries symmetries;
    // define interchangeable columns for roles of the same model type
    for(const std::pair<IRI, uint32_t>& entry : mModelPool)
    {
        const IRI& currentModel = entry.first;
        Gecode::IntVarArgs sameModel;

        size_t numberOfVariables = 0;
        for(size_t c = 0; c < mInterfaceMapping.size(); ++c)
        {
            // check the model of the interface
            if( mInterfaceMapping[c].first == currentModel)
            {
                IndexRange interfaceIndexes = mInterfaceIndexRanges[c];
                LOG_INFO_S << currentModel.toString() << " adding columns: " << interfaceIndexes.first << " -- " << interfaceIndexes.second;
                for(size_t i = interfaceIndexes.first; i <= interfaceIndexes.second; ++i)
                {
                    // Since a single agents spans multiple columns with its
                    // interface range, we have to dynamically create the length
                    // field
                    numberOfVariables += mInterfaces.size();
                    sameModel << connectionMatrix.col(i);
                }
            }
        }
        symmetries << VariableSequenceSymmetry(sameModel, numberOfVariables);
    }

    Gecode::IntVarArgs rows;
    for(int r = 0; r < connectionMatrix.height(); r++)
    {
        rows << connectionMatrix.row(r);
    }
    symmetries << VariableSequenceSymmetry(rows, connectionMatrix.width());

    Gecode::IntVarArgs cols;
    for(int c = 0; c < connectionMatrix.width(); c++)
    {
        cols << connectionMatrix.col(c);
    }
    symmetries << VariableSequenceSymmetry(cols, connectionMatrix.height());

    return symmetries;
}

double Connectivity::merit(const Gecode::Space& space, Gecode::IntVar x, int idx)
{
    // prefer the highly constrained
    const Connectivity& connectivity = static_cast<const Connectivity&>(space);
    return connectivity.computeMerit(x,idx);
}

double Connectivity::computeMerit(Gecode::IntVar x, int idx) const
{
    size_t a1Idx = idx%mInterfaces.size();
    size_t a0Idx = (idx - a1Idx)/mInterfaces.size();

    IndexRange idxRange0;
    IndexRange idxRange1;

    for(IndexRange range : mInterfaceIndexRanges)
    {
        if(a0Idx >= range.first && a0Idx <= range.second)
        {
            idxRange0 = range;
        }
        if(a1Idx >= range.first && a1Idx <= range.second )
        {
            idxRange1 = range;
        }
    }

    size_t existingConnections = 0;
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections, mInterfaces.size(), mInterfaces.size());
    size_t a0;
    for(a0 = idxRange0.first; a0 <= idxRange0.second; ++a0)
    {
        for(size_t a1 = 0; a1 < mInterfaces.size(); ++a1)
        {
            Gecode::IntVar v = connectionMatrix(a0,a1);
            if( v.assigned())
            {
                if(v.val() == 1)
                {
                    ++existingConnections;
                }
            }
        }
    }

    // a0: # of interfaces
    // existingConnections
    return existingConnections/(1.0*a0);
}

} // end namespace algebra
} // end namespace organization_model
