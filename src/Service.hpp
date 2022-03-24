#ifndef ORGANIZATION_MODEL_SERVICE_HPP
#define ORGANIZATION_MODEL_SERVICE_HPP

#include <owlapi/model/IRI.hpp>
#include <set>
#include <vector>

namespace moreorg {

class Service
{
public:
    using QualityOfService = double;
    using Duration = double;
    using Safety = double;

    Service(const owlapi::model::IRI& model)
        : mModel(model)
    {
    }

    const owlapi::model::IRI& getModel() const { return mModel; }

    bool operator<(const Service& other) const;

private:
    owlapi::model::IRI mModel;
    /// How good shall be the quality
    QualityOfService mQos;
    /// How low shall the service be provided
    Duration mDurationInS;
    /// How safe (redundancy level, etc. shall be the execution)
    Safety mSafety;
};

using ServiceList = std::vector<Service>;
using ServiceSet = std::set<Service>;

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_SERVICE_HPP
