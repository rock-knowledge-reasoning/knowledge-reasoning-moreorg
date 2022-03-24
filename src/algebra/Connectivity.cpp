#include "Connectivity.hpp"
#include <base/Time.hpp>
#include <gecode/int.hh>
#include <gecode/minimodel.hh>
#include <gecode/search.hh>
#include <graph_analysis/GraphIO.hpp>
#include <iostream>
#include <numeric/Combinatorics.hpp>

#include "../utils/GecodeUtils.hpp"
#include "../vocabularies/OM.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace algebra {

QueryCache Connectivity::msQueryCache;

Connectivity::Statistics Connectivity::msStatistics;
graph_analysis::BaseGraph::Ptr Connectivity::msConnectionGraph;
qxcfg::Configuration Connectivity::msConfiguration;

Connectivity::Statistics::Statistics()
    : evaluations(0)
{
}

std::string Connectivity::Statistics::toString(size_t indent) const
{
    std::stringstream ss;
    std::string hspace(indent, ' ');
    ss << hspace << "Statistics:" << std::endl;
    ss << hspace << "    # graph completeness evaluations: " << evaluations
       << std::endl;
    ss << hspace << "    time in s: " << timeInS << std::endl;
    ss << hspace << "    stopped: " << stopped << std::endl;
    ss << hspace << "    # propagator executions: " << csp.propagate
       << std::endl;
    ss << hspace << "    # failed nodes: " << csp.fail << std::endl;
    ss << hspace << "    # expanded nodes: " << csp.node << std::endl;
    ss << hspace << "    depth of search stack: " << csp.depth << std::endl;
    ss << hspace << "    # restarts: " << csp.restart << std::endl;
    ss << hspace << "    # nogoods: " << csp.nogood << std::endl;
    return ss.str();
}

std::string Connectivity::Statistics::getStatsDescription()
{
    return "[graph completeness eval][stdev][time in "
           "s][stdev][stopped][stdev][# propagator executions][stdev][# failed "
           "nodes][stdev][# expanded nodes][stdev][# depth of search "
           "stack][stdev][# restarts][stdev][# nogoods][stdev]";
}

std::vector<numeric::Stats<double>> Connectivity::Statistics::compute(
    const std::vector<Connectivity::Statistics>& statistics)
{
    std::vector<numeric::Stats<double>> stats(9);

    for(const Connectivity::Statistics& s : statistics)
    {
        size_t i = 0;
        stats[i++].update(s.evaluations);
        stats[i++].update(s.timeInS);
        stats[i++].update(s.stopped);
        stats[i++].update(s.csp.propagate);
        stats[i++].update(s.csp.fail);
        stats[i++].update(s.csp.node);
        stats[i++].update(s.csp.depth);
        stats[i++].update(s.csp.depth);
        stats[i++].update(s.csp.nogood);
    }
    return stats;
}

std::string Connectivity::Statistics::toString(
    const std::vector<Connectivity::Statistics>& statistics)
{
    std::stringstream ss;
    ss << "[graph completeness eval][time in s][stopped][# propagator "
          "executions][# failed nodes][# expanded nodes][# depth of search "
          "stack][# restarts][# nogoods]"
       << std::endl;
    for(const Connectivity::Statistics& s : statistics)
    {
        ss << s.evaluations << " ";
        ss << s.timeInS << " ";
        ss << s.stopped << " ";
        ss << s.csp.propagate << " ";
        ss << s.csp.fail << " ";
        ss << s.csp.node << " ";
        ss << s.csp.depth << " ";
        ss << s.csp.restart << " ";
        ss << s.csp.nogood << " ";
        ss << std::endl;
    }

    ss << getStatsDescription() << std::endl;
    std::vector<numeric::Stats<double>> stats = compute(statistics);
    for(size_t i = 0; i < stats.size(); ++i)
    {
        ss << stats[i].mean() << " " << stats[i].stdev() << " ";
    }
    ss << std::endl;
    return ss.str();
}

