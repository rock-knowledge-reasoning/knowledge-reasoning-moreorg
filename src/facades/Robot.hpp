#ifndef ORGANIZATION_MODEL_FACADES_ROBOT_HPP
#define ORGANIZATION_MODEL_FACADES_ROBOT_HPP

#include "Facade.hpp"
#include "../algebra/CompositionFunction.hpp"
#include "../Policy.hpp"

namespace moreorg {
    class Agent;
namespace policies {
    struct Distribution;
}
}

namespace moreorg {
namespace facades {

/**
 * A facade for individual and composite robotic systems (aka atomic and
 * composite physical agents)
 */
class Robot : public Facade
{
public:

    static const Robot& getInstance(const owlapi::model::IRI& actorModel,
            const OrganizationModelAsk& ask);

    static const Robot& getInstance(const ModelPool& modelPool,
            const OrganizationModelAsk& ask);

    /**
     * Constructor to allow usage in maps
     */
    Robot();

    /**
     * Facade constructor for an atomic system
     */
    Robot(const owlapi::model::IRI& actorModel, const OrganizationModelAsk& organizationModelAsk);

    /**
     * Facade constructor for a composite system
     */
    Robot(const ModelPool& modelPool, const OrganizationModelAsk& organizationModelAsk);

    /**
     * Get the model pool behind the robot facade
     * \return ModelPool
     */
    const ModelPool& getModelPool() const { return mModelPool; }

    double getMinAcceleration() const { return mMinAcceleration; }
    double getMaxAcceleration() const { return mMaxAcceleration; }
    /**
     * a_n = 0
     */
    double getNominalAcceleration() const { return mNominalAcceleration; }

    double getMinVelocity() const { return mMinVelocity; }
    double getMaxVelocity() const { return mMaxVelocity; }
    /**
     * v_n
     */
    double getNominalVelocity() const { return mNominalVelocity; }

    double getMass() const { return mMass; }

    double getSupplyVoltage() const { return mSupplyVoltage; }

    double getEnergy() const { return mEnergy; }

    /**
     * Available power source capacity in Ah
     * The associated power source capacity in Wh
     */
    double getPowerSourceCapacity() const { return mPowerSourceCapacity; }

    /**
     * Available energy capacity in Ws
     * The associated power source's energy capacity in Ws,
     * this function returns batteries' capacity[Ah]*supply
     * voltage[V]*3600 => [Ws]
     */
    double getEnergyCapacity() const { return mEnergyCapacity; }

    /**
     * Get the payload transport demand
     */
    uint32_t getTransportDemand() const { return mTransportDemand; }

    /**
     * the overal transport capacity for other agents (without further
     * specification)
     */
    uint32_t getTransportCapacity() const { return mTransportCapacity; }

    /**
     * The transport capacity of this agent for a particular other agent model
     */
    uint32_t getTransportCapacity(const owlapi::model::IRI& model) const;

    /**
     * Get the available mass capacity for transport
     */
    double getTransportMass() const;

    /**
     * Get the available volume for transport
     */
    double getTransportVolume() const;

    /**
     * Get the available surface area for transport
     */
    double getLoadArea() const;

    /**
     * For compatibilty reasons provide supplyDemand: positive value represent
     * an available transport capacity while negative value a transport demand
     */
    int32_t getTransportSupplyDemand() const;

    /**
     * Empirical information on average electrical current consumption
     * Watt
     * P = U*I = V*A --  V*Ah/s
     * P_n = W/t
     *
     * 1 Watt = 1 kg m3/s3
     *
     */
    double getNominalPowerConsumption() const { return mNominalPowerConsumption; }

    /**
     * Energy cost in Ws
     */
    double estimatedEnergyCostFromTime(double timeInS) const;

    /**
     * Account for the energy cost, caused by nominal travel
     * s = v*t <=> t = s/v_n
     *
     * W = P_n*t = P_n*s/v_n
     *
     * \return energy cost in Wh
     */
    double estimatedEnergyCost(double distanceInM) const;

    /**
     * Energy cost relative to the total capacity
     */
    double estimatedRelativeEnergyCost(double distanceInM) const;

    /**
      * Get share by agent type according to a given policy (by name)
      */
    policies::Distribution getDistribution(const owlapi::model::IRI& policyName) const;

    /**
      * \return modelpool of agents that are relevant according to the given
      * policyname
      */
    std::set<Agent> getSelection(const owlapi::model::IRI& policyName) const;

    /**
     * Create a string object that list all system
     * charateristics
     */
    std::string toString(size_t indent = 0) const;

    /**
     * Return if robot is mobile
     */
    bool isMobile() const;

    /**
     * Return if robot can trail if operating in a composite system
     * which is overall mobile
     */
    bool canTrail() const;

    /**
     * Return true if robot can manipulate
     */
    bool canManipulate() const;

    /**
     * Return true if robot has a load area
     */
    bool hasLoadArea() const;

    /**
     * Get the numeric value for a particular property using a
     * composition function. The default is a weightedSum of the
     * numericProperty value
     * \param property Name of the property
     * \param cf CompositionFunction of the numeric property values
     */
    double getAtomicDataPropertyValue(const owlapi::model::IRI& property) const;

    double getCompositeDataPropertyValue(const owlapi::model::IRI& property,
            algebra::CompositionFunc cf) const;

    double getDataPropertyValue(const owlapi::model::IRI& property) const;

    /**
     * Get the property value which is either,
     * (1) cached for the predefined values
     * (2) extracted as weighted sum for custom data properties
     * (3) extracted via summed cardinalities, when cardinality restrictions
     *     apply to this property
     */
    double getPropertyValue(const owlapi::model::IRI& property) const;

private:

    ModelPool mModelPool;

    double mMinAcceleration;
    double mMaxAcceleration;
    double mNominalAcceleration;

    double mMinVelocity;
    double mMaxVelocity;
    double mNominalVelocity;

    double mMass;
    double mSupplyVoltage;
    double mEnergy;
    double mEnergyCapacity;
    double mPowerSourceCapacity;
    double mNominalPowerConsumption;

    uint32_t mTransportCapacity;
    uint32_t mTransportDemand;

    /// The permitted maximum mass of transport
    mutable double mTransportMass;
    mutable double mTransportVolume;
    /// The available load area volume (for arbitrary components)
    mutable double mLoadArea;

    mutable std::map<owlapi::model::IRI, double> mProperties;

    // Robot cache
    static std::map<ModelPool, Robot> msRobots;


    double getLoadAreaSize(const owlapi::model::IRI& agent) const;

    /// Try to identify all encountered manipulators in the system
    void updateManipulationProperties();

    void updateProperty(const owlapi::model::IRI& iri, double value, bool useMin);

    /**
     * Extract the drivedBy AnnotationProperty, if set
     */
    std::string getInferFromAnnotation(const owlapi::model::IRI& property) const;
};

} // end namespace facades
} // end namespace moreorg
#endif // ORGANIZATION_MODEL_FACADES_ROBOT_HPP
