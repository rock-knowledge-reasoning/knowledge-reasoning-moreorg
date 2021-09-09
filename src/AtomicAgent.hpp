#ifndef ORGANIZATION_MODEL_AGENT_INSTANCE_HPP
#define ORGANIZATION_MODEL_AGENT_INSTANCE_HPP

#include <owlapi/model/IRI.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/set.hpp>
#include "ModelPool.hpp"
#include "facades/Robot.hpp"

namespace moreorg {

class OrganizationModelAsk;

/**
 * \class AtomicAgent
 * \brief An individual unsplittable agent
 */
class AtomicAgent
{
    friend class boost::serialization::access;
    /// type of model this role fulfills
    owlapi::model::IRI mModel;
    /// Id of the role, i.e. unique for all agent of the
    /// same model
    size_t mId;
    /// Name of the instance derived from model IRI and id
    std::string mName;
public:
    typedef std::vector<AtomicAgent> List;
    typedef std::set<AtomicAgent> Set;
    typedef std::map<owlapi::model::IRI, std::set<AtomicAgent> > TypeMap;

    /**
     * Default constructor to allow usage in lists and maps
     */
    AtomicAgent();

    /**
     * Preferred role constructor
     * \param name Name/Id of the role
     * \param model Model identification id, which needs to be unique for all atomic
     * agents of the same model
     */
    AtomicAgent(size_t id, const owlapi::model::IRI& model);

    const owlapi::model::IRI& getModel() const { return mModel; }
    size_t getId() const  { return mId; }
    const std::string& getName() const { return mName; }

    std::string toString() const;

    static std::string toString(const List& agents, size_t indent = 0);
    static std::string toString(const Set& agents, size_t indent = 0);
    static moreorg::ModelPool getModelPool(const List& agents);
    static moreorg::ModelPool getModelPool(const Set& agents);

    /**
     * Get a list of atomic agents from a model pool
     * \return agent list
     */
    static List toList(const ModelPool& modelPool);

    /**
     * Compute the cardinality of an agent model inside a given list of agents
     * \return cardinality
     */
    static size_t getCardinality(const List& agents, const owlapi::model::IRI& agentModel) { return getModelPool(agents)[agentModel]; }

    /**
     * Get the intersection of two atomic agent sets
     * \return The set of agent which are shared between two sets of atomic
     * agents
     */
    static AtomicAgent::List getIntersection(const AtomicAgent::Set& a0, const AtomicAgent::Set& a1);

    /**
     * Get the difference set between to atomic agent sets
     * \return different set
     */
    static AtomicAgent::List getDifference(const AtomicAgent::Set& a0, const AtomicAgent::Set& a1);

    bool operator<(const AtomicAgent& other) const;
    bool operator==(const AtomicAgent& other) const { return mModel == other.mModel && mId == other.mId; }
    bool operator!=(const AtomicAgent& other) const { return ! (*this == other); }

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & mModel;
        ar & mId;
        ar & mName;
    }

    /**
     * Create the robot facade
     * \param organizationAsk oject including ontology to identify type
     * information
     * \return robot facade for this AtomicAgent
     */
    facades::Robot getFacade(const OrganizationModelAsk& ask) const;

    /**
     * Compute the type partitioned map for a set of atomic agents
     * \param atomicAgents set of atomic agents for which a typemap shall be
     * created
     * \return TypeMap for given atomic agent set
     */
    static TypeMap toTypeMap(const AtomicAgent::Set& atomicAgents);

    /**
     * Stringify the type map
     * \param typeMap typeMap to stringify
     * \param indent indentation Number of spaces for indent
     */
    static std::string toString(const TypeMap& typeMap, size_t indent = 0);
};

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_AGENT_INSTANCE_HPP