Connectivity::Connectivity(const ModelPool& modelPool,
                           const OrganizationModelAsk& ask,
                           const owlapi::model::IRI& interfaceBaseClass,
                           const owlapi::model::IRI& property)
    : mModelPool(modelPool.compact())
    , mAsk(ask.ontology())
    , mInterfaceBaseClass(interfaceBaseClass)
    , mProperty(property)
    , mModelCombination(mModelPool.toModelCombination())
    , mExistingConnections(*this,
                           mModelCombination.size(),
                           0,
                           mModelCombination.size())
    , mAgentConnections(*this,
                        mModelCombination.size() * mModelCombination.size(),
                        0,
                        mModelCombination.size())
    , mRnd(0)
    , mIsTree(true)
{
    // Using hw() potentially requires entropy generator
    // since Gecode uses /dev/random, which might block
    // whilw time seem to be rather low resolution it seems now the better
    // option
    // mRnd.time();
    mRnd.hw();
    identifyInterfaces();

    Gecode::IntVarArray connections(*this,
                                    mInterfaces.size() * mInterfaces.size(),
                                    0,
                                    1);
    enforceSymmetricMatrix(connections);
    applyCompatibilityConstraints(connections);
    cacheExistingConnections(connections);
    maxOneLink(connections);

    mConnections = connections;

    LOG_INFO_S << "Connectivity: after initial propagation" << toString();

    // Avoid computation of solutions that are redunant
    // Gecode documentation says however in 8.10.2 that "Symmetry breaking by
    // LDSB is not guaranteed to be complete. That is, a search may still return
    // two distinct solutions that are symmetric."
    //
    Gecode::Symmetries symmetries = identifySymmetries(connections);

    std::string valueSelection =
        msConfiguration.getValue("connectivity/branching/value-selection",
                                 "MAX");
    Gecode::IntValBranch::Select valSelect =
        utils::GecodeUtils::getIntValSelect(valueSelection);
    Gecode::IntValBranch* valBranch = 0;
    if(valSelect == Gecode::IntValBranch::SEL_RND)
    {
        valBranch = new Gecode::IntValBranch(mRnd);
    } else
    {
        valBranch = new Gecode::IntValBranch(valSelect);
    }

    std::string variableSelection =
        msConfiguration.getValue("connectivity/branching/variable-selection",
                                 "MERIT_MIN");
    Gecode::IntVarBranch::Select varSelect =
        utils::GecodeUtils::getIntVarSelect(variableSelection);
    Gecode::IntVarBranch* varBranch = 0;

    LOG_INFO_S << "Using selection strategies: " << std::endl
               << "    variable: " << variableSelection << std::endl
               << "    value: " << valueSelection << std::endl;

    switch(varSelect)
    {
        // Ideally prefer the assignment of feasible 'connections' for a system,
        // which is the MAX of the domain -> 1
        // Propagation will set the other invalid connections to 0, thus
        // speeding up the assignment process
        case Gecode::IntVarBranch::SEL_MERIT_MIN:
        case Gecode::IntVarBranch::SEL_MERIT_MAX:
            varBranch = new Gecode::IntVarBranch(varSelect, &merit, nullptr);
            break;
        case Gecode::IntVarBranch::SEL_RND:
            varBranch = new Gecode::IntVarBranch(mRnd);
            break;
        case Gecode::IntVarBranch::SEL_MIN_MIN:
        case Gecode::IntVarBranch::SEL_MIN_MAX:
        case Gecode::IntVarBranch::SEL_MAX_MIN:
        case Gecode::IntVarBranch::SEL_MAX_MAX:
        case Gecode::IntVarBranch::SEL_DEGREE_MIN:
        case Gecode::IntVarBranch::SEL_DEGREE_MAX:
            varBranch = new Gecode::IntVarBranch(varSelect, nullptr);
            break;
        default:
            throw std::runtime_error(
                "moreorg::algebra::Connectivity: selected value selection"
                " strategy is not supported: '" +
                variableSelection + "'");
    }

    branch(*this, mConnections, *varBranch, *valBranch, symmetries);
    // branch(*this, mConnections, Gecode::INT_VAR_RND(mRnd),
    // Gecode::INT_VAL_MAX());

    delete valBranch;
    delete varBranch;
}

Connectivity::Connectivity(Connectivity& other)
    : Gecode::Space(other)
    , mModelPool(other.mModelPool)
    , mAsk(other.mAsk)
    , mModelCombination(other.mModelCombination)
    , mInterfaces(other.mInterfaces)
    , mInterfaceMapping(other.mInterfaceMapping)
    , mInterfaceIndexRanges(other.mInterfaceIndexRanges)
    , mIdx2Agents(other.mIdx2Agents)
    , mRnd(other.mRnd)
    , mIsTree(other.mIsTree)
{
    mConnections.update(*this, other.mConnections);
    mAgentConnections.update(*this, other.mAgentConnections);
    mExistingConnections.update(*this, other.mExistingConnections);
}

