#include "Robot.hpp"
#include <organization_model/vocabularies/Robot.hpp>

using namespace owlapi::model;


namespace organization_model {
namespace facets {

Robot::Robot(const owlapi::model::IRI& actorModel, const OrganizationModel::Ptr& organizationModel)
    : Facet(organizationModel)
{

    mMinAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minAcceleration())->getDouble();
    mMaxAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxAcceleration())->getDouble();

    mNominalAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalAcceleration())->getDouble();
    mMinVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minVelocity())->getDouble();
    mMaxVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxVelocity())->getDouble();
    mNominalVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalVelocity())->getDouble();
    mMass = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::mass())->getDouble();
    mSupplyVoltage = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::supplyVoltage())->getDouble();
    mEnergy = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::energy())->getDouble();
    mEnergyCapacity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::energyCapacity())->getDouble();
    mNominalPowerConsumption = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalPowerConsumption())->getDouble();
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
