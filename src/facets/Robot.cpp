#include "Robot.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/vocabularies/Robot.hpp>
#include <base-logging/Logging.hpp>

using namespace owlapi::model;


namespace organization_model {
namespace facets {

Robot::Robot(const owlapi::model::IRI& actorModel, const OrganizationModelAsk& organizationModelAsk)
    : Facet(organizationModelAsk)
    , mMinAcceleration(0.0)
    , mMaxAcceleration(0.0)
    , mNominalAcceleration(0.0)
    , mMinVelocity(0.0)
    , mMaxVelocity(0.0)
    , mMass(0.0)
    , mSupplyVoltage(0.0)
    , mNominalVelocity(0.0)
    , mTransportCapacity(0)
    , mTransportDemand(0)
{

    mModelPool[actorModel] = 1;

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

Robot::Robot(const ModelPool& modelPool, const OrganizationModelAsk& organizationModelAsk)
    : Facet(organizationModelAsk)
    , mModelPool(modelPool)
    , mMinAcceleration(0.0)
    , mMaxAcceleration(0.0)
    , mNominalAcceleration(0.0)
    , mMinVelocity(0.0)
    , mMaxVelocity(0.0)
    , mNominalVelocity(0.0)
    , mMass(0.0)
    , mSupplyVoltage(0.0)
    , mEnergy(0.0)
    , mEnergyCapacity(0.0)
    , mNominalPowerConsumption(0.0)
    , mTransportCapacity(0)
    , mTransportDemand(0)
{

    for(const ModelPool::value_type& pair : mModelPool)
    {
        const owlapi::model::IRI& actorModel = pair.first;
        size_t modelCount = pair.second;
        try {
            mNominalPowerConsumption += modelCount*ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalPowerConsumption())->getDouble();
            mMass += modelCount*ontologyAsk().getDataValue(actorModel, vocabulary::Robot::mass())->getDouble();
            mSupplyVoltage = ontologyAsk().getDataValue(actorModel, vocabulary::Robot::supplyVoltage())->getDouble();
            mEnergyCapacity = modelCount*ontologyAsk().getDataValue(actorModel, vocabulary::Robot::energyCapacity())->getDouble();
        } catch(const std::exception& e)
        {
            LOG_WARN_S << e.what();

            throw std::runtime_error("organization_model::facets::Robot loading of model '" + actorModel.toString() + "' failed: " + e.what());
        }

        // optional base mobility
        try {
            mMinAcceleration = std::max(mMinAcceleration, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minAcceleration())->getDouble());
            mMaxAcceleration = std::max(mMaxAcceleration, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxAcceleration())->getDouble());
            mNominalAcceleration = std::max(mNominalAcceleration, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalAcceleration())->getDouble());

            mMinVelocity = std::max(mMinVelocity, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::minVelocity())->getDouble());
            mMaxVelocity = std::max(mMaxVelocity, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::maxVelocity())->getDouble());
            mNominalVelocity = std::max(mNominalVelocity, ontologyAsk().getDataValue(actorModel, vocabulary::Robot::nominalVelocity())->getDouble());
        } catch(const std::exception& e)
        {
            LOG_INFO_S << e.what();
        }

        // optional transport capability
        try {
            mTransportDemand += modelCount*ontologyAsk().getDataValue(actorModel, vocabulary::Robot::transportDemand())->getDouble();
            // get cardinality constraint
            mTransportCapacity += modelCount*ontologyAsk().getDataValue(actorModel, vocabulary::Robot::transportCapacity())->getDouble();

        } catch(const std::exception& e)
        {
            LOG_INFO_S << e.what();
        }
    }

    // Assume there is only a single system transporting
    mTransportCapacity = mTransportCapacity - mModelPool.numberOfInstances() + 1;
}


uint32_t Robot::getTransportCapacity(const owlapi::model::IRI& model) const
{
    uint32_t capacity = 0;
    if(mModelPool.isAtomic())
    {
        const owlapi::model::IRI& actorModel = mModelPool.getAtomic();
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = ontologyAsk().getCardinalityRestrictionsForTarget(actorModel, vocabulary::OM::hasTransportCapacity(), model);
        capacity = getTransportCapacity();
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
    } else {
        throw std::runtime_error("facets::Robot::getTransportCapacity: not implemented for composite systems");

    }
    return capacity;
}

int32_t Robot::getTransportSupplyDemand() const
{
    if(isMobile())
    {
        return getTransportCapacity();
    } else {
        return getTransportDemand();
    }
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

std::string Robot::toString(size_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << "Robot: " << mModelPool.toString(indent + 4) << std::endl;
    ss << hspace << "    mass (kg):                     " << mMass << std::endl;
    ss << hspace << "    supply voltage (V):            " << mSupplyVoltage << std::endl;
    ss << hspace << "    energy capacity (Wh):          " << mEnergyCapacity << std::endl;
    ss << hspace << "    nominal power consumption (W): " << mNominalPowerConsumption << std::endl;
    ss << hspace << "    min accelleration (m/s):       " << mMinAcceleration << std::endl;
    ss << hspace << "    max accelleration (m/s):       " << mMaxAcceleration << std::endl;
    ss << hspace << "    nominal accelleration (m/s):   " << mNominalAcceleration << std::endl;
    ss << hspace << "    min velocity (m/s):            " << mMinVelocity << std::endl;
    ss << hspace << "    max velocity (m/s):            " << mMaxVelocity << std::endl;
    ss << hspace << "    nominal velocity (m/s):        " << mNominalVelocity << std::endl;
    ss << hspace << "    transport demand (units):      " << mTransportDemand << std::endl;
    ss << hspace << "    transport capacity (units):    " << mTransportCapacity << std::endl;
    return ss.str();
}

bool Robot::isMobile() const
{
    owlapi::model::IRI moveTo = vocabulary::OM::resolve("MoveTo");
    bool supporting = organizationAsk().isSupporting(mModelPool, moveTo);
    return supporting;
}


} // end namespace facets
} // end namespace organization_model
