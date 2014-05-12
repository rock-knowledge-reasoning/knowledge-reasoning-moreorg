#ifndef OWL_OM_ORGANIZATION_MODEL_HPP
#define OWL_OM_ORGANIZATION_MODEL_HPP

#include <owl_om/KnowledgeBase.hpp>

namespace owl_om {

class OrganizationModel
{
    KnowledgeBase mKnowledgeBase;

public:

    enum Entities { ACTOR, SERVICE, INTERFACE, COMPATIBILITY, END_OF_ENTITIES };
    static std::map<Entities, IRI> EntitiesIRIs;

    enum Properties { HAS, PROVIDES, DEPENDS_ON, USES, END_OF_PROPERTIES };
    static std::map<Properties, IRI> PropertiesIRIs;

    KnowledgeBase& knowledgeBase() { return mKnowledgeBase; }

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
    IRIList checkIfCompatibleNow(const IRI& instance, const IRI& otherInstance);

    /**
     * Run inference to identify service that are 'provided'
     */
    void runInferenceEngine();

    void save(const std::string& filename);

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

};

} // end namespace owl_om
#endif // OWL_OM_ORGANIZATION_MODEL_HPP
