#ifndef ORGANIZATION_MODEL_FACETS_ROBOT_HPP
#define ORGANIZATION_MODEL_FACETS_ROBOT_HPP

#include <organization_model/facets/Facet.hpp>

namespace organization_model {
namespace facets {

class Robot : public Facet
{

public:
    Robot(const owlapi::model::IRI& actorModel, const OrganizationModel::Ptr& organizationModel);

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
     * Available Energy W
     */
    double getEnergyCapacity() const { return mEnergyCapacity; }
    uint32_t getPayloadTransportCapacity() const { return mPayloadTransportCapacity; }

    /**
     * Empirical information
     * P_n = W/t
     */
    double getNominalPowerConsumption() const { return mNominalPowerConsumption; }

    /**
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
     * Create a string object that list all system
     * charateristics
     */
    std::string toString() const;

private:
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
    double mNominalPowerConsumption;

    uint32_t mPayloadTransportCapacity;

    owlapi::model::IRI mActorModel;
};

} // end namespace facets
} // end namespace organization_model
#endif // ORGANIZATION_MODEL_FACETS_ROBOT_HPP
