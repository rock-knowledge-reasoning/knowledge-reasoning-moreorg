#ifndef ORGANIZATION_MODEL_FACADES_ROBOT_HPP
#define ORGANIZATION_MODEL_FACADES_ROBOT_HPP

#include "Facade.hpp"
#include "../algebra/CompositionFunction.hpp"

#include "../policies/EnergyProviderPolicy.hpp"
#include "../policies/TransportProviderPolicy.hpp"

namespace organization_model {
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
      * Get energy provider share by agent type (for a full capacity)
      */
    const std::map<owlapi::model::IRI, double>& getEnergyProviderShares() const;

    /**
      * Identify the agents that are relevant for the transport systems
      * \return modelpool of agents that are relevant for the transport system
      */
    const ModelPool& getTransportProvider() const;

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
     * Get the numeric value for a particular property using a
     * composition function. The default is a weightedSum of the
     * numericProperty value
     * \param property Name of the property
     * \param cf CompositionFunction of the numeric property values
     */
    double getDataPropertyValue(const owlapi::model::IRI& property,
            algebra::CompositionFunc cf =
            bind(&algebra::CompositionFunction::weightedSum,placeholder::_1,placeholder::_2)) const;

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

    // Robot cache
    static std::map<ModelPool, Robot> msRobots;

    // Active policies
    policies::EnergyProviderPolicy mEnergyProviderPolicy;
    policies::TransportProviderPolicy mTransportProviderPolicy;
};

} // end namespace facades
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACADES_ROBOT_HPP
