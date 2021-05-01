#include "Robot.hpp"

#include <base-logging/Logging.hpp>
#include <regex>
#include <sstream>

#include <muParser.h>

#include "../vocabularies/OM.hpp"
#include "../vocabularies/Robot.hpp"
#include "../InferenceRule.hpp"

#include "../policies/DistributionPolicy.hpp"
#include "../policies/SelectionPolicy.hpp"

using namespace owlapi::model;


namespace moreorg {
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
        if(modelPool.numberOfInstances() == 1)
        {
            Robot robot( modelPool.compact().begin()->first,
                    organizationModelAsk);
            msRobots[modelPool] = robot;
        } else {
            Robot robot(modelPool, organizationModelAsk);
            msRobots[modelPool] = robot;
        }
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
    , mTransportMass(-1)
    , mTransportVolume(-1)
    , mLoadArea(-1)
{

    mModelPool[actorModel] = 1;

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

    // Custom properties that should be added by domain specific rules
    mProperties[ vocabulary::OM::resolve("loadAreaSize") ] = getLoadArea();

    updateManipulationProperties();
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
    , mTransportMass(-1)
    , mTransportVolume(-1)
    , mLoadArea(-1)
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

    // derived property
    mEnergyCapacity = mPowerSourceCapacity*mSupplyVoltage*3600;

    // Assume there is only a single system transporting
    mTransportCapacity = mTransportCapacity - mModelPool.numberOfInstances() + 1;

    // Custom properties that should be added by domain specific rules
    mProperties[ vocabulary::OM::resolve("loadAreaSize") ] = getLoadArea();
    updateManipulationProperties();
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
                    throw std::runtime_error("moreorg::facades::Robot::getTransportCapacity: expected max cardinality restriction, but got '" + OWLCardinalityRestriction::CardinalityRestrictionTypeTxt[r->getCardinalityRestrictionType()] + "'");
            }
        }
    } else {
        throw std::runtime_error("facades::Robot::getTransportCapacity: not implemented for composite systems");

    }
    return capacity;
}

double Robot::getTransportMass() const
{
    if(mTransportMass < 0)
    {
        mTransportMass = 0.0;
        for(const ModelPool::value_type& v : mModelPool)
        {
            const owlapi::model::IRI& actorModel = v.first;
            size_t modelCount = v.second;

            Robot r = Robot::getInstance(v.first, mOrganizationModelAsk);
            if(r.isMobile() ||  r.canTrail())
            {
                mTransportMass += modelCount*getDoubleValueOrDefault(actorModel,
                        vocabulary::OM::transportMass(), 0.0);
            }
        }
    }
    return mTransportMass;
}

double Robot::getTransportVolume() const
{
    if(mTransportVolume < 0)
    {
        throw
            std::runtime_error("moreorg::facades::Robot::getTransportVolume:"
                    " not implemented");
    }
    return mTransportVolume;
}


double Robot::getLoadArea() const
{
    if(mLoadArea < 0)
    {
        mLoadArea = 0.0;
        if(mModelPool.numberOfInstances() == 1)
        {
            mLoadArea = getLoadAreaSize( mModelPool.compact().begin()->first);
        } else {
            double truckAndTrail = 0.0;
            for(const ModelPool::value_type& v : mModelPool)
            {
                const owlapi::model::IRI& actorModel = v.first;
                size_t modelCount = v.second;

                Robot r = Robot::getInstance(actorModel, mOrganizationModelAsk);
                if(r.hasLoadArea())
                {
                    double loadAreaSurface = getLoadAreaSize(actorModel);
                    if(r.isMobile() || r.canTrail())
                    {
                        truckAndTrail += modelCount*loadAreaSurface;
                    } else {
                        mLoadArea = std::max(mLoadArea, modelCount*loadAreaSurface);
                    }
                }
            }
            mLoadArea  = std::max(truckAndTrail, mLoadArea);
        }
    }
    return mLoadArea;
}

