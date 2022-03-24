#ifndef ORGANIZATION_MODEL_SERVICE_HPP
#define ORGANIZATION_MODEL_SERVICE_HPP

#include <owlapi/model/IRI.hpp>
#include <set>
#include <vector>

namespace moreorg {

class Service
{
public:
    typedef double QualityOfService;
    typedef double Duration;
    typedef double Safety;

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

typedef std::vector<Service> ServiceList;
typedef std::set<Service> ServiceSet;

} // end namespace moreorg
#endif // ORGANIZATION_MODEL_SERVICE_HPP
