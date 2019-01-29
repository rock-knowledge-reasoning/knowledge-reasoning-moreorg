#include "Robot.hpp"
#include <organization_model/vocabularies/OM.hpp>
#include <organization_model/vocabularies/Robot.hpp>
#include <base-logging/Logging.hpp>

using namespace owlapi::model;


namespace organization_model {
namespace facades {

std::map<ModelPool, Robot> Robot::msRobots;

const Robot& Robot::getInstance(const owlapi::model::IRI& actorModel, const OrganizationModelAsk& organizationModelAsk)
{
    ModelPool modelPool;
    modelPool[actorModel] = 1;
    return getInstance(modelPool, organizationModelAsk);
}

const Robot& Robot::getInstance(const ModelPool& modelPool, const OrganizationModelAsk& organizationModelAsk)
{
    std::map<ModelPool, Robot>::const_iterator cit = msRobots.find(modelPool);
    if(cit != msRobots.end())
    {
        return cit->second;
    } else {
        Robot robot(modelPool, organizationModelAsk);
        msRobots[modelPool] = robot;
        return msRobots[modelPool];
    }
}

Robot::Robot()
    : Facade()
{}

Robot::Robot(const owlapi::model::IRI& actorModel, const OrganizationModelAsk& organizationModelAsk)
    : Facade(organizationModelAsk)
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
    , mPowerSourceCapacity(0.0)
    , mNominalPowerConsumption(0.0)
    , mTransportCapacity(0)
    // A physical item has always a transport demand (here assuming at least 1 standard unit)
    , mTransportDemand(1)
{

    mModelPool[actorModel] = 1;
    mEnergyProviderPolicy = policies::EnergyProviderPolicy(mModelPool, mOrganizationModelAsk);
    mTransportProviderPolicy = policies::TransportProviderPolicy(mModelPool, mOrganizationModelAsk);

    mNominalPowerConsumption = getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalPowerConsumption(), 0.0);
    mMass = getDoubleValueOrDefault(actorModel, vocabulary::Robot::mass(), 0.0);
    mSupplyVoltage = getDoubleValueOrDefault(actorModel, vocabulary::Robot::supplyVoltage(), 0.0);
    mPowerSourceCapacity = getDoubleValueOrDefault(actorModel, vocabulary::Robot::powerSourceCapacity(), 0.0);
        // convert from Ah (battery capacity to Ws)
    mEnergyCapacity = mPowerSourceCapacity*mSupplyVoltage*3600;

    mMinAcceleration = getDoubleValueOrDefault(actorModel, vocabulary::Robot::minAcceleration(),0.0);
    mMaxAcceleration = getDoubleValueOrDefault(actorModel, vocabulary::Robot::maxAcceleration(),0.0);
    mNominalAcceleration = getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalAcceleration(),0.0);


    mMinVelocity = getDoubleValueOrDefault(actorModel, vocabulary::Robot::minVelocity(),0.0);
    mMaxVelocity = getDoubleValueOrDefault(actorModel, vocabulary::Robot::maxVelocity(),0.0);
    mNominalVelocity = getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalVelocity(),0.0);


    mTransportDemand = getDoubleValueOrDefault(actorModel, vocabulary::Robot::transportDemand(),1.0);
    // get cardinality constraint
    mTransportCapacity = getDoubleValueOrDefault(actorModel, vocabulary::Robot::transportCapacity(),0.0);
}

Robot::Robot(const ModelPool& modelPool, const OrganizationModelAsk& organizationModelAsk)
    : Facade(organizationModelAsk)
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
    , mPowerSourceCapacity(0.0)
    , mNominalPowerConsumption(0.0)
    , mTransportCapacity(0)
    // A physical item has always a transport demand (here assuming at least 1 standard unit)
    , mTransportDemand(1)
    , mEnergyProviderPolicy(modelPool, organizationModelAsk)
    , mTransportProviderPolicy(modelPool, organizationModelAsk)
{

    for(const ModelPool::value_type& pair : mModelPool)
    {
        const owlapi::model::IRI& actorModel = pair.first;
        size_t modelCount = pair.second;
        mNominalPowerConsumption +=
            modelCount*getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalPowerConsumption(),0.0);
        mMass += modelCount*getDoubleValueOrDefault(actorModel, vocabulary::Robot::mass(),0.0);
        // TODO: check consistency of supply voltage
        mSupplyVoltage = std::max(getDoubleValueOrDefault(actorModel, vocabulary::Robot::supplyVoltage(),0.0), mSupplyVoltage);
        mPowerSourceCapacity += getDoubleValueOrDefault(actorModel, vocabulary::Robot::powerSourceCapacity(),0.0);

        // optional base mobility
        mMinAcceleration = std::max(mMinAcceleration,
                getDoubleValueOrDefault(actorModel, vocabulary::Robot::minAcceleration(),0.0));
        mMaxAcceleration = std::max(mMaxAcceleration,
                getDoubleValueOrDefault(actorModel, vocabulary::Robot::maxAcceleration(),0.0));
        mNominalAcceleration = std::max(mNominalAcceleration,
                getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalAcceleration(),0.0));

        mMinVelocity = std::max(mMinVelocity, getDoubleValueOrDefault(actorModel, vocabulary::Robot::minVelocity(),0.0));
        mMaxVelocity = std::max(mMaxVelocity, getDoubleValueOrDefault(actorModel, vocabulary::Robot::maxVelocity(),0.0));
        mNominalVelocity = std::max(mNominalVelocity, getDoubleValueOrDefault(actorModel, vocabulary::Robot::nominalVelocity(),0.0));

        mTransportDemand += modelCount*getDoubleValueOrDefault(actorModel, vocabulary::Robot::transportDemand(),0.0);
        // get cardinality constraint
        mTransportCapacity += modelCount*getDoubleValueOrDefault(actorModel, vocabulary::Robot::transportCapacity(),0.0);
    }

    mEnergyCapacity = mPowerSourceCapacity*mSupplyVoltage*3600;
    // Assume there is only a single system transporting
    mTransportCapacity = mTransportCapacity - mModelPool.numberOfInstances() + 1;
}


