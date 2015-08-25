#include "Robot.hpp"
#include <organization_model/vocabularies/Robot.hpp>
#include <base/Logging.hpp>

using namespace owlapi::model;


namespace organization_model {
namespace facets {

Robot::Robot(const owlapi::model::IRI& actorModel, const OrganizationModel::Ptr& organizationModel)
    : Facet(organizationModel)
{

    try {
        mMinAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minAcceleration())->getDouble();
        mMaxAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxAcceleration())->getDouble();

        mNominalAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalAcceleration())->getDouble();
        mNominalVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalVelocity())->getDouble();
        mNominalPowerConsumption = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalPowerConsumption())->getDouble();

        mMinVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minVelocity())->getDouble();
        mMaxVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxVelocity())->getDouble();
        mMass = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::mass())->getDouble();
        mSupplyVoltage = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::supplyVoltage())->getDouble();
        mEnergyCapacity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::energyCapacity())->getDouble();
    } catch(const std::exception& e)
    {
        LOG_WARN_S << e.what();

        throw std::runtime_error("organization_model::facets::Robot loading of model '" + actorModel.toString() + "' failed: " + e.what());
    }
}

double Robot::estimatedEnergyCost(double distanceInM) const
{
    return mNominalPowerConsumption*distanceInM/mNominalVelocity;
}

double Robot::estimatedRelativeEnergyCost(double distanceInM) const
{
    return estimatedEnergyCost(distanceInM) / mEnergyCapacity;
}


} // end namespace facets
} // end namespace organization_model