void Robot::updateManipulationProperties()
{
    for(const ModelPool::value_type& v : mModelPool)
    {
        const owlapi::model::IRI& actorModel = v.first;

        // Identify manipulators on the system
        std::map<IRI, std::map<IRI, double> > manipulators = mOrganizationModelAsk.getPropertyValues(actorModel, vocabulary::OM::resolve("Manipulator"));

        for(const std::map<IRI, std::map<IRI, double> >::value_type v : manipulators)
        {
            const std::map<IRI, double>& properties = v.second;
            for(const std::pair<IRI, double>& p : properties)
            {
                if(vocabulary::OM::resolve("minPickingHeight") == p.first)
                {
                    updateProperty(p.first, p.second, true);
                } else if(vocabulary::OM::resolve("maxPickingHeight") == p.first)
                {
                    updateProperty(p.first, p.second, false);
                } else if(vocabulary::OM::resolve("maxPayloadMass") == p.first)
                {
                    updateProperty(p.first, p.second, false);
                }
            }
        }
    }
}

void Robot::updateProperty(const owlapi::model::IRI& iri, double value, bool useMin)
{
    double existingValue = mProperties[iri];
    if(useMin)
    {
        mProperties[iri] = std::min(value, existingValue);
    } else {
        mProperties[iri] = std::max(value, existingValue);
    }
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


policies::Distribution Robot::getDistribution(const IRI& policyName) const
{
    policies::DistributionPolicy::Ptr policy =
        dynamic_pointer_cast<policies::DistributionPolicy>(Policy::getInstance(policyName,
                    mOrganizationModelAsk));
    return policy->apply(mModelPool, mOrganizationModelAsk);

}

policies::Selection Robot::getSelection(const IRI& policyName) const
{
    policies::SelectionPolicy::Ptr policy =
        dynamic_pointer_cast<policies::SelectionPolicy>(Policy::getInstance(policyName,
                    mOrganizationModelAsk));

    return policy->apply({ Agent(mModelPool) }, mOrganizationModelAsk);
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

bool Robot::canTrail() const
{
    owlapi::model::IRI trail = vocabulary::OM::resolve("Trail");
    bool supporting = false;
    if(mModelPool.numberOfInstances() == 1)
    {
        supporting = organizationAsk().isSupporting(mModelPool, trail);
    } else {
        throw std::runtime_error("moreorg::facades::Robot::canTrail:"
                " not implemented for composite agents");
    }
    return supporting;
}

bool Robot::canManipulate() const
{
    owlapi::model::IRI manipulationProvider = vocabulary::OM::resolve("ManipulationProvider");
    return organizationAsk().isSupporting(mModelPool, manipulationProvider);
}

bool Robot::hasLoadArea() const
{
    owlapi::model::IRI loadArea = vocabulary::OM::resolve("LoadAreaProvider");
    bool supporting = organizationAsk().isSupporting(mModelPool, loadArea);
    return supporting;
}

double Robot::getAtomicDataPropertyValue(const owlapi::model::IRI& property)
    const
{
    size_t numberOfInstances = mModelPool.numberOfInstances();
    if(numberOfInstances != 1)
    {
        throw std::runtime_error("facades::Robot::getAtomicDataPropertyValue:"
                " expected atomic agent, but got one of size" +
                std::to_string(numberOfInstances));
    }

    const owlapi::model::IRI& actorModel = mModelPool.begin()->first;
    return organizationAsk().ontology().
         getDataValue(actorModel, property)->getDouble();
}

double Robot::getCompositeDataPropertyValue(const owlapi::model::IRI& property,
        algebra::CompositionFunc cf) const
{
    std::map<owlapi::model::IRI,double> values;
    for(const ModelPool::value_type& pair : mModelPool)
    {
        const owlapi::model::IRI& actorModel = pair.first;
        ModelPool m;
        m[actorModel] = 1;
        Robot r = Robot::getInstance(m, organizationAsk());
        double value =
            r.getDataPropertyValue(property);
        values[actorModel] = value;
    }

    return cf(mModelPool, values);
}

double Robot::getDataPropertyValue(const owlapi::model::IRI& property) const
{
    if(mModelPool.numberOfInstances() <= 1)
    {
        try {
            InferenceRule::Ptr rule = InferenceRule::loadAtomicAgentRule(property, organizationAsk());
            return rule->apply(*this);
        } catch(const std::invalid_argument& e)
        {}

        return getAtomicDataPropertyValue(property);
    } else {
        try {
            // Load inference rule
            InferenceRule::Ptr rule = InferenceRule::loadCompositeAgentRule(property,
                    organizationAsk());
            return rule->apply(*this);
        } catch(const std::invalid_argument& e)
        {}

        return getCompositeDataPropertyValue(property, bind(&algebra::CompositionFunction::weightedSum,placeholder::_1,placeholder::_2));
    }
}

double Robot::getPropertyValue(const owlapi::model::IRI& property) const
{
    std::map<owlapi::model::IRI, double>::const_iterator cit =
        mProperties.find(property);
    if(cit != mProperties.end())
    {
        return cit->second;
    } else {
        LOG_INFO_S << "Property '" << property << "' not in cache";
    }

    try {
        double value = getDataPropertyValue(property);
        mProperties[property] = value;
        return value;
    } catch(const std::runtime_error& e)
    {
        // data property does not exist or value is not set
        LOG_WARN_S << "Data property '" << property << "' could not be computed  " << e.what();
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
    throw std::invalid_argument("moreorg::OrganizationModelAsk::getPropertyValue: failed to identify value for '"
            + property.toString() + "' for model pool: " + mModelPool.toString(8));

}


double Robot::getLoadAreaSize(const owlapi::model::IRI& agent) const
{
    owlapi::model::IRIList loadAreas =
        organizationAsk().ontology().allRelatedInstances(agent,
                vocabulary::OM::has(),
                vocabulary::OM::resolve("LoadArea"));

    double loadAreaSurface = 0.0;;
    for(const owlapi::model::IRI& loadArea :  loadAreas)
    {
        double loadAreaWidth = getDoubleValueOrDefault(loadArea,
                vocabulary::OM::nominalWidth(), 0.0);
        double loadAreaLength = getDoubleValueOrDefault(loadArea,
                vocabulary::OM::nominalLength(), 0.0);

        loadAreaSurface = loadAreaWidth*loadAreaLength;
    }
    return loadAreaSurface;
}

std::string Robot::getInferFromAnnotation(const owlapi::model::IRI& property) const
{
    using namespace owlapi::model;
    OWLAnnotationValue::Ptr value =
        organizationAsk().ontology().getAnnotationValue(property,
                vocabulary::OM::inferFrom());
    if(value)
    {
        switch(value->getObjectType())
        {
            case OWLObject::IRIType:
                return dynamic_pointer_cast<IRI>(value)->toString();
            case OWLObject::Literal:
                return dynamic_pointer_cast<OWLLiteral>(value)->toString();
            default:
                break;
        }
    }
    throw std::invalid_argument("owlapi::facades::Robot::getDerivedByAnnotation: cannot identify value for '" + property.toString() + "'");
}

bool Robot::isDerivedProperty(const owlapi::model::IRI& property) const
{
    try {
        std::string formula = getInferFromAnnotation(property);
        return true;
    } catch(const std::invalid_argument& e)
    {
        LOG_DEBUG_S << "Property: '" << property << "' is not derivedBy formula";
        return false;
    }
}

double Robot::getDerivedPropertyValue(const owlapi::model::IRI& property) const
{
    mu::Parser muParser;
    std::string formula = getInferFromAnnotation(property);
    // process formula and identify dependencies / bindings

    std::string translatedFormula = formula;
    std::string tmpFormula = formula;

    std::map<std::string, std::string> bindings;
    char a = 'a';

    while(true)
    {
        size_t pos = tmpFormula.find_first_of("iri(");
        if(pos == std::string::npos)
        {
            break;
        }
        // Find first iri(...) match
        size_t closingBracketPos = tmpFormula.find_first_of(")", pos);
        if(pos == std::string::npos)
        {
            throw
                std::invalid_argument("moreorg::facades::Robot::getDerivedPropertyValue:"
                        " formula for '" + property.toString() + "' misses a"
                        " closing bracket: " + tmpFormula.substr(pos));
        }

        // Extract found iri
        std::string iri = tmpFormula.substr(pos+4,closingBracketPos-pos-4);
        // prepare for search of next iri(...)
        tmpFormula = tmpFormula.substr(closingBracketPos);

        // Extract the bindings for this formula
        size_t matchPos = translatedFormula.find(iri);
        size_t startPos = matchPos - 4;
        size_t endPos = matchPos + iri.size() + 1;
        std::stringstream ss;
        ss << "__" << a++ << "__";
        std::string binding = ss.str();

        translatedFormula = translatedFormula.replace(startPos,endPos-startPos,
                binding);
        bindings[binding] = iri;
    }

    std::map<std::string, double> valueBindings;
    for(const auto& p : bindings)
    {
        valueBindings[p.first] = getPropertyValue(p.second);
        muParser.DefineVar(p.first, &valueBindings[p.first]);
    }

    muParser.SetExpr(translatedFormula);
    return muParser.Eval();
}


} // end namespace facades
} // end namespace moreorg