void Connectivity::identifyInterfaces()
{
    assert(!mModelCombination.empty());
    // Identify interfaces -- we assume here ElectroMechanicalInterface
    IRIList::const_iterator mit = mModelCombination.begin();
    for(; mit != mModelCombination.end(); ++mit)
    {
        const IRI& model = *mit;
        std::vector<OWLCardinalityRestriction::Ptr> restrictions =
            mAsk.getCardinalityRestrictions(model,
                                            mProperty,
                                            mInterfaceBaseClass);

        owlapi::model::IRIList interfaces;
        for(const OWLCardinalityRestriction::Ptr& r : restrictions)
        {
            const OWLObjectCardinalityRestriction::Ptr& restriction =
                dynamic_pointer_cast<OWLObjectCardinalityRestriction>(r);
            if(!restriction)
            {
                throw std::runtime_error(
                    "moreorg::algebra::Connectivity::identifyInterfaces:"
                    " expected OWLObjectCardinalityRestriction");
            }

            if(restriction->getCardinalityRestrictionType() ==
               OWLCardinalityRestriction::MAX)
            {
                for(size_t i = 0; i < restriction->getCardinality(); ++i)
                {
                    interfaces.push_back(restriction->getQualification());
                }
            } else
            {
                LOG_INFO_S << "Found a minimum cardinality restriction "
                           << restriction->getQualification() << " on model "
                           << model
                           << " -- was expecting a max cardinality constraint";
            }
        }

        if(interfaces.empty())
        {
            throw NoConnectionInterfaces(
                "moreorg::algebra::Connecticity:"
                " cannot construct problem since model '" +
                model.toString() +
                "' does not have any associated interfaces of type '" +
                mInterfaceBaseClass.toString());
        }

        std::pair<IRI, IRIList> interfacesOfModel(model, interfaces);
        uint32_t startRange = mInterfaces.size();
        mInterfaces.insert(mInterfaces.end(),
                           interfaces.begin(),
                           interfaces.end());
        uint32_t endRange = mInterfaces.size() - 1;

        // for each model register start + end index so that we can define the
        // constraints between different systems more easily
        mInterfaceIndexRanges.push_back(IndexRange(startRange, endRange));
        mInterfaceMapping.push_back(interfacesOfModel);
    }

    for(size_t idx = 0; idx < mInterfaces.size() * mInterfaces.size(); ++idx)
    {
        size_t a1Idx = idx % mInterfaces.size();
        size_t a0Idx = (idx - a1Idx) / mInterfaces.size();

        IndexRange idxRange0;
        IndexRange idxRange1;

        for(IndexRange range : mInterfaceIndexRanges)
        {
            if(a0Idx >= range.first && a0Idx <= range.second)
            {
                idxRange0 = range;
            }
            if(a1Idx >= range.first && a1Idx <= range.second)
            {
                idxRange1 = range;
            }
        }

        size_t agent0 = std::distance(mInterfaceIndexRanges.begin(),
                                      std::find(mInterfaceIndexRanges.begin(),
                                                mInterfaceIndexRanges.end(),
                                                idxRange0));
        size_t agent1 = std::distance(mInterfaceIndexRanges.begin(),
                                      std::find(mInterfaceIndexRanges.begin(),
                                                mInterfaceIndexRanges.end(),
                                                idxRange1));

        mIdx2Agents.push_back(std::pair<size_t, size_t>(agent0, agent1));
    }
}

void Connectivity::enforceSymmetricMatrix(Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());
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
            Gecode::IntVar v = connectionMatrix(s, t);
            Gecode::IntVar vSym = connectionMatrix(t, s);

            rel(*this, v == vSym);
            if(t == s)
            {
                rel(*this, v, Gecode::IRT_EQ, 0);
            }

            exactNumberOfConnections << v;
        }
    }

    Gecode::IntVar linkCount(*this,
                             mInterfaceIndexRanges.size() - 1,
                             mInterfaceIndexRanges.size());
    if(mIsTree)
    {
        rel(*this, sum(exactNumberOfConnections) == linkCount);
    } else
    {
        rel(*this, sum(exactNumberOfConnections) >= linkCount);
    }
}

