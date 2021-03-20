#include "EnergyProviderPolicy.hpp"
#include "../facades/Robot.hpp"
#include "../vocabularies/OM.hpp"

using namespace owlapi::model;

namespace moreorg {
namespace policies {

PolicyRegistration<EnergyProviderPolicy> EnergyProviderPolicy::msRegistration;

EnergyProviderPolicy::EnergyProviderPolicy()
    : DistributionPolicy(vocabulary::OM::resolve("EnergyProviderPolicy"))
{
}

EnergyProviderPolicy::EnergyProviderPolicy(const ModelPool& pool, const OrganizationModelAsk& ask)
    : DistributionPolicy(pool, ask, vocabulary::OM::resolve("EnergyProviderPolicy"))
{
    update(pool, ask);
}

EnergyProviderPolicy::~EnergyProviderPolicy()
{}

void EnergyProviderPolicy::update(const ModelPool& modelPool, const OrganizationModelAsk& ask)
{
    if(modelPool.empty())
    {
        throw
            std::invalid_argument("owlapi::model::EnergyProviderPolicy::update "
                    " invalid argument: model pool cannot be empty");
    } else if(modelPool.numberOfInstances() == 1)
    {
        mDistribution.shares[ modelPool.begin()->first ] = 1.0;
    } else {
        // set energy consumption policy
        double fullCapacity = 0;
        std::map<IRI, double> energyCapacity;
        for(const ModelPool::value_type p :  modelPool)
        {
            const IRI& agentType = p.first;
            size_t numberOfInstances = p.second;

            facades::Robot robot = facades::Robot::getInstance(agentType, ask);

            double capacity = robot.getEnergyCapacity();
            fullCapacity += capacity*numberOfInstances;
            energyCapacity[agentType] = capacity;
        }

        for(const std::pair<IRI,double>& p : energyCapacity)
        {
            mDistribution.shares[p.first] = p.second / fullCapacity;
        }
    }
}

} // end namespace policies
} // end moreorg
