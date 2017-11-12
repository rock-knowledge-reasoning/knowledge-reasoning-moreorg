#include "Robot.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/vocabularies/Robot.hpp>
#include <base-logging/Logging.hpp>

using namespace owlapi::model;


namespace organization_model {
namespace facets {

Robot::Robot(const owlapi::model::IRI& actorModel, const OrganizationModelAsk& organizationModelAsk)
    : Facet(organizationModelAsk)
    , mActorModel(actorModel)
    , mMinAcceleration(0.0)
    , mMaxAcceleration(0.0)
    , mNominalAcceleration(0.0)
    , mMinVelocity(0.0)
    , mMaxVelocity(0.0)
    , mNominalVelocity(0.0)
    , mTransportCapacity(0)
    , mTransportDemand(0)
{

    try {
        mNominalPowerConsumption = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalPowerConsumption())->getDouble();
        mMass = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::mass())->getDouble();
        mSupplyVoltage = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::supplyVoltage())->getDouble();
        mEnergyCapacity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::energyCapacity())->getDouble();
    } catch(const std::exception& e)
    {
        LOG_WARN_S << e.what();

        throw std::runtime_error("organization_model::facets::Robot loading of model '" + actorModel.toString() + "' failed: " + e.what());
    }

    // optional base mobility
    try {
        mMinAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minAcceleration())->getDouble();
        mMaxAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxAcceleration())->getDouble();
        mNominalAcceleration = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalAcceleration())->getDouble();


        mMinVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minVelocity())->getDouble();
        mMaxVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxVelocity())->getDouble();
        mNominalVelocity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalVelocity())->getDouble();
    } catch(const std::exception& e)
    {
        LOG_INFO_S << e.what();
    }


    // optional transport capability
    try {
        mTransportDemand = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::transportDemand())->getDouble();
        // get cardinality constraint
        mTransportCapacity = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::transportCapacity())->getDouble();


    } catch(const std::exception& e)
    {
        LOG_INFO_S << e.what();
    }
}

uint32_t Robot::getTransportCapacity(const owlapi::model::IRI& model) const
{
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = ontologyAsk().getCardinalityRestrictionsForTarget(mActorModel, vocabulary::OM::hasTransportCapacity(), model);
    uint32_t capacity = getTransportCapacity();
    for(const OWLCardinalityRestriction::Ptr& r : restrictions)
    {
        switch(r->getCardinalityRestrictionType())
        {
            case OWLCardinalityRestriction::MAX:
                capacity = std::min(capacity, r->getCardinality());
                break;
            case OWLCardinalityRestriction::MIN:
            case OWLCardinalityRestriction::EXACT:
            case OWLCardinalityRestriction::UNKNOWN:
                throw std::runtime_error("organization_model::facets::Robot::getTransportCapacity: expected max cardinality restriction, but got '" + OWLCardinalityRestriction::CardinalityRestrictionTypeTxt[r->getCardinalityRestrictionType()] + "'");
        }
    }
    return capacity;
}

double Robot::estimatedEnergyCost(double distanceInM) const
{
    // W * m/ (m/s)
    // 1 Ws = 1/3600 Wh => Wh = Ws*3600
    //
    return (mNominalPowerConsumption*distanceInM/mNominalVelocity) / 3600.0;
}

double Robot::estimatedRelativeEnergyCost(double distanceInM) const
{
    return estimatedEnergyCost(distanceInM) / mEnergyCapacity;
}

std::string Robot::toString() const
{
    std::stringstream ss;
    ss << "Robot: " << mActorModel.toString() << std::endl;
    ss << "    mass (kg):                     " << mMass << std::endl;
    ss << "    supply voltage (V):            " << mSupplyVoltage << std::endl;
    ss << "    energy capacity (Wh):          " << mEnergyCapacity << std::endl;
    ss << "    nominal power consumption (W): " << mNominalPowerConsumption << std::endl;
    ss << "    min accelleration (m/s):       " << mMinAcceleration << std::endl;
    ss << "    max accelleration (m/s):       " << mMaxAcceleration << std::endl;
    ss << "    nominal accelleration (m/s):   " << mNominalAcceleration << std::endl;
    ss << "    min velocity (m/s):            " << mMinVelocity << std::endl;
    ss << "    max velocity (m/s):            " << mMaxVelocity << std::endl;
    ss << "    nominal velocity (m/s):        " << mNominalVelocity << std::endl;
    ss << "    transport demand (units):      " << mTransportDemand << std::endl;
    ss << "    transport capacity (units):    " << mTransportCapacity << std::endl;
    return ss.str();
}

bool Robot::isMobile() const
{
    owlapi::model::IRI moveTo = vocabulary::OM::resolve("MoveTo");
    bool supporting = organizationAsk().isSupporting(mActorModel, moveTo);
    return supporting;
}


} // end namespace facets
} // end namespace organization_model