uint32_t Robot::getTransportCapacity(const owlapi::model::IRI& model) const
{
    uint32_t capacity = 0;
    if(mModelPool.isAtomic())
    {
        const owlapi::model::IRI& actorModel = mModelPool.getAtomic();
        std::vector<OWLCardinalityRestriction::Ptr> restrictions = organizationAsk().ontology().getCardinalityRestrictionsForTarget(actorModel, vocabulary::OM::hasTransportCapacity(), model);
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
                    throw std::runtime_error("organization_model::facades::Robot::getTransportCapacity: expected max cardinality restriction, but got '" + OWLCardinalityRestriction::CardinalityRestrictionTypeTxt[r->getCardinalityRestrictionType()] + "'");
            }
        }
    } else {
        throw std::runtime_error("facades::Robot::getTransportCapacity: not implemented for composite systems");

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

double Robot::estimatedEnergyCostFromTime(double timeInS) const
{
    // Watt * s
    // 1 Wattsekunde = 1/3600 Wh => Wh = Ws*3600
    //
    return (mNominalPowerConsumption*timeInS);
}

double Robot::estimatedEnergyCost(double distanceInM) const
{
    // Watt * m/ (m/s)
    // 1 Wattsekunde = 1/3600 Wh => Wh = Ws*3600
    //
    return estimatedEnergyCostFromTime( distanceInM/mNominalVelocity);
}

double Robot::estimatedRelativeEnergyCost(double distanceInM) const
{
    return estimatedEnergyCost(distanceInM) / mEnergyCapacity;
}


const std::map<IRI, double>& Robot::getEnergyProviderShares() const
{
    return mEnergyProviderPolicy.getSharesByType();
}

const ModelPool& Robot::getTransportProvider() const
{
    return mTransportProviderPolicy.getActiveTransportProviders();
}


std::string Robot::toString(size_t indent) const
{
    std::string hspace(indent,' ');
    std::stringstream ss;
    ss << hspace << "Robot: " << mModelPool.toString(indent + 4) << std::endl;
    ss << hspace << "    mass (kg):                     " << mMass << std::endl;
    ss << hspace << "    supply voltage (V):            " << mSupplyVoltage << std::endl;
    ss << hspace << "    energy capacity (Wh):          " << mEnergyCapacity/3600.0 << std::endl;
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

double Robot::getDataPropertyValue(const owlapi::model::IRI& property,
        algebra::CompositionFunc cf) const
{
    std::map<owlapi::model::IRI,double> values;
    for(const ModelPool::value_type& pair : mModelPool)
    {
        const owlapi::model::IRI& actorModel = pair.first;
        double value = organizationAsk().ontology().
            getDataValue(actorModel, property)->getDouble();
        values[actorModel] = value;
    }

    return cf(mModelPool, values);
}

double Robot::getPropertyValue(const owlapi::model::IRI& property) const
{
    try {
        double value = getDataPropertyValue(property,
            bind(&algebra::CompositionFunction::weightedSum,placeholder::_1,placeholder::_2));
        return value;
    } catch(const std::runtime_error& e)
    {
        // data property does not exist or value is not set
    }

    using namespace owlapi::model;
    std::vector<OWLCardinalityRestriction::Ptr> restrictions = organizationAsk().getCardinalityRestrictions(mModelPool,
            property,
            OWLCardinalityRestriction::SUM_OP
            );
    for(OWLCardinalityRestriction::Ptr restriction : restrictions)
    {
        if(restriction->getCardinalityRestrictionType() == OWLCardinalityRestriction::MAX)
        {
            return restriction->getCardinality();
        }
    }
    throw std::invalid_argument("organization_model::OrganizationModelAsk::getPropertyValue: failed to identify value for '"
            + property.toString() + "' for model pool: " + mModelPool.toString(8));

}

} // end namespace facades
} // end namespace organization_model
