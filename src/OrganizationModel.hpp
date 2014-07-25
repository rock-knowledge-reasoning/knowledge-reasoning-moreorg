#ifndef OWL_OM_ORGANIZATION_MODEL_HPP
#define OWL_OM_ORGANIZATION_MODEL_HPP

#include <stdint.h>
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

    IRIList parents;

    void addParent(const IRI& parent);
    bool sameParents(const InterfaceConnection& other) const;

    /**
     * Test if the two interface connections use the same interface
     */
    bool useSameInterface(const InterfaceConnection& other) const;

    bool operator<(const InterfaceConnection& other) const;

    std::string toString() const;
};

typedef std::vector< InterfaceConnection > InterfaceConnectionList;
typedef std::vector< InterfaceConnectionList > InterfaceCombinationList;

std::ostream& operator<<(std::ostream& os, const InterfaceConnection& connection);
std::ostream& operator<<(std::ostream& os, const InterfaceConnectionList& list);
std::ostream& operator<<(std::ostream& os, const InterfaceCombinationList& list);

class OrganizationModel
{
    Ontology::Ptr mpOntology;

    IRI createNewActor(const IRISet& actorSet, const InterfaceConnectionList& interfaceConnections, uint32_t id);

    /**
     * Infer new instance of a given class type, base on checking whether a given model
     * is fulfilled
     * \param actor Actor to perform inference for, actor will in effect have two types of relationships:
     *     (a) provides model
     *     (b) has instance of the class type  (where instance is modelledBy the given model)
     * \param classTypeOfInferred Class type of the inferred instances
     * \param models List of models that should be check
     */
    IRIList infer(const IRI& actor, const IRI& classTypeOfInferred, const IRIList& models);

public:

    /**
     * Constructor for an empty OrganizationModel
     */
    OrganizationModel();

    /**
     * Constructor to create an OrganizationModel from an existing description file
     * \param filename File name to an rdf/xml formatted ontology description file
     */
    OrganizationModel(const std::string& filename);

    /**
     * Update the organization model
     */
    void refresh();

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

    /**
     * Reduce list of actor to unique individuals, i.e. removing
     * alias and same individuals
     */
    IRIList compactActorList();

    IRI getResourceModel(const IRI& instance) const;

    bool isSameResourceModel(const IRI& instance, const IRI& otherInstance);

    /**
     * Create new instance (ABox from existing model (also ABox)
     * Model will be associated via modelledBy relation and subclassed from
     * classType
     * \param classType New instance will be subclass of this type
     * \param model New instance will be modelledBy this model
     * \param createDependantResources create dependants objects if required
     * \return IRI to new model
     */
    IRI createNewFromModel(const IRI& classType, const IRI& model, bool createDependantResources = false) const;

    /**
     * Generate a combination list based on actor interface
     */
    InterfaceCombinationList generateInterfaceCombinations();
};

} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_HPP
