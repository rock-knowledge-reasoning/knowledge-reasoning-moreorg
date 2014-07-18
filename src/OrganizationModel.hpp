#ifndef OWL_OM_ORGANIZATION_MODEL_HPP
#define OWL_OM_ORGANIZATION_MODEL_HPP

#include <owl_om/Ontology.hpp>

namespace owl_om {

typedef std::vector<IRIList> CandidatesList;

struct InterfaceConnection
{
    InterfaceConnection() {}

    InterfaceConnection(const IRI& interface0, const IRI& interface1)
        : begin(interface0)
        , end(interface1)
    {}

    IRI begin;
    IRI end;

    bool operator<(const InterfaceConnection& other) const;
};

typedef std::vector< InterfaceConnection > InterfaceConnectionList;
typedef std::vector< InterfaceConnectionList > InterfaceCombinationList;

class OrganizationModel
{
    Ontology::Ptr mpOntology;

public:

    /**
     * Constructor for an empty OrganizationModel
     */
    OrganizationModel();

    /**
     * Constructor to create an OrganizationModel from an existing description file
     * \param filename File name to an rdf/xml formatted ontology description file
     * \param runInference Set to true to run inference for recombinations after loading the description file, False to suppress the inference at construction time
     */
    OrganizationModel(const std::string& filename, bool runInference = true);

    Ontology::Ptr ontology() { return mpOntology; }

    const Ontology::Ptr ontology() const { return mpOntology; }

    /**
     * Creation of an instance of a given elementary klass, which is modeled by a given model klass
     */
    void createInstance(const IRI& instanceName, const IRI& klass, const IRI& model);

    // PREDICATES
    /**
     * Test if a resource requirement is fullfilled
     *
     * used properties: 'has','provides', and 'dependsOn'
     */
    bool checkIfFulfills(const IRI& resourceProvider, const IRI& resourceRequirement);

    /**
     * Test if a set of interfaces is compatible (in general)
     *
     * used relations: <Actor> 'has' <Interface>
     * test if the intersection of both interface classes are subclassing 'Compatibility'
     */
    bool checkIfCompatible(const IRI& instance, const IRI& otherInstance);

    /**
     * Account of interface that are in use, i.e.
     * relation: <Actor> 'uses' <Interface>
     */
    CandidatesList checkIfCompatibleNow(const IRI& instance, const IRI& otherInstance);

    /**
     * Run inference to identify service that are 'provided'
     */
    void runInferenceEngine();

    /** Provide a list of available actors, so that we can bound the actual generation of theoretically
     * possible combinations
     *
     * \return list of available actors
     */
    IRIList computeActorsFromRecombination();

    /**
     * \param actor Base actor which should be recombined with another set of actors
     * \param otherActors Set of actors using for recombination
     * \param interfaceBlackList Set of interfaces that are already in use
     * \return List of new actors that arose from recombination
     */
    IRIList recombine(const IRI& actor, const IRIList& otherActors);

    /**
     * Reduce list of actor to unique individuals, i.e. removing
     * alias and same individuals
     */
    IRIList compactActorList();

    IRI getResourceModel(const IRI& instance);
    
    bool isSameResourceModel(const IRI& instance, const IRI& otherInstance);

    /**
     * Generate a combination list based on actor interface
     */
    InterfaceCombinationList generateInterfaceCombinations();
};

//
///**
// *
// */
//class SequenceHandler
//{
//    operator()
//};
//
//class CombinatoricEngine
//{
//    IRIList resources;
//
//    void permutations(SequenceHandler handler, SequenceFilter filter);
//
//    void combinations(
//
//};

} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_HPP