void Connectivity::applyCompatibilityConstraints(
    Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());

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
            for(size_t i0 = a0InterfaceIndexes.first;
                i0 <= a0InterfaceIndexes.second;
                ++i0)
            {
                const IRI& interfaceModel0 = mInterfaces[i0];

                // Interfaces of Agent B
                for(size_t i1 = a1InterfaceIndexes.first;
                    i1 <= a1InterfaceIndexes.second;
                    ++i1)
                {
                    const IRI& interfaceModel1 = mInterfaces[i1];

                    Gecode::IntVar v = connectionMatrix(i0, i1);
                    agentInterconnection << v;

                    if(a1 == a0) // same agent
                    {
                        LOG_DEBUG_S << "Same agents that shall be connected";
                        // no connection possible within the same agent
                        rel(*this, v, Gecode::IRT_EQ, 0);
                    } else
                    {
                        bool hasRelation = false;
                        try
                        {
                            hasRelation = mAsk.isRelatedTo(
                                interfaceModel0,
                                vocabulary::OM::compatibleWith(),
                                interfaceModel1);
                        } catch(const std::invalid_argument& e)
                        {
                            LOG_INFO_S << "No relation found between "
                                       << interfaceModel0 << " and "
                                       << interfaceModel1 << " -- " << e.what();
                            // seems there is not even an individual for this
                            // interface type
                        }

                        if(hasRelation)
                        {
                            LOG_DEBUG_S << interfaceModel0.toString()
                                        << " isCompatibleWith "
                                        << interfaceModel1.toString();
                            rel(*this, v, Gecode::IRT_LQ, 1);
                        } else
                        {
                            // no connection possible between these two models
                            LOG_DEBUG_S << interfaceModel0.toString()
                                        << " isNotCompatibleWith "
                                        << interfaceModel1.toString();
                            rel(*this, v, Gecode::IRT_EQ, 0);
                        }
                    }
                }
            }
            // There should be maximum one connection between two systems
            rel(*this, sum(agentInterconnection) <= 1);
            Gecode::Matrix<Gecode::IntVarArray> agentConnectionMatrix(
                mAgentConnections,
                mModelCombination.size(),
                mModelCombination.size());
            rel(*this,
                agentConnectionMatrix(a0, a1) == sum(agentInterconnection));
        }
    }
}

void Connectivity::cacheExistingConnections(Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());

    // This constraint implicitly holds through the other constraints
    // of # of overall links, agent interconnection and max one connection per
    // interface
    for(size_t a = 0; a < mInterfaceIndexRanges.size(); ++a)
    {
        IndexRange aInterfaceIndexes = mInterfaceIndexRanges[a];
        Gecode::IntVarArgs agentConnections;
        // Interfaces of Agent A
        for(size_t i = aInterfaceIndexes.first; i <= aInterfaceIndexes.second;
            ++i)
        {
            agentConnections << connectionMatrix.col(i);
        }

        // there should be at least one outgoing connection to another system
        // rel(*this, sum(agentConnections) >= 1);
        rel(*this, mExistingConnections[a] == sum(agentConnections));
    }
}

void Connectivity::maxOneLink(Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());

    // Maximum of one connection per interface
    for(size_t c = 0; c < mInterfaces.size(); ++c)
    {
        Gecode::IntVarArgs interfaceUsage;

        for(size_t r = 0; r < mInterfaces.size(); ++r)
        {
            Gecode::IntVar v = connectionMatrix(c, r);
            interfaceUsage << v;
        }

        // There should be maximum one connection per interface
        rel(*this, sum(interfaceUsage) <= 1);
    }
}

Gecode::Space* Connectivity::copy() { return new Connectivity(*this); }

bool Connectivity::isComplete() const
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());

    using namespace graph_analysis;
    Vertex::PtrList vertices;
    // Currently testing connectivity is implemented only for lemon
    mpBaseGraph = BaseGraph::getInstance(BaseGraph::LEMON_DIRECTED_GRAPH);
    for(size_t i = 0; i < mInterfaceMapping.size(); ++i)
    {
        Vertex::Ptr v =
            make_shared<Vertex>(mInterfaceMapping[i].first.getFragment());
        mpBaseGraph->addVertex(v);

        vertices.push_back(v);
    }

    for(size_t a0 = 0; a0 < mInterfaceIndexRanges.size() - 1; ++a0)
    {
        IndexRange a0InterfaceIndexes = mInterfaceIndexRanges[a0];

        for(size_t a1 = a0 + 1; a1 < mInterfaceIndexRanges.size(); ++a1)
        {
            IndexRange a1InterfaceIndexes = mInterfaceIndexRanges[a1];

            bool connectionFound = false;
            // check if any of the interface link has a value of one:
            // Interfaces of Agent A
            for(size_t i0 = a0InterfaceIndexes.first;
                i0 <= a0InterfaceIndexes.second;
                ++i0)
            {
                const IRI& interfaceModel0 = mInterfaces[i0];

                // Interfaces of Agent B
                for(size_t i1 = a1InterfaceIndexes.first;
                    i1 <= a1InterfaceIndexes.second;
                    ++i1)
                {
                    const IRI& interfaceModel1 = mInterfaces[i1];

                    Gecode::IntVar v = connectionMatrix(i0, i1);
                    if(!v.assigned())
                    {
                        throw std::runtime_error(
                            "moreorg::algebra::Connectivity::"
                            "checkGraphCompleteness: expected value to be "
                            "assiged");
                    }

                    if(v.val() == 1)
                    {
                        // connection exists between these two systems
                        connectionFound = true;
                        Edge::Ptr e0 =
                            make_shared<Edge>(vertices[a0], vertices[a1]);
                        e0->setLabel(interfaceModel0.getFragment());
                        Edge::Ptr e1 =
                            make_shared<Edge>(vertices[a1], vertices[a0]);
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
    } else
    {
        return true;
    }
}

bool Connectivity::isFeasible(const ModelPool& modelPool,
                              const OrganizationModelAsk& ask,
                              double timeoutInMs,
                              size_t minFeasible,
                              const owlapi::model::IRI& interfaceBaseClass)
{
    msConnectionGraph.reset();
    return Connectivity::isFeasible(modelPool,
                                    ask,
                                    msConnectionGraph,
                                    timeoutInMs,
                                    minFeasible,
                                    interfaceBaseClass);
}

bool Connectivity::isFeasible(const ModelPool& modelPool,
                              const OrganizationModelAsk& ask,
                              graph_analysis::BaseGraph::Ptr& baseGraph,
                              double timeoutInMs,
                              size_t minFeasible,
                              const owlapi::model::IRI& interfaceBaseClass)
{
    FeasibilityQuery query =
        std::make_tuple(modelPool,
                        ask.ontology().getOntology()->getIRI(),
                        interfaceBaseClass,
                        timeoutInMs,
                        minFeasible);

    QueryCache::const_iterator cit = msQueryCache.find(query);
    if(cit != msQueryCache.end())
    {
        std::pair<graph_analysis::BaseGraph::Ptr, bool> cachedResult =
            cit->second;
        baseGraph = cachedResult.first;
        return cachedResult.second;
    }

    // For a single system this check is trivially true
    size_t numberOfInstances = modelPool.numberOfInstances();
    if(numberOfInstances == 0)
    {
        throw std::invalid_argument(
            "moreorg::algebra::Connectivity::isFeasible: "
            " the given model pool has a model count of 0");
    } else if(modelPool.numberOfInstances() == 1)
    {
        LOG_DEBUG_S << "An atomic agent is always feasible";
        return true;
    }

    msStatistics.evaluations = 0;

    Connectivity* last = NULL;
    Connectivity* connectivity = NULL;
    try
    {
        connectivity = new Connectivity(modelPool, ask, interfaceBaseClass);
    } catch(const NoConnectionInterfaces& e)
    {
        LOG_INFO_S << "No connection interfaces of type '" << interfaceBaseClass
                   << "' found on " << modelPool.toString(4);
        return false;
    }

    Gecode::Search::Options options;
    if(timeoutInMs > 0)
    {
        options.stop = Gecode::Search::Stop::time(timeoutInMs);
    }
    options.nogoods_limit = 1024;
    // Gecode::Search::Cutoff * c = Gecode::Search::Cutoff::geometric(10,2);
    Gecode::Search::Cutoff* c = Gecode::Search::Cutoff::constant(10);
    // Gecode::Rnd rnd;
    // rnd.hw();
    // Gecode::Search::Cutoff * c =
    // Gecode::Search::Cutoff::rnd(rnd.seed(),1,connectivity->mInterfaces.size(),2);
    options.cutoff = c;
    Gecode::RBS<Connectivity, Gecode::DFS> searchEngine(connectivity, options);
    // Gecode::BAB<Connectivity> searchEngine(connectivity, options);

    bool isComplete = false;
    size_t feasibleSolutions = 0;
    Connectivity* current = NULL;
    base::Time startTime = base::Time::now();
    try
    {
        while((current = searchEngine.next()))
        {
            ++msStatistics.evaluations;

            isComplete = current->isComplete();
            baseGraph = current->mpBaseGraph->clone();
            delete last;
            last = NULL;

            if(isComplete)
            {
                LOG_DEBUG_S
                    << "Connection is feasible: found solution "
                    << current->toString() << std::endl
                    << "    previously found feasible: " << feasibleSolutions
                    << ", required: " << minFeasible;
                ++feasibleSolutions;
                if(feasibleSolutions >= minFeasible)
                {
                    break;
                }
            }
            last = current;
        }
    } catch(const std::invalid_argument& e)
    {
        // When there is no connection interface then the construction of
        // connectivity fails, thus a connection is not feasible
        LOG_WARN_S << e.what();
    }

    msStatistics.timeInS = (base::Time::now() - startTime).toSeconds();
    msStatistics.stopped = searchEngine.stopped();
    msStatistics.csp = searchEngine.statistics();

    delete last;
    delete current;
    delete connectivity;

    msQueryCache[query] = std::make_pair(baseGraph, isComplete);
    return isComplete;
}

std::string Connectivity::toString() const
{
    std::stringstream ss;
    std::vector<std::pair<size_t, size_t>> links;
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(mConnections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());
    for(uint32_t r = 0; r < mInterfaces.size(); ++r)
    {
        ss << std::endl << "interface #" << r << " ";
        for(uint32_t c = r; c < mInterfaces.size(); ++c)
        {
            Gecode::IntVar var = connectionMatrix(r, c);
            ss << "(" << r << "/" << c << ")=" << var << " ";
            if(var.assigned() && var.val() == 1)
            {
                links.push_back(std::pair<size_t, size_t>(r, c));
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

Gecode::Symmetries
Connectivity::identifySymmetries(Gecode::IntVarArray& connections)
{
    Gecode::Matrix<Gecode::IntVarArray> connectionMatrix(connections,
                                                         mInterfaces.size(),
                                                         mInterfaces.size());

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
            if(mInterfaceMapping[c].first == currentModel)
            {
                IndexRange interfaceIndexes = mInterfaceIndexRanges[c];
                LOG_INFO_S << currentModel.toString()
                           << " adding columns: " << interfaceIndexes.first
                           << " -- " << interfaceIndexes.second;
                for(size_t i = interfaceIndexes.first;
                    i <= interfaceIndexes.second;
                    ++i)
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

double
Connectivity::merit(const Gecode::Space& space, Gecode::IntVar x, int idx)
{
    // prefer the less constrained
    const Connectivity& connectivity = static_cast<const Connectivity&>(space);
    return connectivity.computeMerit(x, idx);
}

double Connectivity::computeMerit(Gecode::IntVar x, int idx) const
{
    std::pair<size_t, size_t> agents = mIdx2Agents[idx];

    // find number of existing connections for both agents using the
    // cached values
    size_t existingConnections0 = mExistingConnections[agents.first].min();
    size_t existingConnections1 = mExistingConnections[agents.second].min();

    IndexRange idxRange0 = mInterfaceIndexRanges[agents.first];
    double merit0 = 0;
    size_t numberOfInterfaces0 = idxRange0.second - idxRange0.first + 1;

    IndexRange idxRange1 = mInterfaceIndexRanges[agents.second];
    double merit1 = 0;
    size_t numberOfInterfaces1 = idxRange1.second - idxRange1.first + 1;

    double bias = 1 / (100.0 + mRnd(1000));

    // existing connections/
    if(existingConnections0 != 0)
    {
        // a0: # of interfaces
        // existingConnections
        merit0 = existingConnections0 / (1.0 * numberOfInterfaces0);
        LOG_DEBUG_S << "Merit: " << merit0 << ": bias: " << bias
                    << " existingConnections:" << existingConnections0 << "/"
                    << numberOfInterfaces0;
    }
    if(existingConnections1 != 0)
    {
        merit1 = existingConnections1 / (1.0 * numberOfInterfaces1);
        LOG_DEBUG_S << "Merit: " << merit1 << ": bias: " << bias
                    << " existingConnections:" << existingConnections1 << "/"
                    << numberOfInterfaces1;
    }

    return std::min(merit0, merit1) + bias;
}

} // end namespace algebra
} // end namespace moreorg
